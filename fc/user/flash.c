/**
 * 文件：user/flash.c                       【角色】片内 Flash 读写：保存对码信息与传感器校准参数
 */
#include "flash.h"
#include "stdlib.h"

/**
 * 写 Flash 用的中转数组：128 字节，正好一页里的一小段。 用 uint32_t 类型是为了让 Flash 按“字”编程时地址天然对齐
 */
uint32_t flash_arr[32];
/**
 * 用户参数结构体实例：全工程共用这一份（main.c、deal_pack.c 都 extern 它）
 */
Pair_Code_Config pair_data;
/* 注意：joystick_ch_offset[] 只有 extern 声明，本工程没有定义/使用（历史遗留，未改动） */
extern int16_t joystick_ch_offset[];

/* device_ID 的定义在 deal_pack.c（那里定义成 uint8_t device_ID;），此处只是引用 */
extern uint8_t device_ID;



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
/* 打印 UID 全部字段。整个函数体被 #ifdef DEBUG 包住： 量产固件没定义 DEBUG，于是 Printf_UID() 变成空函数（不占 Flash，见编译产物）。 */
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



/* @brief  把参数结构体按字节拷进 Flash 中转数组 【参数】config：源结构体（通常是 &pair_data）；flash_arr：目标数组 【返回】无 【算法】用 uint8_t* 逐字节拷贝（结构体 → 字节流），绕开对齐与填充差异； */
void copy_config_to_flash_array(Pair_Code_Config* config, uint32_t* flash_arr) {
    uint8_t* src = (uint8_t*)config;
    uint8_t* dest = (uint8_t*)flash_arr;
    
        // 复制8字节数据到数组（占用前两个uint32_t元素）
    for (int i = 0; i < JOYSTICK_PAIR_SIZE; i++) {
        dest[i] = src[i];
    }
        // printf("%x\r\n",flash_arr[0]);
        // 剩余数组元素清零（可选）
    for (int i = JOYSTICK_PAIR_SIZE; i < 32; i++) {
        flash_arr[i] = 0;
    }
}

/* @brief  把 Flash 中转数组按字节还原成参数结构体（copy 的逆操作） 【参数】flash_arr：源数组；config：目标结构体（通常是 &pair_data） 【返回】无 【算法】同样逐字节拷贝，保证与写盘时的字节布局完全一致。 */
// 从数组还原结构体
void restore_config_from_flash_array(uint32_t* flash_arr, Pair_Code_Config* config) {
    uint8_t* src = (uint8_t*)flash_arr;
    uint8_t* dest = (uint8_t*)config;
    
    for (int i = 0; i < JOYSTICK_PAIR_SIZE; i++) {
        dest[i] = src[i];
    }
}



void flash_write(uint32_t flash_start_addr)
{
	copy_config_to_flash_array(&pair_data,flash_arr);
  HAL_FLASH_Unlock();
  uint32_t PAGEError = 0;
  FLASH_EraseInitTypeDef EraseInitStruct = {0};

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;        
  EraseInitStruct.PageAddress = flash_start_addr;                /* Starting address for erase */
  EraseInitStruct.NbPages  = (ARR_Flash_SIZE + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;          /* Number of pages to be erased */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  
  {
		// printf("擦除失败\r\n");
	return;
  }
  
   uint32_t *src_data = (uint32_t *)flash_arr;
   uint32_t address = flash_start_addr;
   
   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, address, src_data) != HAL_OK)
	 {
								// 写入失败处理
				// printf("写入失败\r\n");
	 }
	HAL_FLASH_Lock();
}

// 这个只是在开机的时候读取的
void flash_read(uint32_t flash_start_addr)
{
  uint32_t addr = 0;

  while (addr < ARR_Flash_SIZE)
  {
    flash_arr[addr / 4] = HW32_REG(flash_start_addr + addr);           /* 每次读 4 字节，下标 addr/4 正好对应数组元素 */
    addr += 4;
  }
  restore_config_from_flash_array(flash_arr,&pair_data);
	device_ID = pair_data.device_id;   		/* 同步设备 ID 给 deal_pack.c（它 extern 了 device_ID） */

}

