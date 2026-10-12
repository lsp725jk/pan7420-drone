#include "flash.h"
#include "stdlib.h"
Joystick_Pair_Config joystick_pair_str;
uint32_t flash_arr[32];

extern int16_t joystick_ch_offset[];
extern uint16_t joystick_min[];
extern uint16_t joystick_mid[];
extern uint16_t joystick_max[];
extern uint8_t joystick_deadband[];
extern uint16_t vref_real;
extern BatCalibPoint bat_calib_table[];  // 校准表

uint8_t device_ID;

uint32_t HAL_GetUIDw3(void)
{
  return (READ_REG(*((uint32_t *)(UID_BASE + 12U))));
}

void Get_UID(PY32F030_UID_TypeDef *uid){
	uint32_t temp_uid0 = HAL_GetUIDw0();
	uint32_t temp_uid1 = HAL_GetUIDw1();
	uint32_t temp_uid2 = HAL_GetUIDw2();
	uint32_t temp_uid3 = HAL_GetUIDw3();
	for (uint8_t i=0; i<4;i++){
		uid->lotNumber[i] = (temp_uid0>>(i*8))&0xff;
	}
	uid->waferNumber = temp_uid1&0xff;
	uint8_t j = 4;
	for (uint8_t i=1; i<4;i++){
		uid->lotNumber[j++] = (temp_uid1>>(i*8))&0xff;
	} 
	uid->internalCode1 = temp_uid2&0xff;
	uid->yCoordinate = (((temp_uid2>>24)&0xf0)<<4)|((temp_uid2>>8)&0xff);
	uid->xCoordinate = (((temp_uid2>>24)&0x0f)<<8)|((temp_uid2>>16)&0xff);
	uid->fixedCode = temp_uid3&0xff;
	uid->internalCode2 = (temp_uid3>>8)&0xff;
	uid->internalCode3 = (temp_uid3>>16)&0xff;
	uid->internalCode4 = (temp_uid3>>24)&0xff;
	uid->myuin = (((temp_uid0|temp_uid1|temp_uid2)>>16)&(uid->yCoordinate|uid->xCoordinate))&0xff;
}
void Printf_UID(void){
#ifdef DEBUG
	PY32F030_UID_TypeDef uid;
	Get_UID(&uid);
    printf("=== PY32F030 UID信息 ===\r\n");
	for(uint8_t i =0;i<7;i++){
		printf("批次号: %x\r\n", uid.lotNumber[i]);
	}
    printf("晶圆编号: %u\r\n", uid.waferNumber);
    printf("X坐标: %u\r\n", uid.xCoordinate);
    printf("Y坐标: %u\r\n", uid.yCoordinate);
    printf("固定码: 0x%X\r\n", uid.fixedCode);
    printf("内部编码1: 0x%X\r\n", uid.internalCode1);
    printf("内部编码2: 0x%X\r\n", uid.internalCode2);
    printf("内部编码3: 0x%X\r\n", uid.internalCode3);
    printf("内部编码4: 0x%X\r\n", uid.internalCode4);	
	printf("myuin: 0x%X\r\n", uid.myuin);
#endif	
}

// 定义需要排除的敏感信道列表（0~79范围内）
uint8_t exclude_chs[] = {13,14,15,29,30,31,45,46,47,61,62,63,77,78,79};
uint8_t total_exclude = sizeof(exclude_chs)/sizeof(exclude_chs[0]);
// 2. 定义函数：判断某个信道是否在排除列表中
uint8_t is_excluded(uint8_t ch) {
    for(uint8_t i=0; i<total_exclude; i++) {
        if(ch == exclude_chs[i]) {
            return 1;
        }
    }
    return 0;
}

uint8_t generateAddrChannelId(void){
	 uint8_t addr_hash = 0;
	for(uint8_t i = 0;i<5;i++){
		addr_hash ^= joystick_pair_str.paircode[i];
	}
	return addr_hash ^ joystick_pair_str.rf_ch;
}

void Get_Pair_Code(void){
	PY32F030_UID_TypeDef uid;
	Get_UID(&uid);
	joystick_pair_str.write_count = 1;
	joystick_pair_str.paircode[0] = (uid.myuin + rand())%255 ;
	joystick_pair_str.paircode[1] = uid.xCoordinate&0xff;
	joystick_pair_str.paircode[2] = rand()%255;
	joystick_pair_str.paircode[3] = uid.yCoordinate&0xff;
	joystick_pair_str.paircode[4] = rand()%255;
	for(uint8_t i=0;i<4;i++){
		joystick_pair_str.rf_ch += joystick_pair_str.paircode[i];
	}
	joystick_pair_str.rf_ch = (joystick_pair_str.rf_ch + uid.waferNumber)%80;
// if(joystick_pair_str.rf_ch%16==0 | joystick_pair_str.rf_ch==36 ){
// joystick_pair_str.rf_ch +=1;
// }
		// 检查是否在敏感列表中，若是则加1（需确保加1后仍在0~79，且不再次落入敏感列表）
// 3. 循环检查：如果当前信道在排除列表中，就递增并再次检查（直到找到安全信道）
	while(is_excluded(joystick_pair_str.rf_ch)) {
		joystick_pair_str.rf_ch = (joystick_pair_str.rf_ch + 1) % 80; // 超过79则回绕到0
	}
	
	joystick_pair_str.device_id = generateAddrChannelId();
	
		// 将对频地址写进flash中
	
	#ifdef DEBUG
	printf("%x-%x-%x-%x-%x\r\n",joystick_pair_str.paircode[0],joystick_pair_str.paircode[1],joystick_pair_str.paircode[2],joystick_pair_str.paircode[3],joystick_pair_str.paircode[4]);
	printf("%x\r\n",joystick_pair_str.rf_ch);
	#endif
}



