/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_pn298.h
 * @brief     pn298 驱动
 * @history - V1.0, 2022-02-17, yangyaming, first implementation.
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



// ////////////////////////////////////////////////////////////////////////////////////////////////////////
/* REGISTER */
#define	RF_READ_REG						0x00							// 读配置寄存器,低5位为寄存器地址
#define	W_REGISTER						0x20							// 写配置寄存器,低5位为寄存器地址
#define	R_RX_PAYLOAD					0x61							// 读RX有效数据, 1~32字节
#define	W_TX_PAYLOAD					0xA0							// 写TX有效数据,1~32字节
#define	FLUSH_TX						0xE1							// 清除TX FIFO寄存器.发射模式下用
#define	FLUSH_RX						0xE2							// 清除RX FIFO寄存器.接收模式下用
#define	REUSE_TX_PL						0xE3							// 重新使用上一包数据,CE为高,数据包被不断发送
#define	ACTIVATE						0x50							// ACTIVATE
#define	DEACTIVATE						0x50							// DEACTIVATE
#define	R_RX_PL_WID						0x60							// Read width of RX data
#define	W_ACK_PAYLOAD					0xA8							// Data with ACK（应答确认）
#define	W_TX_PAYLOAD_NOACK				0xB0							// TX Payload no ACK Request
#define	CE_ON							0xFD							// CE HIGH
#define	CE_OFF							0xFC							// CE LOW
#define	RST_FSPI						0x53							// RESET
#define	NOP								0xFF							// 空操作,可以用来读状态寄存器

// ////////////////////////////////////////////////////////////////////////////////////////////////////////
/* CONTROL   REGISTER */
#define	CONFIG							0x00							// 配置寄存器地址;bit0:1接收模式,0发射模式;bit1:电选择;bit2:CRC模式;bit3:CRC使能;
																																				// bit4:中断MAX_RT(达到最大重发次数中断)使能;bit5:中断TX_DS使能;bit6:中断RX_DR使能
#define	EN_AA							0x01							// 使能自动应答功能  bit0~5,对应通道0~5
#define	EN_RXADDR						0x02							// 接收地址允许,bit0~5,对应通道0~5
#define	SETUP_AW						0x03							// 设置地址宽度(所有数据通道):bit1,0:00,3字节;01,4字节;02,5字节;
#define	SETUP_RETR						0x04							// 建立自动重发;bit3:0,自动重发计数器;bit7:4,自动重发延时 250*x+86us
#define	RF_CH							0x05							// rf通道,bit6:0,工作通道频率;
#define	RF_SETUP						0x06							// rf寄存器;bit3:传输速率(0:1Mbps,1:2Mbps);bit2:1,发射功率;bit0:低噪声放大器增益
#define	RF_STATUS						0x07							// 状态寄存器;bit0:TX FIFO满标志;bit3:1,接收数据通道号(最大:6);bit4,达到最多次重发
																																				// bit5:数据发送完成中断;bit6:接收数据中断;
#define	OBSERVE_TX						0x08							// 传输状态寄存器,bit7:4,数据包丢失计数器;bit3:0,重发计数器
#define	CD								0x09							// 载波检测寄存器,bit0,载波检测;
#define	RX_ADDR_P0						0x0A							// 数据通道0接收地址,最大长度5个字节,低字节在前
#define	RX_ADDR_P1						0x0B							// 数据通道1接收地址,最大长度5个字节,低字节在前
#define	RX_ADDR_P2						0x0C							// 数据通道2接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	RX_ADDR_P3						0x0D							// 数据通道3接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	RX_ADDR_P4						0x0E							// 数据通道4接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	RX_ADDR_P5						0x0F							// 数据通道5接收地址,最低字节可设置,高字节,必须同RX_ADDR_P1[39:8]相等;
#define	TX_ADDR							0x10							// 发送地址(低字节在前),ShockBurstTM模式下,RX_ADDR_P0与此地址相等
#define	RX_PW_P0						0x11							// 接收数据通道0有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P1						0x12							// 接收数据通道1有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P2						0x13							// 接收数据通道2有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P3						0x14							// 接收数据通道3有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P4						0x15							// 接收数据通道4有效数据宽度(1~32字节),设置为0则非法
#define	RX_PW_P5						0x16							// 接收数据通道5有效数据宽度(1~32字节),设置为0则非法
#define	FIFO_STATUS						0x17							// FIFO状态寄存器;bit0,RX FIFO寄存器空标志;bit1,RX FIFO满标志;bit2,3,保留
																																				// bit4,TX FIFO空标志;bit5,TX FIFO满标志;bit6,1,循环发送上一数据包.0,不循环;
