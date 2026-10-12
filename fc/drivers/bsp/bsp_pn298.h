/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_pn298.h
 * @brief     pn298 驱动
 * @history - V1.0, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_pn298.h
 */
#ifndef __BSP_PN298_H
#define __BSP_PN298_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp.h"



#define	REG0_3V_WRITE					0xf1
#define REG0_3V_READ					0xf2
#define	REG1_3V_WRITE					0xf3
#define REG1_3V_READ					0xf4



// //////////////////////////////////////////////////////////////////////////////////////////////////////
/* REGISTER */

#define	RF_READ_REG						0x00							// 读配置寄存器,低5位为寄存器地址
																																				// 【说明】0x00|reg，读回的第一个字节是状态寄存器
#define	W_REGISTER						0x20							// 写配置寄存器,低5位为寄存器地址
																																				// 【说明】0x20|reg，例：W_REGISTER+RF_CH = 0x25
#define	R_RX_PAYLOAD					0x61							// 读RX有效数据, 1~32字节
																																				// 【说明】从 RX FIFO 弹出 1 个数据包（读完自动出队）
#define	W_TX_PAYLOAD					0xA0							// 写TX有效数据,1~32字节
																																				// 【说明】把待发数据压入 TX FIFO（先进先出缓冲队列）
#define	FLUSH_TX						0xE1							// 清除TX FIFO寄存器.发射模式下用
#define	FLUSH_RX						0xE2							// 清除RX FIFO寄存器.接收模式下用
#define	REUSE_TX_PL						0xE3							// 重新使用上一包数据,CE为高,数据包被不断发送
																																				// 【说明】本项目未使用（做持续载波测试时才需要）
#define	ACTIVATE						0x50							// ACTIVATE
																																				// 【说明】解锁“隐藏”寄存器 FEATURE/DYNPD，
																																				// 用法 bsp_pn298_write_reg(ACTIVATE, 0x73)
#define	DEACTIVATE						0x50							// DEACTIVATE
																																				// 【注意】与 ACTIVATE 同为 0x50，两者靠后面跟的
																																				// 数据字节区分（0x73=激活，0x53=反激活）。
																																				// 本项目未使用 DEACTIVATE（未改动）
#define	R_RX_PL_WID						0x60							// Read width of RX data
																																				// 【说明】读当前 RX FIFO 里这包数据的字节数
#define	W_ACK_PAYLOAD					0xA8							// Data with ACK（应答确认）
																																				// 【说明】预装“随 ACK 一起回给对方”的载荷，
																																				// 本项目用它把飞控状态回传给遥控器
#define	W_TX_PAYLOAD_NOACK				0xB0							// TX Payload no ACK Request
																																				// 【说明】发送但不要求对方应答，本项目未使用
#define	CE_ON							0xFD							// CE HIGH
																																				// 【说明】软件方式拉高 CE（芯片没有引出 CE 引脚，
																																				// 只能发这条命令控制发射/接收使能）
#define	CE_OFF							0xFC							// CE LOW
#define	RST_FSPI						0x53							// RESET
																																				// 【说明】复位芯片的 SPI 前端状态机（通信卡死时用）
#define	NOP								0xFF							// 空操作,可以用来读状态寄存器
																																				// 【说明】只想读状态、不想改任何东西时发它

// //////////////////////////////////////////////////////////////////////////////////////////////////////
/* CONTROL   REGISTER */

#define	CONFIG							0x00							// 配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
																																				// bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
																																				// 【补充】bit7:6 在本芯片里是“协议族选择”（见下方 PN298_MODE_MSK 的用法）
#define	EN_AA							0x01							// 使能自动应答功能  bit0~5,对应通道0~5
																																				// 【说明】自动应答(auto-ACK)：收到包后硬件立刻回一个确认，
																																				// 发送方据此判断“对方收到了”，是可靠传输的基础
#define	AW_RXADDR						0x02							// 接收地址允许,bit0~5,对应通道0~5
																																				// 【注意】名称里的 AW 容易让人误以为是“地址宽度”，
																																				// 其实 0x02 是 EN_RXADDR（接收通道使能）；
																																				// 真正的地址宽度寄存器是下面的 SETUP_AW(0x03)（未改动）
#define	SETUP_AW						0x03							// 设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
																																				// 【注意】本项目初始化时没有单独写这个寄存器，
																																				// 地址宽度是靠 0x03 起的 3 字节块一起配的（见 bsp_pn298.c）
#define	SETUP_RETR						0x04							// 建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
																																				// 【说明】bit3:0 = ARC 重发次数，bit7:4 = ARD 重发间隔
#define	RF_CH							0x05							// rf通道,bit6:0,工作通道频率;
																																				// 【说明】本驱动写入时会先 +64 偏移，见 bsp_pn298_set_channel()
