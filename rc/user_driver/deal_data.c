#include "deal_data.h"
#include "rc_pc_ctrl.h"      /**
 * rc_pc_ctrl_apply()：用键盘指令覆盖摇杆值
 */
#include "flash.h"
#include "key.h"
#include "led.h"
#include "config_rf.h"
#include "crsf.h"
#include "buzzer.h"

uint8_t pair_addr_arr[5];
uint8_t send_pn298_pack[__PN298_PAYLOAD];

extern Joystick_Pair_Config joystick_pair_str;
extern uint16_t aADCxConvertedData[];
extern int16_t joystick_ch_offset[];
extern uint16_t hardware_value[];
/* 这个函数用来发送对频地址和频段的数据包 */
extern uint8_t key10_count;	//用来记录挡位的
extern uint8_t key8_count;	//用来灯光的
// extern uint8_t key9_count;
extern KeyBitStatus_t key_bit_status;

extern uint16_t rcChannels[];
extern Key_t keys[];
extern uint8_t crsf_send_en;

Flip_flag_t flip_flag;
extern Flip_Dirs flip_dir;

extern uint8_t device_ID;

uint8_t fly_status;

// 这是准备发送crsf的通道数据
// 这里的key10和key9的有点不太一样了
void ready_channels_data(void){
	joystick_filter_map();

	rc_pc_ctrl_apply();
	for(uint8_t i=0;i<4;i++){
		rcChannels[i] = hardware_value[i]; 
	}
	rcChannels[4] = keys[1].count ? 1811 : 172;
	rcChannels[5] = keys[0].count ? 1811 : 172;
	rcChannels[6] = 172 ;
	
// for(uint8_t j=0;j<7;j++){
// printf("rcChannels[%d]:%d\r\n",j,rcChannels[j]);
// }
}



void Send_Pair_Code(void){
	send_pn298_pack[0]=0xdb;
	send_pn298_pack[1]=0xdb;
	for(uint8_t j=2;j<8;j++){
		send_pn298_pack[j] = *((uint8_t *)(&joystick_pair_str.paircode)+(j-2));
	}
	send_pn298_pack[8]=joystick_pair_str.device_id;
	for(uint8_t i=9;i<__PN298_PAYLOAD;i++){
		send_pn298_pack[i] = 0;
	}
}



void get_now_addr(void){
	for(uint8_t i=0;i<5;i++){
		pair_addr_arr[i] = *((uint8_t *)(&joystick_pair_str.paircode)+i);
	}
	
	printf("%x-%x-%x-%x-%x\r\n",pair_addr_arr[0],pair_addr_arr[1],pair_addr_arr[2],pair_addr_arr[3],pair_addr_arr[4]);
	printf("rf_ch = %d\r\n",joystick_pair_str.rf_ch);
	
}

/* 这个函数是将数据进行打包同时发送数据 要有摇杆的四个数据，key10，key9，key8 key7 key6 hardware_value[]这个数组 */
void Send_Remote_Data(void){
	send_pn298_pack[0]=0xaa;
	send_pn298_pack[1]=device_ID;
	joystick_filter_map();
	for(uint8_t t=1;t<5;t++)
	{		
		send_pn298_pack[2*t] = (uint8_t)(hardware_value[t-1]>>8)&0xFF; //高8位，把u16拆分成两个u8进行传输
		send_pn298_pack[2*t+1] = (uint8_t)hardware_value[t-1]&0xFF; //低8位
	}
	send_pn298_pack[10]=key_bit_status.key_byte;
	if(flip_dir)
		send_pn298_pack[11]=0xF0 | flip_dir;
	else
		send_pn298_pack[11]=0x0;
	if(flip_flag.start_flag==0){
		flip_dir = NONE;
	}
	#ifdef DEBUG
		// printf("key8:%d key10:%d key9:%d key7:%d key6:%d\r\n",send_pn298_pack[10],send_pn298_pack[11],send_pn298_pack[12],send_pn298_pack[13],send_pn298_pack[14]);
	#endif	
}