#define	DEM_CAL							0x19
#define	RF_CAL2							0x1A
#define	DEM_CAL2						0x1B
#define	DYNPD							0x1C
#define	FEATURE							0x1D
#define	RF_CAL							0x1E
#define	BB_CAL							0x1F



/* PAN1026 模式 */
	
#define PN298_XN297L_NORMAL_MODE		0x00							//xn297L普通型模式
#define PN298_XN297L_ENHANCE_MODE		0x01							//xn297L增强型模式
#define PN298_BLE_MODE				0x10							//ble模式
#define PN298_NORDIC_NORMAL_MODE		0x20							//nordic普通型模式
#define PN298_NORDIC_ENHANCE_MODE		0x21							//nordic增强型模式
		 
#define PN298_MODE_MSK				0x30



/* PAN1026 功率 */

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
		 
#define	PN298_RF_DATA_RATE_1M			0x00
#define	PN298_RF_DATA_RATE_2M			0x40
#define	PN298_RF_DATA_RATE_250K		0xC0

#define DR_1M     PN298_RF_DATA_RATE_1M
#define DR_2M     PN298_RF_DATA_RATE_2M
#define DR_250K   PN298_RF_DATA_RATE_250K

#define	RX_DR_FLAG						0x40							// 接收中断标志位
#define	TX_DS_FLAG						0x20							// 发送完成中断标志位
#define	RX_TX_FLAG						0x60							// 发送接收完成中断标志位，ack_payload 模式下使用
#define	MAX_RT_FLAG						0x10							// 发送重传超时中断标志位

#define	TRANS_ENHANCE_MODE				1								// 发送增强模式
#define	TRANS_NORMAL_MODE				2								// 发送普通模式

#define	CE_SOFTWARE_MODE				1								// CE软件模式
#define	CE_HARDWARE_MODE				2								// CE硬件模式

// ////////////////////////////////////////////////////////////////////////////////////////////////////////
/* USER  CONFIG */
#define	DEFAULT_CHANNEL					CONF_DEFAULE_CNANNEL			// 初始频率
#define	PAYLOAD_WIDTH					CONF_PAYLOAD_WIDTH				// Payload长度
#define	CE_TYPE							CE_SOFTWARE_MODE				// 初始设置为硬件模式
#define	TRANSMIT_TYPE					TRANS_ENHANCE_MODE				// 初始设置为普通模式
#define DATA_RATE						XN297L_RF_DATA_RATE_1M
#define	RF_POWER						(XN297L_RF_POWER_P_10|DATA_RATE)
#define RF_COMMON_ADDR					CONF_DEFAULT_ADDR				// 初始地址
#define RF_RETRANS						01
#define EN_ACK_PAYLOAD					1
#define EN_NO_ACK						0

#if(TRANSMIT_TYPE == TRANS_ENHANCE_MODE)
#define	EN_DYNPLOAD						0
#define	EN_ACK_PAYLOAD					1
#define	ACK_PAYLOAD_WIDTH				16
#endif
// ////////////////////////////////////////////////////////////////////////////////////////////////////////

void	bsp_pn298_write_reg(uint8_t reg,uint8_t data);												// PN298写寄存器函数
uint8_t	bsp_pn298_read_reg(uint8_t reg);															// PN298读寄存器函数
void	bsp_pn298_write_buf(uint8_t reg,uint8_t *pbuf,uint8_t len);									// PN298写缓冲函数
void	bsp_pn298_read_buf(uint8_t reg,uint8_t *pbuf,uint8_t len);									// PN298读缓冲函数
void 	bsp_pn298_ce_enable(void);																	// PN298 CE 使能
void 	bsp_pn298_ce_disable(void);																	// PN298_CE 禁用
uint8_t	bsp_pn298_get_status(void);																	// PN298获取状态信息
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
uint8_t bsp_pn298_rx_data_ack_fix(uint8_t *rx_data, uint8_t *ack_data, uint8_t len, uint8_t ack);
void bsp_pn298_tx_data_ack(uint8_t *data, uint8_t *ack_data, uint8_t len);
		
void	bsp_pn298_init(void);																		// PN298初始化
void pn298_carrier(void);
void pn298_deep_sleep(void);



void bsp_pn298_change_addr(uint8_t * addr,uint8_t ch);
void bsp_pn298_re_sign_init(void);
#ifdef __cplusplus
}
#endif

#endif // __DRV_PAN1026_H
