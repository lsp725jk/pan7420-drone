/**
 * 文件：user/deal_pack.c                    【角色】通信数据包的拆帧（遥控指令 → 飞控内部变量）
 */
#include "deal_pack.h"
#include "main.h"
#include "bsp.h"
#include "flash.h"



/* 接收暂存缓冲：static ⇒ 只在本文件可见。 为什么要它：bsp_pn298_rx_data_ack_fix() 的第二个参数是“发送/ACK 缓冲”， 第一个参数才是真正收到的数据 —— 分开两个数组避免收发包互相踩内存 */

static uint8_t rx_buff[__PN298_PAYLOAD];

/* 解析出来的通道原始值（16 位）：[0]横滚 [1]俯仰 [2]油门 [3]偏航 [4..8]辅助。 数组开 16 个是留余量（实际只填前 9 个） */
uint16_t ch_value[16];
extern Pair_Code_Config pair_data;   /* 用户参数（对码地址/信道/零偏），定义在 flash.c */

uint8_t altitude_mode;		//定高模式
/* 定高开关：由按键 bit2 得来（0=手动，1=定高）。被 imu/altitude.c 与 imu/fc.c 读取 */

uint8_t color=0;	//颜色选择



uint8_t pair_addr_arr[5];
/* 对码完成标志：main.c 的对码 while(1) 靠它判断能否跳出。 注意它在收到对码包(0xDB)与收到正常遥控包(0xAA)时都会置 1 */
uint8_t pair_over_flag = 0;
/* 两档“速度系数”，由按键 speed_level 二选一写进全局 speed_level： {0.2, 0.5} 分别对应“慢/快”档，用于限制姿态控制的最大修正量 */
float speed[2]={0.2,0.5};

// 记录连接建立时间
/* 最近一次收到**合法遥控包**的时间戳（HAL_GetTick() 毫秒值）。 只在解析成功后刷新，所以“收到垃圾包”不会延长连接寿命 */
uint32_t connection_nowtime;

/* 按键位域实例（类型见 deal_pack.h）：整字节从包的第 11 字节来， 之后各功能模块按名字读某一位 */
KeyBitStatus_t key_bit_status;



Flip_Dirs flip_dir;



uint8_t device_ID;