/* 说明：仿Arduino,将一个数字从一个范围重新映射到另一个范围 也就是说，fromLow的值将映射到toLow，fromlhigh到toHigh的值等等。 参数：value：待映射的数值； fromLow：原范围的最小值 */
float map(uint16_t value,uint16_t fromLow,uint16_t fromHigh,uint16_t toLow,uint16_t toHigh)
{
	return ((value-fromLow)*(toHigh-toLow)/(fromHigh-fromLow)+toLow);
}

/* 说明：将ADC获取的采样值转换到1000~2000，lower~middle~upper适配遥杆的范围， 并加入正反开关的控制。 参数：val：该通道ADC当前采样值； lower：该通道遥杆最低位置时的ADC采样值； */
// uint16_t mapChValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, uint16_t reverse, int16_t offset)
// {

// if(val<middle+10 && val>middle-10){
// if(crsf_send_en){
// val = CRSF_DIGITAL_CHANNEL_MID;
// return ( reverse ? 1982 - val : val );
// }
// else{
// val = MAPCh_Val;
// return ( reverse ? MAPCh_REVVal - val : val );
// }
// }
// if ( val < middle )
// {
// if(crsf_send_en)
// val = (uint16_t)map(val, lower, middle, CRSF_DIGITAL_CHANNEL_MIN, CRSF_DIGITAL_CHANNEL_MID)+ offset;
// else
// val = (uint16_t)map(val, lower, middle, MAPCh_LOWVal, MAPCh_MIDVal)+ offset;
// //		if(val<=1020){
// //			return ( reverse ? MAPCh_REVVal - 1020 : 1020 );
// //		}
// }
// else
// {
// if(crsf_send_en)
// val = (uint16_t)map(val, middle, upper, CRSF_DIGITAL_CHANNEL_MID, CRSF_DIGITAL_CHANNEL_MAX)+ offset;
// else
// val = (uint16_t)map(val, middle, upper, MAPCh_MIDVal, MAPCh_MAXVal)+ offset;
// //		if(val>=1980){
// //			return ( reverse ? MAPCh_REVVal - 1980 : 1980 );
// //		}
// }
// if(crsf_send_en){
// if(val>CRSF_DIGITAL_CHANNEL_MAX) val = CRSF_DIGITAL_CHANNEL_MAX;
// if(val<CRSF_DIGITAL_CHANNEL_MIN) val = CRSF_DIGITAL_CHANNEL_MIN;//将val限制在lower~upper范围内
// return ( reverse ? 1982 - val : val );
// }
// else{
// if(val>2000) val = 2000;
// if(val<1000) val = 1000;//将val限制在lower~upper范围内
// return ( reverse ? MAPCh_REVVal - val : val );

// }

// }
// ///////////////////////////////////////////////////
// uint16_t mapChValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, uint16_t reverse, int16_t offset,uint8_t deadband)
// {

// if(abs(val-middle)<deadband){
// if(crsf_send_en){
// val = CRSF_DIGITAL_CHANNEL_MID;
// return ( reverse ? 1982 - val : val );
// }
// else{
// val = MAPCh_Val;
// return ( reverse ? MAPCh_REVVal - val : val );
// }
// }
// if ( val < middle )
// {
// if(crsf_send_en)
// val = (uint16_t)map(val, lower, middle, CRSF_DIGITAL_CHANNEL_MIN, CRSF_DIGITAL_CHANNEL_MID);
// else
// val = (uint16_t)map(val, lower, middle, MAPCh_LOWVal, MAPCh_MIDVal);
// //		if(val<=1020){
// //			return ( reverse ? MAPCh_REVVal - 1020 : 1020 );
// //		}
// }
// else
// {
// if(crsf_send_en)
// val = (uint16_t)map(val, middle, upper, CRSF_DIGITAL_CHANNEL_MID, CRSF_DIGITAL_CHANNEL_MAX);
// else
// val = (uint16_t)map(val, middle, upper, MAPCh_MIDVal, MAPCh_MAXVal);
// //		if(val>=1980){
// //			return ( reverse ? MAPCh_REVVal - 1980 : 1980 );
// //		}
// }
// if(crsf_send_en){
// if(val>CRSF_DIGITAL_CHANNEL_MAX) val = CRSF_DIGITAL_CHANNEL_MAX;
// if(val<CRSF_DIGITAL_CHANNEL_MIN) val = CRSF_DIGITAL_CHANNEL_MIN;//将val限制在lower~upper范围内
// return ( reverse ? 1982 - val : val );
// }
// else{
// if(val>2000) val = 2000;
// if(val<1000) val = 1000;//将val限制在lower~upper范围内
// return ( reverse ? MAPCh_REVVal - val : val );