#define	RF_SETUP						0x06							// rf寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
																																				// 【注意】驱动里是从 0x06 起连写 3 个字节的“速率配置块”，
																																				// 不是只写这 1 个字节（未改动）
#define	RF_STATUS						0x07							// 状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
																																				// bit5:数据发送完成中断;bit6:接收数据中断;
																																				// 【关键】这 3 个中断位是“写 1 清零”(W1C)：想清标志就往
																																				// 对应位写 1。清除动作见 bsp_pn298_clear_status()
#define	OBSERVE_TX						0x08							// 传输状态寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
																																				// 【说明】调试链路质量时读它，可判断“丢包多还是重发多”
#define	CD								0x09							// 载波检测寄存器,bit0,载波检测;
																																				// 【说明】相当于“信道是否被占用”，做跳频避让时有用
#define	RX_ADDR_P0						0x0A							// 数据通道0接收地址,最大长度5个字节,低字节在前
																																				// 【说明】本项目把“对码地址”写在这里，用于接收遥控器数据
#define	RX_ADDR_P1						0x0B							// 数据通道1接收地址,最大长度5个字节,低字节在前
																																				// 【说明】本项目把“默认地址”写在这里，用于对码前的握手
#define	RX_ADDR_P2						0x0C							// 数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	RX_ADDR_P3						0x0D							// 数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	RX_ADDR_P4						0x0E							// 数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	RX_ADDR_P5						0x0F							// 数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	TX_ADDR							0x10							// 发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
																																				// 【说明】发 ACK 时要靠 P0 地址匹配，所以改地址必须 TX_ADDR
																																				// 与 RX_ADDR_P0 一起改（见 bsp_pn298_change_addr()）
#define	RX_PW_P0						0x11							// 接收数据通道0有效数据宽度(1~32字节),设置为0则非法
																																				// 【说明】必须等于对方发来的字节数，否则整包收不到
#define	RX_PW_P1						0x12							// 接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P2						0x13							// 接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P3						0x14							// 接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P4						0x15							// 接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P5						0x16							// 接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define	FIFO_STATUS						0x17							// FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
																																				// bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
#define	DEM_CAL							0x19							// 【手册名】解调器校准寄存器（demodulator calibration）
																																				// 【本例】bit0 被当作“扰码(scramble)开关”：Nordic 模式关、其它模式开，
																																				// 见 bsp_pn298_set_mode()；速率切换时还会整字节改写，见 set_rate()
#define	RF_CAL2							0x1A							// 【手册名】射频校准2；本例被借用来配置发射功率
																																				// （从 0x1A 起连写 3 字节，见 bsp_pn298_set_power()）
#define	DEM_CAL2						0x1B							// 【手册名】解调器校准2；单载波测试时从 0x1B 起连写 3 字节
																																				// （见 bsp_pn298.c: pn298_carrier()）
#define	DYNPD							0x1C							// 动态载荷长度使能（dynamic payload），需要先 ACTIVATE 才能写
																																				// 【本例】写 0xc0 作为“noIRQ_noWORK”使能，见 __bsp_pn298_1v8_reg_init()
#define	FEATURE							0x1D							// 特性寄存器（使能动态载荷/ACK载荷/免ACK等），需先 ACTIVATE
																																				// 【本例】写 0x38（直接 64 字节模式）/ 0x3A（增强型初始化）
#define	RF_CAL							0x1E							// 【手册名】射频校准；本例用来把 PN298 的晶振时钟输出到 PAD
																																				// 位定义作者已写在 bsp_pn298.c 的注释里，见 __bsp_pn298_1v8_reg_init()
#define	BB_CAL							0x1F							// 【手册名】基带校准（baseband calibration）；从 0x1F 起连写 5 字节
																																				// 作为“模式时间配置”，见 bsp_pn298_set_mode()



/* PAN1026 模式 */
/* 工作模式编号。数值不是随便排的，而是“高 4 位选协议族、bit0 选普通/增强型”： 高 4 位（配合下面的 PN298_MODE_MSK=0x30 取出后左移 2 位，落到 CONFIG[7:6]）： 0x0_ = XN297L 协议族 0x1_ = BLE 协议族 */
#define PN298_XN297L_NORMAL_MODE		0x00							//xn297L普通型模式
#define PN298_XN297L_ENHANCE_MODE		0x01							//xn297L增强型模式
#define PN298_BLE_MODE				0x10							//ble模式
#define PN298_NORDIC_NORMAL_MODE		0x20							//nordic普通型模式
#define PN298_NORDIC_ENHANCE_MODE		0x21							//nordic增强型模式
		 