/* @brief  收一个射频包并解析（非阻塞：没收到就立刻返回） 【参数】无（数据从射频芯片取，结果写全局变量） 【返回】无 【算法步骤】 */
void decode_pack(void){
// printf("jingr \r\n");
// for(int i = 0; i < __PN298_PAYLOAD; i++){
// temp_buff[i] = i;
// }



	if(bsp_pn298_rx_data_ack_fix(rx_buff, temp_buff, __PN298_PAYLOAD, 1)){
		uint8_t i=0;uint8_t index = 0;
		
				/* 步骤 2：帧头 + 设备 ID 双重校验。0xaa 是遥控指令包的标志； device_ID 由 flash.c 在上电时从 Flash 读出（同场地多台机器靠它区分）。 注释掉的 rx_buff[11]==0xac 是曾经用过的第三重校验（未启用，未改动） */
		if(rx_buff[0]==0xaa&&rx_buff[1]==device_ID){//&&rx_buff[11]==0xac
			pair_over_flag = 1;
												/* 收到正常指令包也置对码标志：含义是“通信链路已建立”。 这让飞机在遥控器从未重新对码（地址没变）时也能直接开机起飞 */
			index = 2;
						/* 步骤 3：通道数据，每个 2 字节、大端序（高字节在前） ⚠⚠ 这个循环的上界有问题：GCC 编译时报 [-Waggressive-loop-optimizations]" 已核实**不是误报**，而是**循环次数与协议布局不匹配**： */
			for(i=0;i<9;i++){
																/* ch_value[0]为横滚值 ch_value[1]为俯仰值 ch_value[2]为油门值 ch_value[3]为偏航值 */
																/* 大端合并：先收高字节，左移 8 位再或上低字节。 强转 (uint16_t) 是必须的：rx_buff 是 uint8_t，若不移位到 16 位就左移会溢出 */
				ch_value[i] = ((uint16_t)rx_buff[index] << 8) | ((uint16_t)(rx_buff[index+1]));// 合并uint8_t为uint16_t
										// ch_value[i] = mapChValue(recPWMvalue[i]);
										// if(ch_value[i]!=0) PWMvalue[i] = recPWMvalue[i];
				index = index+2;
			}
						// 得到按键的状态
												/* 整字节赋值后，key_bit_status.key_bits.xxx 就能按位读到各按键 （union 共享内存，见 deal_pack.h 的 KeyBitStatus_t） */
			key_bit_status.key_byte = rx_buff[10];
												/* 第 12 字节低 4 位 = 翻滚方向；为 0 时不覆盖旧值（保持“不翻”）， 这样遥控器松手后 fly_dir 仍是上一次的方向，由 flip.c 的状态机决定何时退出 */
			if(rx_buff[11]!=0x00){
				flip_dir = (Flip_Dirs)(rx_buff[11]&0x0F);
			}
// flip_dir = (Flip_Dirs)(rx_buff[11]&0x0F);
// if(rx_buff[11]){
// printf("%d\r\n",flip_dir);
// }
						// printf("%d\r\n",flip_dir);
						/* 换算成飞控内部指令：摇杆类通道减中位 1500 ⇒ 范围 -500~+500， 正负号直接表示方向（右手系：正 = 右/前/右转）。 油门不减中位：它是 1000~2000 的单向值，imu/fc.c 会再映射到 0~100% 推力 */
			fcCmd[FC_CMD_ROLL]=ch_value[0]-1500;		//横滚值
			fcCmd[FC_CMD_PITCH]=ch_value[1]-1500;		//俯仰值
			fcCmd[FC_CMD_THROTTLE]=ch_value[2];		//油门值
			fcCmd[FC_CMD_YAW]=ch_value[3]-1500;			//偏航值
// printf("%d\r\n",fcCmd[FC_CMD_THROTTLE]);
// fc.flags.bLanding=key_bit_status.key_bits.is_rolling;
			
												/* 按键 → 指令：FC_CMD_FLAGS_1 是“解锁/上锁”开关，被 imu/fc.c 的状态机读取。 它只表示“遥控器上那个开关现在拨在哪边”，真正的解锁判定在 fc.c 里做 */
			fcCmd[FC_CMD_FLAGS_1]=key_bit_status.key_bits.is_armed;		//解锁，上锁标志位
			
												/* 定高开关：全局变量，让高度环知道“要不要保持当前高度” */
			altitude_mode=key_bit_status.key_bits.is_althold;		//定高模式选择
// printf("key:%d ,%d\r\n",fc.flags.motorArmed ,fc.flags.motorIdle);
												/* 3 位灯效编号（0~7）→ led.c 的 Led_Process() 里对应 8 种预设颜色 */
			color=key_bit_status.key_bits.light_mode;			//颜色
			
												/* 两档速度系数：0=慢档 0.2、1=快档 0.5（数组 speed[] 在文件开头定义）。 用数组而不是写死常量，是为了以后加档位方便 */
			if(key_bit_status.key_bits.speed_level==0)
			{
// speed_level=0.05;
						speed_level=speed[0];
			}
			else if(key_bit_status.key_bits.speed_level==1)
			{
// speed_level=0.5;
						speed_level=speed[1];
			}
			
												/* 刷新“连接活着”的时间戳 —— 只有走到这里（帧头与设备 ID 都匹配）才刷新， 所以噪声包不会骗过失联检测。main.c 用 5 秒超时判断 */
			connection_nowtime = HAL_GetTick();	
			
// printf("p:%f i:%f d:%f \r\n",p_add*0.01f,i_add*0.01f,d_add*0.0001f);
// printf("rx_buff[10]的值为 %x,\r\n",rx_buff[10]);
// printf("PWM的值%d,%d,%d,%d\r\n",ch_value[0],ch_value[1],ch_value[2],ch_value[3]);
// printf("按键5的值%d\r\n",key_bit_status.key_bits.is_rolling);
// printf("按键6的值%d\r\n",key_bit_status.key_bits.is_armed);
// printf("按键7的值%d\r\n",key_bit_status.key_bits.is_althold);
// printf("按键9的值%d\r\n",key_bit_status.key_bits.is_steeringon);
// printf("按键10的值%d\r\n",key_bit_status.key_bits.speed_level);
// printf("按键8的值%d\r\n",key_bit_status.key_bits.light_mode);
		}
				/* ---------- 分支 B：对码包（0xDB 0xDB）---------- 对码过程是“遥控器广播自己的地址，飞机抄下来并存好”。 为什么飞机要先回一个 ACK：遥控器靠收到 ACK 才知道“有人接住了”， 否则它会一直广播（见遥控器端的对码状态机）。 */
		else if(rx_buff[0]==0xdb&&rx_buff[1]==0xdb){//对码
// temp_buff[0] += 1;
			pair_over_flag = 1;
			bsp_pn298_write_reg(FLUSH_TX, 0);						// 清除发送缓冲
												/* 清 TX FIFO 再写 ACK：避免上一次没发完的旧数据排在新 ACK 前面 */
			bsp_pn298_write_buf(W_ACK_PAYLOAD, temp_buff, __PN298_PAYLOAD);	// 发送ACK数据
			index=2;
			for(i=0;i<6;i++){

				*((uint8_t *)&pair_data.paircode+i) = rx_buff[index];
												// rc_pair_packet[i] = g_RF24L01RxBuffer[index];
				index++;
			}
												/* 设备 ID 来自包的第 9 字节（index 走到 8）。之后 main.c 靠它过滤“不是给我的包” */
			pair_data.device_id=rx_buff[8];
						// printf("地址%x,%x,%x,%x,%x\r\n",pair_data.paircode[0],pair_data.paircode[1],pair_data.paircode[2],pair_data.paircode[3],pair_data.paircode[4]);
										// 写入flash
												/* 落盘：把整个 pair_data 写进 Flash 最后一页（内部先整页擦除，见 flash.c）。 这一步会挂起 CPU 取指几十毫秒，所以绝不能放在飞行过程中调用 */
			flash_write(FLASH_USER_START_ADDR);

		}	
	}
}

/* @brief  把 pair_data.paircode 的 5 字节拆成独立的地址数组 pair_addr_arr 【参数】无 【返回】无 【算法】结构体成员 → 数组的逐字节拷贝。为什么绕一圈：射频接口 */
void get_now_addr(void){
	for(uint8_t i=0;i<5;i++){
		pair_addr_arr[i] = *((uint8_t *)(&pair_data.paircode)+i);
	}
	
		// printf("%x-%x-%x-%x-%x\r\n",pair_addr_arr[0],pair_addr_arr[1],pair_addr_arr[2],pair_addr_arr[3],pair_addr_arr[4]);
		// printf("%x\r\n",pair_addr_arr.rf_ch);
	
}
