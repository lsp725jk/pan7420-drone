#include "board.h"
#include "led.h"
#include "adc.h"
#include "tim.h"
#include "bsp_uart.h"
#include "joystick.h"
#include "flash.h"
#include "key.h"
#include "buzzer.h"
#include "deal_data.h"
#include "bsp.h"
#include "crsf.h"
#include "tim16.h"
#include "rc_pc_ctrl.h"      /**
 * 键盘控飞机：串口接收 + 注入摇杆值（默认关闭）
 */

/**
 * 【射频遥测转发开关】默认 0（关闭）——固件行为与改动前完全一致。
 */
#define RC_TELEMETRY_FORWARD_ENABLE   0     /* ← 改成 1 启用转发 */



#include "vbat_vref.h"



// extern Pair_Code_Config pair_data;

#define EXAMPLE_NAME	"PN298 enhanced mode compatible with xn297l"
#define EXAMPLE_DATE	"2022-08-08"
#define DEMO_VER		"1.0.0"
extern uint8_t send_pn298_pack[];
#define __PN298_TX_PERIOD		5
static uint8_t tx_rx_mode = 1;	//0: 表示PN298工作于接收状态，1：表示PN298工作于发送状态
// static uint8_t temp_buff[__PN298_PAYLOAD];	//数据接收或者发送缓冲区
// static uint8_t rx_buff[__PN298_PAYLOAD];
static uint8_t ack_buff[__PN298_PAYLOAD];
// uint8_t addr_test[5] = {0xC1,0xC2,0xC3,0xC4,0xC5};
extern uint8_t pair_addr_arr[];
extern Joystick_Pair_Config joystick_pair_str;
extern uint16_t hardware_value[];
uint8_t send_pair_pack_flag = 0;

uint8_t Bind_Flag=0;
uint32_t crsfTime = 0;
uint16_t loop_count = 0;
uint8_t crsf_send_en = 0;
volatile uint32_t bat_check_time;

extern uint8_t loop50HzFlag;

extern Flip_flag_t flip_flag;



Flip_Dirs flip_dir;