#define PN298_MODE_MSK				0x30							// 模式掩码：取出“协议族”高 4 位
																																				// bsp_pn298_set_mode() 里用 (mode & PN298_MODE_MSK) << 2
																																				// 把 0x00/0x10/0x20 映射到 CONFIG 的 bit7:6



/* PAN1026 功率 */
/* 发射功率档位：每个值是“从 0x1A 起连写 3 字节”里的后 2 个字节（16bit PA 配置字）， 功率越大通信距离越远、电流越大；本项目 config_rf.h 选的是 0xC9Ca（约 10dBm 档）。 */
#define PN298_POWER_P_12				0xc9cf							// 12dBm
#define PN298_POWER_P_11				0x49cf							// 11dBm
#define PN298_POWER_P_10				0x49c7							// 10dBm
#define PN298_POWER_P_7				0xc9da							// 7dBm
#define PN298_POWER_P_6				0xc9c6							// 6dBm
#define PN298_POWER_P_4				0xc9c2							// 4dBm
#define PN298_POWER_P_2				0x49ca							// 2dBm
#define PN298_POWER_P_0				0x49c2							// 0dBm
#define PN298_POWER_N_2				0xc9c1							// -2dBm
#define PN298_POWER_N_10				0x49c1							// -10dBm
		 
/* 空中速率档位：写进“0x06 起的 3 字节速率配置块”的低位/高位里。 速率越高吞吐越大但灵敏度越差（同样距离更容易丢包）； 速率越低抗干扰越好、距离越远，但一包数据占用空中的时间变长。 */
#define	PN298_RF_DATA_RATE_1M			0x00
#define	PN298_RF_DATA_RATE_2M			0x40
#define	PN298_RF_DATA_RATE_250K		0xC0

/* 速率宏的短别名，方便书写。 */
#define DR_1M     PN298_RF_DATA_RATE_1M
#define DR_2M     PN298_RF_DATA_RATE_2M
#define DR_250K   PN298_RF_DATA_RATE_250K



#define	RX_DR_FLAG						0x40							// 接收中断标志位
																																				// 收到一包数据后硬件置位
#define	TX_DS_FLAG						0x20							// 发送完成中断标志位
																																				// 对方已回 ACK（=确实收到了）才置位
#define	RX_TX_FLAG						0x60							// 发送接收完成中断标志位，ack_payload 模式下使用
																																				// = RX_DR_FLAG|TX_DS_FLAG：本包已发出且对方 ACK 里带了回传数据
#define	MAX_RT_FLAG						0x10							// 发送重传超时中断标志位
																																				// 重发次数用尽仍未收到 ACK -> 这一包算失败/链路丢失

/* 发送类型：普通模式不带自动应答；增强模式带自动应答+ACK 载荷，链路更可靠。 */
#define	TRANS_ENHANCE_MODE				1								// 发送增强模式
#define	TRANS_NORMAL_MODE				2								// 发送普通模式

/* CE（Chip Enable，芯片使能）的驱动方式。 芯片的 CE 没有引到 MCU 引脚上，所以本项目用“软命令”CE_ON/CE_OFF 代替引脚。 */
#define	CE_SOFTWARE_MODE				1								// CE软件模式
#define	CE_HARDWARE_MODE				2								// CE硬件模式

// //////////////////////////////////////////////////////////////////////////////////////////////////////
/* USER  CONFIG */
/* ---- 以下几行是“驱动层默认配置”，项目实际使用的参数在 config_rf.h ---- */
#define	DEFAULT_CHANNEL					CONF_DEFAULE_CNANNEL			// 初始频率
																																				// 【注意】CONF_DEFAULE_CNANNEL 在本工程里没有定义，
																																				// 而且拼写像 CONF_DEFAULT_CHANNEL 的笔误；
																																				// 只要没人用到 DEFAULT_CHANNEL 就不会报错（未改动）
#define	PAYLOAD_WIDTH					CONF_PAYLOAD_WIDTH				// Payload长度
																																				// 【注意】CONF_PAYLOAD_WIDTH 在本工程里同样没有定义（未改动）
#define	CE_TYPE							CE_SOFTWARE_MODE				// 初始设置为硬件模式
																																				// 【注意】值选的是“软件模式”，注释写的是“硬件模式”，
																																				// 二者矛盾；以代码为准（未改动）
#define	TRANSMIT_TYPE					TRANS_ENHANCE_MODE				// 初始设置为普通模式
																																				// 【注意】同样：值是增强模式，注释写普通模式（未改动）
#define DATA_RATE						XN297L_RF_DATA_RATE_1M
																																				// 【注意】XN297L_ 前缀的宏本文件没有定义（本文件只有
																																				// PN298_RF_DATA_RATE_1M），疑似历史遗留；
																																				// 因为没有代码展开 DATA_RATE，所以不报错（未改动）