// }

// }
// /////////////////////////////////////////////////////////

uint16_t mapChValue(uint16_t val, uint16_t lower, uint16_t middle, uint16_t upper, uint16_t reverse, int16_t offset,uint8_t deadband)
{
	
	 	 // 关键：动态调整中心基准点（只偏移中心，不改变上下限）
    int16_t adjusted_middle = (int16_t)middle + offset;
        // 限制调整后的中心不能超出有效范围（避免中心跑到上下限外，导致映射异常）
    if (adjusted_middle < lower + deadband) adjusted_middle = lower + deadband;
    if (adjusted_middle > upper - deadband) adjusted_middle = upper - deadband;

	if(abs(val-adjusted_middle)<deadband){
		if(crsf_send_en){
			val = CRSF_DIGITAL_CHANNEL_MID;
			return ( reverse ? 1982 - val : val );
		}
		else{
			val = MAPCh_Val;
			return ( reverse ? MAPCh_REVVal - val : val );
		}
	}
	if ( val < adjusted_middle )
	{
		if(crsf_send_en)
			val = (uint16_t)map(val, lower, adjusted_middle, CRSF_DIGITAL_CHANNEL_MIN, CRSF_DIGITAL_CHANNEL_MID);
		else
			val = (uint16_t)map(val, lower, adjusted_middle, MAPCh_LOWVal, MAPCh_MIDVal);
// if(val<=1020){
// return ( reverse ? MAPCh_REVVal - 1020 : 1020 );
// }
	}
	else
	{
		if(crsf_send_en)
			val = (uint16_t)map(val, adjusted_middle, upper, CRSF_DIGITAL_CHANNEL_MID, CRSF_DIGITAL_CHANNEL_MAX);
		else
			val = (uint16_t)map(val, adjusted_middle, upper, MAPCh_MIDVal, MAPCh_MAXVal);
// if(val>=1980){
// return ( reverse ? MAPCh_REVVal - 1980 : 1980 );
// }
	}
	if(crsf_send_en){
		if(val>CRSF_DIGITAL_CHANNEL_MAX) val = CRSF_DIGITAL_CHANNEL_MAX;
		if(val<CRSF_DIGITAL_CHANNEL_MIN) val = CRSF_DIGITAL_CHANNEL_MIN;//将val限制在lower~upper范围内
		return ( reverse ? 1982 - val : val );
	}
	else{
		if(val>2000) val = 2000;
		if(val<1000) val = 1000;//将val限制在lower~upper范围内
		return ( reverse ? MAPCh_REVVal - val : val );
		
	}

}



/* 这个是俯仰加 */
static void pitch_calibration_add(void){
	if(joystick_ch_offset[1]<500) joystick_ch_offset[1]  += 1;
// joystick_ch_offset[1] = 1;
}

/* 这个是俯仰减 */
static void pitch_calibration_sub(void){
	if(joystick_ch_offset[1]>-500) joystick_ch_offset[1] -= 1;
// joystick_ch_offset[1] = 0;
}

/* 这个是横滚加 */
static void roll_calibration_add(void){
	if(joystick_ch_offset[0]<500) joystick_ch_offset[0] += 1;
}

/* 这个是横滚减 */
static void roll_calibration_sub(void){
	if(joystick_ch_offset[0]>-500) joystick_ch_offset[0] -= 1;
}