/* 这个函数一定要放到中值读取和最大值读取的后面 */
void Set_Joystick_Data(void){
	joystick_pair_str.write_count +=1; 
	for(uint8_t i=0;i<4;i++){
		joystick_pair_str.adjustValue[i] = joystick_ch_offset[i];
		joystick_pair_str.chMiddle[i] = joystick_mid[i];
		joystick_pair_str.chUpper[i] = joystick_max[i];
	}
}



void copy_config_to_flash_array(Joystick_Pair_Config* config, uint32_t* flash_arr) {
    uint8_t* src = (uint8_t*)config;
    uint8_t* dest = (uint8_t*)flash_arr;
    
        // 复制8字节数据到数组（占用前两个uint32_t元素）
    for (int i = 0; i < JOYSTICK_PAIR_SIZE; i++) {
        dest[i] = src[i];
    }
    printf("%x\r\n",flash_arr[0]);
        // 剩余数组元素清零（可选）
    for (int i = JOYSTICK_PAIR_SIZE; i < 32; i++) {
        flash_arr[i] = 0;
    }
}

// 从数组还原结构体
void restore_config_from_flash_array(uint32_t* flash_arr, Joystick_Pair_Config* config) {
    uint8_t* src = (uint8_t*)flash_arr;
    uint8_t* dest = (uint8_t*)config;
    
    for (int i = 0; i < JOYSTICK_PAIR_SIZE; i++) {
        dest[i] = src[i];
    }
}

/* @brief  Erase Flash @param  None @retval None */
void flash_write(uint32_t flash_start_addr)
{
	copy_config_to_flash_array(&joystick_pair_str,flash_arr);
  HAL_FLASH_Unlock();
  uint32_t PAGEError = 0;
  FLASH_EraseInitTypeDef EraseInitStruct = {0};

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;        
  EraseInitStruct.PageAddress = flash_start_addr;                /* Starting address for erase */
  EraseInitStruct.NbPages  = (ARR_Flash_SIZE + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;          /* Number of pages to be erased */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  
  {
	printf("擦除失败\r\n");
	return;
  }
  
   uint32_t *src_data = (uint32_t *)flash_arr;
   uint32_t address = flash_start_addr;
   
   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, address, src_data) != HAL_OK)
	 {
								// 写入失败处理
		printf("写入失败\r\n");
	 }
	HAL_FLASH_Lock();
}
// 这个只是在开机的时候读取的------可以尝试再用一个结构体
void flash_read(uint32_t flash_start_addr)
{
  uint32_t addr = 0;

  while (addr < ARR_Flash_SIZE)
  {
    flash_arr[addr / 4] = HW32_REG(flash_start_addr + addr);
    addr += 4;
  }
  restore_config_from_flash_array(flash_arr,&joystick_pair_str);
  for(uint8_t i=0;i<4;i++){
	  joystick_ch_offset[i] = joystick_pair_str.adjustValue[i] ;
	  joystick_min[i] = joystick_pair_str.chMin[i];
	  joystick_mid[i] = joystick_pair_str.chMiddle[i];
	  joystick_max[i] = joystick_pair_str.chUpper[i];
	  joystick_deadband[i] = joystick_pair_str.deadband[i];
	  
	}
	device_ID = joystick_pair_str.device_id;
	vref_real = joystick_pair_str.vref_real_mv;
	memcpy(bat_calib_table,joystick_pair_str.calib_table,sizeof(BatCalibPoint) * CALIB_POINT_NUM);  
	printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", joystick_min[0],joystick_min[1],joystick_min[2],joystick_min[3]);
	printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", joystick_mid[0],joystick_mid[1],joystick_mid[2],joystick_mid[3]);
	printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", joystick_max[0],joystick_max[1],joystick_max[2],joystick_max[3]);
	#ifdef DEBUG
	printf("%x-%x-%x-%x-%x\r\n",joystick_pair_str.paircode[0],joystick_pair_str.paircode[1],joystick_pair_str.paircode[2],joystick_pair_str.paircode[3],joystick_pair_str.paircode[4]);
	printf("%x\r\n",joystick_pair_str.write_count);
	printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", joystick_pair_str.chMiddle[0],joystick_pair_str.chMiddle[1],joystick_pair_str.chMiddle[2],joystick_pair_str.chMiddle[3]);
	printf("offest1:%d offest2:%d offest3:%d offest4:%d\r\n", joystick_ch_offset[0],joystick_ch_offset[1],joystick_ch_offset[2],joystick_ch_offset[3]);
	#endif
}



// * @brief  Program Flash
// * @param  None
// * @retval None

// void APP_FlashProgram(void)
// {



// while (flash_program_start < flash_program_end)
// {

// {



// }
// }
// }



// * @brief  Check if Flash is blank
// * @param  None
// * @retval None

// void APP_FlashBlank(void)
// {
// uint32_t addr = 0;

// while (addr < sizeof(DATA))
// {
// if (0xFFFFFFFF != HW32_REG(FLASH_USER_START_ADDR + addr))
// {
// while(1){}
// }
// addr += 4;
// }
// }



// * @brief  Verify Flash
// * @param  None
// * @retval None

// void APP_FlashVerify(void)
// {
// uint32_t addr = 0;

// while (addr < 2)
// {
// if (DATA[addr / 4] != HW32_REG(FLASH_USER_START_ADDR + addr))
// {
// while(1){}
// }
// addr += 4;
// }
// }