#define	RF_POWER						(XN297L_RF_POWER_P_10|DATA_RATE)
																																				// 【注意】同上，XN297L_RF_POWER_P_10 也未定义（未改动）
#define RF_COMMON_ADDR					CONF_DEFAULT_ADDR				// 初始地址
																																				// 【注意】CONF_DEFAULT_ADDR 在本工程里没有定义（未改动）
#define RF_RETRANS						01
																																				// 【说明】自动重传次数（八进制 01 = 十进制 1）；
																																				// 本工程没有代码引用它（未改动）
#define EN_ACK_PAYLOAD					1
																																				// 【说明】使能“ACK 载荷”：对方回 ACK 时能捎带数据回来。
																																				// 本工程靠它把飞控状态回传给遥控器
#define EN_NO_ACK						0
																																				// 【说明】不使能“免应答发送”模式



#if(TRANSMIT_TYPE == TRANS_ENHANCE_MODE)
#define	EN_DYNPLOAD						0
																																				// 【说明】0=不启用动态载荷长度，改用固定长度（省得每包都问长度）
#define	EN_ACK_PAYLOAD					1
																																				// 【注意】与前面第 137 行重复定义，值相同所以编译器不报错
																																				// （C 允许“完全相同”的重复宏定义）（未改动）
#define	ACK_PAYLOAD_WIDTH				16
																																				// 【说明】ACK 里回传的字节数（遥控器侧按 16 字节解析）
#endif
// //////////////////////////////////////////////////////////////////////////////////////////////////////

/* ---- 对外函数声明 ---- */
void	bsp_pn298_write_reg(uint8_t reg,uint8_t data);												// PN298写寄存器函数
uint8_t	bsp_pn298_read_reg(uint8_t reg);															// PN298读寄存器函数
void	bsp_pn298_write_buf(uint8_t reg,uint8_t *pbuf,uint8_t len);									// PN298写缓冲函数
void	bsp_pn298_read_buf(uint8_t reg,uint8_t *pbuf,uint8_t len);									// PN298读缓冲函数
void 	bsp_pn298_ce_enable(void);																	// PN298 CE 使能
void 	bsp_pn298_ce_disable(void);																	// PN298_CE 禁用
uint8_t	bsp_pn298_get_status(void);																	// PN298获取状态信息
																																																		// 【注意】本工程只定义了 bsp_pan1026_get_status()，
																																																		// 名字对不上；因为没人调用所以不报错（未改动）
void	bsp_pn298_clear_status(void);																// PN298清除状态信息
void 	bsp_pn298_flush_rx_fifo(void);																// PN298清除rx缓冲
void 	bsp_pn298_flush_tx_fifo(void);																// PN298清除tx缓冲
void	bsp_pn298_clear_fifo(void);																	// PN298清除缓冲
void	bsp_pn298_tx_mode(void);																	// PN298设置为发送模式
void	bsp_pn298_rx_mode(void);																	// PN298设置为接收模式
void 	bsp_pn298_set_power(uint16_t power);														// PN298设置发射功率
void	bsp_pn298_set_channel(uint8_t channel);														// PN298设置通信信道
void 	bsp_pn298_set_mode(uint8_t mode);															// PN298设置模式
void 	bsp_pn298_set_addr(uint8_t reg, uint8_t *rf_addr_array, uint8_t addr_len);				
void 	bsp_pn298_set_rx_payload_len(uint8_t len);				
void	bsp_pn298_tx_data(uint8_t *data, uint8_t len);												// PN298发送数据
uint8_t	bsp_pn298_rx_data(uint8_t *data,uint8_t len);												// PN298接收函数
void 	bsp_pn298_byte_bit_decode(uint8_t* buf, uint8_t len);
																																																		// 反位序+反字节序（芯片收发时按位倒序）
uint8_t bsp_pn298_rx_data_ack_fix(uint8_t *rx_data, uint8_t *ack_data, uint8_t len, uint8_t ack);
																																																		// 收包并在应答窗口内回 ACK 载荷
void bsp_pn298_tx_data_ack(uint8_t *data, uint8_t *ack_data, uint8_t len);
																																																		// 发包并等待对方 ACK 回包
		
void	bsp_pn298_init(void);																		// PN298初始化
void pn298_carrier(void);																			// 发射单载波（产测/认证用）
void pn298_deep_sleep(void);																		// 深度睡眠（省电）

void bsp_pn298_change_addr(uint8_t * addr,uint8_t ch);												// 运行中切换地址与信道

#ifdef __cplusplus
}
#endif

#endif // __DRV_PAN1026_H
/* 【注意】上面的 #endif 行尾注释写的是 __DRV_PAN1026_H，与真正的宏名 __BSP_PN298_H 不一致，属于改名时漏改注释，不影响编译（未改动）。 */