extern uint8_t signal_lost_flag;
int main(void)
{
				/* 初始化所有外设，Flash接口，SysTick（内核自带的系统节拍定时器） */
	
		// APP_SystemClockConfig();
	HAL_Init();       
				/* 配置时钟 36M 修改hal_conf.h文件 */
	APP_SystemClockConfig();
	
	bsp_uart_init(115200);

		/* 键盘控飞机：把 USART2 从「只发」改成「收发双向」并配好 RX 引脚。 必须在 bsp_uart_init() 之后 —— 要改的就是它设的 Mode。 （上面 CRSF 分支里也有一处，但那个分支有自己的 while(1)， 正常遥控模式靠的是这一处。） */
	rc_pc_ctrl_init();
	Led_Init();
		// key_33_init();
		// 初始化按键
    Keypad_Init();
        // 设置按键处理回调函数
    Keypad_SetKeyHandler(HandleKeyPress);
	
	joystick_init();	//这里面包含了adc和定时器3的初始化
	
	Buzzer_Init();

// auto_calibrate_joystick_center();
	
	flash_read(FLASH_USER_START_ADDR);
				// 增加摇杆校准的功能---这个功能不对用户，只用于出产校准
	    	    // 检查是否触发校准模式(生产模式)
        // 只有未校准或强制校准触发时才进入
// printf("joystick_pair_str.calibrated  = %d\r\n",joystick_pair_str.calibrated );
    if(joystick_pair_str.calibrated != 1 || checkCalibrationTrigger()) {
		if(checkCalibrationTrigger())
		while(checkCalibrationTrigger()){}
		BUZZER_PORT->BSRR = (uint32_t)BUZZER_PIN;
		HAL_Delay(1000);
		BUZZER_PORT->BRR = (uint32_t)BUZZER_PIN;
        startCalibration();
    }

	if(is_crsf_en()){
		printf("key8\r\n");
		Led_SetState(LED2, LED_STATE_ON, 0);
		while(is_pair_key()){}
		crsf_send_en = 1;
		printf("key8 release\r\n");
		bsp_uart_init(400000);

				/* 键盘控飞机：把 USART2 从「只发」改成「收发双向」并配好 RX 引脚。 必须在 bsp_uart_init() 之后调用 —— 因为要改的就是它设的 Mode。 RC_PC_CTRL_ENABLE=0 时这是空语句，不影响原行为。 */
		rc_pc_ctrl_init();
		while(1) {
			key_crsf_scankey();
			Keypad_Scan();
			Buzzer_Update();
			if(is_crsf_en() &&	Bind_Flag == 0){
				Bind_Flag = 1;
			}
			
			if (HAL_GetTick() > crsfTime){
				ready_channels_data();	
				switch(Bind_Flag){
					case 0:
						crsfDataPacket_Send();
						break;
					case 1:
						crsfBindPacket_Send();
						if(loop_count++ > 50){
							Bind_Flag = 2;
						}
						break;
					default:
						crsfDataPacket_Send();
						break;
						
				}
								// 设置帧间隔时间
				crsfTime = HAL_GetTick() + CRSF_TIME_BETWEEN_FRAMES_US;
			}
		}
	}






	
		// Buzzer_Start(BUZZER_MODE_DOUBLE);
	BUZZER_PORT->BSRR = (uint32_t)BUZZER_PIN;
// HAL_Delay(200);
// BUZZER_PORT->BRR = (uint32_t)BUZZER_PIN;



	HAL_Delay(120);


	
		// HAL_Delay(500);
	BUZZER_PORT->BRR = (uint32_t)BUZZER_PIN;
		// LED2(LED_ON);
		// WRITE_REG(GPIOA->BRR,GPIO_PIN_3);
		// GPIOA->BRR = (uint32_t)GPIO_PIN_3;
	bsp_init();
		// flash_read(FLASH_USER_START_ADDR);
	if(is_pair_key()){
		printf("key5\r\n");
		Led_SetState(LED2, LED_STATE_BLINKING, 500);
		while(is_pair_key()){}
		printf("key5 release\r\n");
		
				// calibrate_joystick_max();
		init_random_seed();
		Get_Pair_Code();
		Set_Joystick_Data();
		bsp_pn298_tx_mode(); 
		while(1) {
// Keypad_Scan();
			Buzzer_Update();
			HAL_Delay(20);
			Led_Process();
			if(aADCxConvertedData[2]<=0 && send_pair_pack_flag != 0x01) {
				Buzzer_Start(BUZZER_MODE_SHORT);
				send_pair_pack_flag = 0x01;
			}
						// Send_Remote_Data();
			if(send_pair_pack_flag == 0x01){
				Send_Pair_Code();
				bsp_pn298_tx_data_ack(send_pn298_pack, ack_buff, __PN298_PAYLOAD);
			}
			if(aADCxConvertedData[2]>4050 && send_pair_pack_flag == 0x01){
				Buzzer_Start(BUZZER_MODE_SHORT);
				send_pair_pack_flag = send_pair_pack_flag | (1<<1);
				flash_write(FLASH_USER_START_ADDR);
				printf("send_pair_pack_flag = %x\r\n",send_pair_pack_flag);
			}
			if(send_pair_pack_flag == 0x03){
				break;
			}
						// printf("ack_buff[0] cnt:%d\r\n", ack_buff[0]);
// if(ack_buff[0]==1){
// flash_write(FLASH_USER_START_ADDR);
// printf("flash_write\r\n" );
// break;
// //				get_now_addr();
// //				bsp_pn298_change_addr(pair_addr_arr,joystick_pair_str.rf_ch);
// }
						
						// printf("Tx cnt:%d\r\n", count);
						// HAL_Delay(60);
						// delay_ms(1000);
		}
	}
	Led_SetState(LED2, LED_STATE_ON, 0);
		// Get_Pair_Code();
// Set_Joystick_Data();
		// flash_write(FLASH_USER_START_ADDR);
	flash_read(FLASH_USER_START_ADDR);
	get_now_addr();
	
	if(tx_rx_mode == 1){
		bsp_pn298_tx_mode(); 
	}
	bsp_pn298_change_addr(pair_addr_arr,joystick_pair_str.rf_ch);


	
	MX_TIM16_Init();
// HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
// HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
// int32_t accalt;
	while (1)
	{
				/* 键盘控飞机：非阻塞收串口并解析。**必须每圈无条件执行** —— 它同时负责「多久没收到帧就交还物理摇杆」的超时判断。 放在主循环最前面，保证即使在别的分支里卡住之前也先跑过它。 RC_PC_CTRL_ENABLE=0 时这是个空函数。 */
		rc_pc_ctrl_poll();
		Keypad_Scan();
		Buzzer_Update();
		Led_Process();
		if(flip_flag.start_flag){
			flip_dir = Check_Flip_Dir();
			if(flip_dir){
				memset(&flip_flag, 0, sizeof(flip_flag));
								// printf("%d\r\n",flip_dir);
			}
				
		}


		
				// check_sign_loss();
		if(loop50HzFlag) {
			loop50HzFlag = 0;
			Send_Remote_Data();
			bsp_pn298_tx_data_ack(send_pn298_pack, ack_buff, __PN298_PAYLOAD);

						/* 射频遥测转发：把飞机回传的遥测帧（ack_buff[2..11]）从 USART2 发给电脑。 必须放在这里（loop50HzFlag 块的 ACK 收发之后）—— 这才是**飞行时每 50Hz 循环执行**的通路； 对码分支里虽然也有一次 ACK 收发，但只在配对时走。 */
#if RC_TELEMETRY_FORWARD_ENABLE
			CRSF_send(&ack_buff[2], 10);
#endif
						// memcpy(&accalt,ack_buff,4);
// int32_t setVal = ack_buff[0]<<24 | ack_buff[1]<<16 | ack_buff[2]<<8 | ack_buff[3];
// int16_t setVal = ack_buff[0]<<8 | ack_buff[1];
// int16_t veltemp = ack_buff[2]<<8 | ack_buff[3];
// int32_t error = setVal - veltemp;
						// printf("%d,%d,%d\r\n", setVal,veltemp,error);
						// printf("%d\r\n", accalt);
						// HAL_Delay(60);
						// delay_ms(1000);
		}



				// Led_Process();
		if(HAL_GetTick() - bat_check_time > 500){
			battery_check();
			if(ack_buff[0]==LOW_VOL){
				if(Get_Buzzer_Statue()==BUZZER_MODE_OFF){
						Buzzer_Start(BUZZER_MODE_SHORT);
				}
			}
			bat_check_time = HAL_GetTick();
						// printf("%d\r\n",ack_buff[0]);
			
		}
		if(!signal_lost_flag)
			Led_SetState(LED2, LED_STATE_ON, 0);
		readJoystickAndBuzz();
	}
}