// 全局变量
uint32_t value = 0;
uint8_t locked = 1;
// 按键处理函数
void HandleKeyPress(KeyCode_t key) {
    switch (key) {
        case KEY_1:
           roll_calibration_add();
						// if(joystick_ch_offset[0]==0){Buzzer_Start(BUZZER_MODE_DOUBLE);}
						// printf("KEY_1 %d\r\n",joystick_ch_offset[0]);
            break;
            
        case KEY_2:
           pitch_calibration_add();
						// if(joystick_ch_offset[1]==0){Buzzer_Start(BUZZER_MODE_DOUBLE);}
						// printf("KEY_2 %d\r\n",joystick_ch_offset[1]);
            break;
            
        case KEY_3:
           roll_calibration_sub();
						// if(joystick_ch_offset[0]==0){Buzzer_Start(BUZZER_MODE_DOUBLE);}
						// printf("KEY_3 %d\r\n",joystick_ch_offset[0]);
            break;
		case KEY_4:
           pitch_calibration_sub();
						// if(joystick_ch_offset[1]==0){Buzzer_Start(BUZZER_MODE_DOUBLE);}
						// printf("KEY_4 %d\r\n",joystick_ch_offset[1]);
            break;
		case KEY_5: //对频和写入
// Set_Joystick_Data();
// flash_write(FLASH_USER_START_ADDR);
// flash_read(FLASH_USER_START_ADDR);
			key_bit_status.key_bits.key5 ^= 1;
						// key6_count |= 0x01;
                        // 这里可以添加LED指示锁定状态的代码
						// LED2_TOGGLE();
						// printf("KEY_5\r\n");
            break;
		case KEY_6://解锁/一键起飞
                        // key6_count = !key6_count;
			if(hardware_value[2]==1500 && fly_status==0){
				key_bit_status.key_bits.key6 = 1;
				fly_status = 1; 
			}
			else{
				key_bit_status.key_bits.key6 = 0;
				fly_status = 0; 
			}
				
                        // 这里可以添加LED指示锁定状态的代码
						// LED2_TOGGLE();
						// printf("KEY_6\r\n");
            break;
		case KEY_7://定高
                        // key7_count = !key7_count;
			key_bit_status.key_bits.key7 ^= 1;
                        // 这里可以添加LED指示锁定状态的代码
						// LED2_TOGGLE();
						// printf("KEY_7\r\n");
            break;
		case KEY_8://切换灯光 //这里暂时改成四个
// key8_count ++;//这种单独的key8_count ++;实际就是key8_count = key8_count+1；
// if (key8_count > 8) {
// key8_count = 1;
// }
// key_bit_status.key_bits.key8 = key8_count;
			key_bit_status.key_bits.key8++;
                        // 这里可以添加LED指示锁定状态的代码
						// LED2_TOGGLE();
						// printf("KEY_8 = %d\r\n",key_bit_status.key_bits.key8);
            break;
		case KEY_9://翻滚
                        // key9_count = !key9_count;
// key_bit_status.key_bits.key9 ^= 1;
			flip_flag.start_flag = 1;
						// Buzzer_Start(BUZZER_MODE_SHORT);
                        // 这里可以添加LED指示锁定状态的代码
						// LED2_TOGGLE();
						// printf("KEY_9\r\n");
            break;
		case KEY_10://切换速度
			key_bit_status.key_bits.key10 ^= 1;
                        // 这里可以添加LED指示锁定状态的代码
						// LED2_TOGGLE();
						// printf("KEY_10\r\n");
            break;
        default:
                        // 其他按键可以添加更多处理逻辑
            break;
    }
}

Flip_Dirs Check_Flip_Dir(void){
	if(hardware_value[0]>1700 && hardware_value[1] == 1500)
		return RIGHT;
	if(hardware_value[0]<1300 && hardware_value[1] == 1500)
		return LEFT;
	if(hardware_value[1]>1700 && hardware_value[0] == 1500)
		return FORWARD;
	if(hardware_value[1]<1300 && hardware_value[0] == 1500)
		return BACK;
	return NONE;
}

void readJoystickAndBuzz(void){
	static uint8_t calibration_buzzer_over;
	if(hardware_value[0]<1100 && hardware_value[1] < 1100&& hardware_value[2] < 1100&& hardware_value[3] > 1800){
		if(calibration_buzzer_over){			
			return;
		}
		else{
			calibration_buzzer_over = 1;
			Buzzer_Start(BUZZER_MODE_SHORT);
		}

	}
	else
		calibration_buzzer_over = 0;


	
}
