/**
 * @note      Copyright (C) Shanghai Panchip Microelectronics Co., Ltd.
 * @file      bsp_pn298.c
 * @brief     pn298 RF驱动
 * @history - V1.0, 2022-02-17, yangyaming, first implementation.
 */
/**
 * 文件：drivers/bsp/bsp_pn298.c            【角色】PN298 2.4G 射频芯片驱动
 */



#include "bsp_pn298.h"
#include "flash.h"
#include "deal_pack.h"
#include "bmp3.h"
#include "common.h"
extern uint8_t pair_addr_arr[];
/* @description	: 向PN298寄存器写入一个数据 @param		: reg ：data @return		: 无 */
/* @brief  向 PN298 的一个寄存器写入 1 个字节（最底层的“写寄存器”原语） 【参数】 @param reg   寄存器“命令字”：必须是 W_REGISTER(0x20) 或上寄存器地址， 例如 W_REGISTER + RF_CH(0x05) = 0x25。 */
void bsp_pn298_write_reg(uint8_t reg, uint8_t data)
{
	__rfspi_pan742_write_cs(0);
	rfspi_pan742_write_byte(reg);
	rfspi_pan742_write_byte(data);
	__rfspi_pan742_write_cs(1);
}
/* @description	: 从PN298寄存器读取一个数据 @param		: reg @return		: 读取到的数据 */
/* @brief  从 PN298 的一个寄存器读出 1 个字节 【参数】 @param reg   寄存器地址（光秃秃的编号，如 CONFIG/RF_STATUS）。 注意：这里**不能**再或上 W_REGISTER，读时序的第一个字节 */
uint8_t bsp_pn298_read_reg(uint8_t reg)
{
    uint8_t temp;
	__rfspi_pan742_write_cs(0);
	rfspi_pan742_write_byte(reg);
	temp= rfspi_pan742_read_byte();
	__rfspi_pan742_write_cs(1);
    return temp;
}
/* @description	: 从PN298寄存器读取多个数据 @param		: reg ：*pbuf ：length */
/* @brief  连续向 PN298 写入多个字节（从 reg 开始，地址自增） 【参数】 @param reg   起始寄存器（命令字 = W_REGISTER + 起始地址） @param pbuf  数据缓冲区首地址 */
void bsp_pn298_write_buf(uint8_t reg,uint8_t *pbuf, uint8_t len)
{
	uint8_t i;
	__rfspi_pan742_write_cs(0);
	rfspi_pan742_write_byte(reg);
	
	for(i = 0; i < len; i++){
		rfspi_pan742_write_byte(pbuf[i]);
	}
	__rfspi_pan742_write_cs(1);
}
/* @description	: 从PN298寄存器读取多个数据 @param		: reg ：*pbuf ：length */
/* @brief  从 PN298 连续读出多个字节 【参数】 @param reg   起始寄存器地址 @param pbuf  接收缓冲区首地址（由调用者保证足够大） */
void bsp_pn298_read_buf(uint8_t reg, uint8_t *pbuf, uint8_t len)
{
	uint8_t i;
	__rfspi_pan742_write_cs(0);
	rfspi_pan742_write_byte(reg);
	for(i = 0; i < len; i++){
		pbuf[i] = rfspi_pan742_read_byte();
	}
	__rfspi_pan742_write_cs(1);
}
/* @description	: 打开ce @param		: 无 @return		: 无 */

/* @brief  使能 CE（Chip Enable）—— 让芯片进入收发工作状态 【参数】 无 【返回】 无 【为什么是发命令而不是拉引脚】 */
void bsp_pn298_ce_enable(void)
{
	bsp_pn298_write_reg(CE_ON, 0x00);
}
/* @description	: 关闭ce @param		: 无 @return		: 无 */
/* @brief  关闭 CE —— 让芯片退出收发状态、回到待机 【参数】 无 【返回】 无 【为什么切模式前一定要先关 CE】 */
void bsp_pn298_ce_disable(void)
{
	bsp_pn298_write_reg(CE_OFF, 0x00);
}
/* @description	: 获取PN298的状态 @param		: 无 @return		: 无 */
/* @brief  读状态寄存器 RF_STATUS(0x07) 【参数】 无 【返回】 状态字节，位定义见 bsp_pn298.h 里的 *_FLAG 宏： bit6 RX_DR（收到包）bit5 TX_DS（发送完成）bit4 MAX_RT（重发超时） */
uint8_t	bsp_pan1026_get_status(void)
{
	return bsp_pn298_read_reg(0x07);
}

/* @description	: 清除状态 @param		: 无 @return		: 无 */
/* @brief  清除 RF_STATUS 里的 3 个中断标志 【参数】 无 【返回】 无 【算法/为什么】 */
void bsp_pn298_clear_status(void)
{
	bsp_pn298_write_reg(W_REGISTER + 0x07, 0x70);

}
/* @description	: 清除rx fifo @param		: 无 @return		: 无 */
/* @brief  清空接收 FIFO（丢掉里面还没被读走的包） 【参数】 无 【返回】 无 【为什么需要】FIFO 有 3 级深度。如果上次的包没被读走，新的包会排在后面； */
void bsp_pn298_flush_rx_fifo(void)
{
	bsp_pn298_write_reg(FLUSH_RX, 0x00);
}
/* @description	: 打开ce @param		: 无 @return		: 无 */
/* @brief  清空发送 FIFO 【参数】 无 【返回】 无 【注意】上面作者原有的注释写的是“打开ce”，是从 flush_rx_fifo 复制过来的 */
void bsp_pn298_flush_tx_fifo(void)
{
	bsp_pn298_write_reg(FLUSH_TX, 0x00);
}
/* @description	: 清除rx，tx fifo @param		: 无 @return		: 无 */
/* @brief  同时清空 RX / TX FIFO 【参数】 无 【返回】 无 【调用关系】本文件内部没有调用；对外供上层在“通信异常需要彻底重启链路”时使用。 */
void bsp_pn298_clear_fifo(void)
{
	bsp_pn298_write_reg(FLUSH_TX, 0x00);
	bsp_pn298_write_reg(FLUSH_RX, 0x00);
}

/* @description	: 设置tx模式 @param		: 无 @return		: 无 */
/* @brief  把芯片切成“发射模式” 【参数】 无 【返回】 无 【算法/寄存器含义】 CONFIG(0x00) 的 bit0 决定 CE 高电平代表什么： */
void bsp_pn298_tx_mode(void)
{
	uint8_t temp;
	bsp_pn298_ce_disable();
	temp = bsp_pn298_read_reg(0x00);
	temp &= ~(0x01);
	bsp_pn298_write_reg(W_REGISTER + 0x00, temp);
	
	bsp_pn298_flush_tx_fifo();
	bsp_pn298_flush_rx_fifo();
	bsp_pn298_clear_status();
	
	bsp_pn298_ce_enable();
}
/* @description	: 设置为rx模式 @param		: 无 @return		: 无 */
/* @brief  把芯片切成“接收模式”（飞控绝大部分时间都在这个状态） 【参数】 无 【返回】 无 【算法/寄存器含义】 CONFIG(0x00) bit0 置 1 = 接收模式；置位写法 temp |= 0x01。 */
void bsp_pn298_rx_mode(void)
{
	bsp_pn298_ce_disable();
	delay_us(20);
	
	uint8_t temp;
	temp = bsp_pn298_read_reg(0x00);
	temp |= 0x01;
	bsp_pn298_write_reg(W_REGISTER + 0x00, temp);
	
	bsp_pn298_ce_enable();
}
/* @description	: 设置发射功率 @param		: power @return		: 无 */
/* @brief  设置发射功率 【参数】 @param power  16 位功率配置字，取值见 bsp_pn298.h 的 PN298_POWER_* 宏 （例如 PN298_POWER_P_12 = 0xc9cf 表示 12dBm）。 */
void bsp_pn298_set_power(uint16_t power)
{
		uint8_t data[3];
    data[0] = 0x00;
    data[1] = (uint8_t)(power >> 8);
    data[2] = (uint8_t)(power);
		data[2] |= (1 << 5);
    bsp_pn298_write_buf(W_REGISTER + 0x1a, data, 3);
}

/* @brief  set rate @param  rate PN298_RATE_1M   1Mbps PN298_RATE_2M   2Mbps */

void bsp_pan298_set_rate(uint8_t rate)
{
    uint8_t dat[3];
    uint8_t demod_cal, en_aa_reg;
    demod_cal =bsp_pn298_read_reg(DEM_CAL)& 0x01;
    en_aa_reg= bsp_pn298_read_reg(EN_AA)&0x3f;
      
    if (rate == PN298_RF_DATA_RATE_1M)
    {
        en_aa_reg |= DR_1M;
        dat[0] = 0x48; 
        dat[1] = 0x2a; 
        dat[2] = 0xfd;
        demod_cal += 0x1a;
    }
    else if (rate == PN298_RF_DATA_RATE_2M )
    {
        en_aa_reg |= DR_2M;
        dat[0] = 0x49; 
        dat[1] = 0x2a; 
        dat[2] = 0xfd;
        demod_cal += 0x18;
    }
    else if(rate == PN298_RF_DATA_RATE_250K )
    {   
			  en_aa_reg |= DR_250K;
				dat[0] = 0x4B; 
        dat[1] = 0x28; 
        dat[2] = 0xfd;
        demod_cal += 0x1B;
    }
    bsp_pn298_write_reg(W_REGISTER + EN_AA, en_aa_reg);
    bsp_pn298_write_buf(W_REGISTER + RF_SETUP, dat, 3);
    bsp_pn298_write_reg(W_REGISTER + DEM_CAL, demod_cal);
}
/* @description	: 设置信道 @param		: chn @return		: 无 */
/* @brief  设置射频通信信道（频点） 【参数】 @param chn  上层信道号（0~255），工程里用 36（config_rf.h 的 __PN298_INIT_CHN）。 【返回】 无 */
void bsp_pn298_set_channel(uint8_t chn)
{
	chn += 64;
	bsp_pn298_write_reg(W_REGISTER + 0x05, chn);
}
/* @description	: 设置模式 @param		: mode @return		: 无 */
/* @brief  设置芯片工作模式（协议族 + 普通/增强型） 【参数】 @param mode  取值见 bsp_pn298.h： 0x00 XN297L 普通 / 0x01 XN297L 增强 / 0x10 BLE */
void bsp_pn298_set_mode(uint8_t mode)
{
	uint8_t data[5];
    uint8_t temp;
                /* nordic模式关闭扰码 */
	temp = bsp_pn298_read_reg(0x19);
	if((mode == PN298_NORDIC_NORMAL_MODE) || (mode == PN298_NORDIC_ENHANCE_MODE)){
		temp &= ~(0x01);
	}else{
		temp |= 0x01;
	}
	
    bsp_pn298_write_reg(W_REGISTER + 0x19, temp);
    
                /* nordic增强型有单独的时间配置 */
    if(mode == PN298_NORDIC_ENHANCE_MODE){
		data[0] = 0xcc; 
		data[1] = 0x41; 
		data[2] = 0x04;     
    }else{
        data[0] = 0xd3; 
		data[1] = 0x59; 
		data[2] = 0x0c;
    }
    data[3] = 0x50; 
	data[4] = 0x9c;
    bsp_pn298_write_buf(W_REGISTER + 0x1f, data, 5);
  
                /* 设置模式 */
	temp = bsp_pn298_read_reg(0x00);
    temp &= ~(0x03 << 6);
    temp |= (((uint8_t)mode & PN298_MODE_MSK) << 2);
    bsp_pn298_write_reg(W_REGISTER + 0x00, temp); 
    
                /* 增强型与非增强型有不同的配置 */
	temp = bsp_pn298_read_reg(0x01);
    if(((uint8_t)mode & 0x01) == 0){
        temp &= ~(0x01);
        bsp_pn298_write_reg(W_REGISTER + 0x04, 0x00);
    }else{
        temp |= 0x01;
        bsp_pn298_write_reg(W_REGISTER + 0x04, 0x03);                  /* 若失败不自动重传 */
    }
    bsp_pn298_write_reg(W_REGISTER + 0x01, temp);
}

/* @description	: 设置通信地址 @param		: reg rf_addr_array addr_len */

void bsp_pn298_set_addr(uint8_t reg, uint8_t *rf_addr_array, uint8_t addr_len)
{
	bsp_pn298_write_buf(W_REGISTER + reg, rf_addr_array, addr_len);
}
/* @description	: 设置payload长度 @param		: len @return		: 无 */
/* @brief  设置接收通道 0 的有效载荷长度（RX_PW_P0） 【参数】 @param len  字节数。本函数限定 len <= 64。 【返回】 无 */
void bsp_pn298_set_rx_payload_len(uint8_t len)
{
	if(len <= 64){
		bsp_pn298_write_reg(W_REGISTER + 0x11, len);
	}
}
/* @description	: 反位序和字节序 @param		: *buf len @return		: 无 */
/* @brief  把整包数据做“比特镜像”：字节顺序首尾对调，同时每个字节内部 bit7..0 倒过来 【参数】 @param buf  数据缓冲区（原地修改） @param len  字节数 */
void bsp_pn298_byte_bit_decode(uint8_t* buf, uint8_t len)
{
    uint8_t data1;
    uint8_t data2;
    uint8_t i;
    uint8_t j;
    uint8_t* p1;
    uint8_t* p2;
    
    i = len >> 1;
    p1 = buf;
    p2 = buf + len - 1;

    if((len & 1) != 0){
                                /* len奇数, 中间一个字节下面while里不会处理, 先额外处理 */
        if(len > 1){
            buf = buf + i;               
        }
        data1 = *buf;
        
        *buf = 0;
        for(j = 0; j < 7; j++){
            *buf += ((data1 >> j) & 0x01);
            *buf = *buf << 1;
        }
        *buf += (data1 >> 7);
    }
    
    while(i--){
        data1 = *p1;
        data2 = *p2;
        *p1 = *p2 = 0;
        for(j = 0; j < 7; j++){
            *p1 += ((data2 >> j) & 0x01);
            *p1 = *p1 << 1;
            *p2 += ((data1 >> j) & 0x01);
            *p2 = *p2 << 1;
        }
        *p1 += (data2 >> 7);
        *p2 += (data1 >> 7);
        p1++;
        p2--;
    }
}
/* @description	: 发送数据 @param		: data len @return		: 无 */
/* @brief  发送一包数据（不做应答载荷处理，发送后只等状态位） 【参数】 @param data  待发数据首地址 @param len   字节数 */
void bsp_pn298_tx_data(uint8_t *data, uint8_t len)
{
	bsp_pn298_write_buf(W_TX_PAYLOAD, data, len);
	
	if(__CHIP_TYPE)
	{ //pan7421
		while(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0) == GPIO_PIN_SET);
	}
	else
	{
		delay_ms(2);
		while(!((bsp_pn298_read_reg(RF_STATUS) & TX_DS_FLAG) == TX_DS_FLAG));
	}
	bsp_pn298_flush_tx_fifo();
	bsp_pn298_clear_status();
}



uint8_t status;

/* 【注意】cnt 只在一句被注释掉的 printf 里出现过，现在是“定义了但没人用”的变量， 会占 2 字节 RAM；保留它只是作者的调试痕迹（未改动）。 */
uint16_t cnt = 0;

/* @brief  发送一包数据，并尝试接收对方随 ACK 一起回传的数据 【参数】 @param data      待发数据 @param ack_data  用来接收“ACK 载荷”的缓冲区（对方回传的数据） */
void bsp_pn298_tx_data_ack(uint8_t *data, uint8_t *ack_data, uint8_t len)
{
	bsp_pn298_write_buf(W_TX_PAYLOAD, data, len);
	if(__CHIP_TYPE)
	{//PAN7421
		while(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0) == GPIO_PIN_SET);
		status = (bsp_pn298_read_reg(RF_STATUS) & 0x70);
	}
	else
	{
		delay_ms(3);
		status = (bsp_pn298_read_reg(RF_STATUS) & 0x70);
	}
	
	if(status == RX_TX_FLAG)
	{
		bsp_pn298_read_buf(R_RX_PAYLOAD, ack_data, len);	//读取ACK数据
				// printf("RX_TX_FLAG cnt:%d\r\n", ++cnt);
	}
	else if(status == TX_DS_FLAG)
	{
				// printf("TX_DS_FLAG \r\n");
	}
	else if(status == MAX_RT_FLAG)
	{
				// printf("MAX_RT_FLAG \r\n");
	}
	else
	{
				// printf("send err! rf status = 0 \r\n");
	}
	
	bsp_pn298_flush_rx_fifo();
	bsp_pn298_flush_tx_fifo();
	bsp_pn298_clear_status();
}
/* @description	: 接收数据 @param		: *data len @return		: 0 或 1 */
/* @brief  收一包数据（不带 ACK 载荷的简化版） 【参数】 @param data  接收缓冲区 @param len   期望字节数 */
uint8_t	bsp_pn298_rx_data(uint8_t *data,uint8_t len)
{
	if(__CHIP_TYPE)
	{ //pan7421
		if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0)==GPIO_PIN_SET)
		{
			return 0;
		}
	}
	else
	{
		if(!((bsp_pn298_read_reg(RF_STATUS) & RX_DR_FLAG) == RX_DR_FLAG))
		{
			return 0;
		}
	}
	bsp_pn298_read_buf(R_RX_PAYLOAD, data, len);
	bsp_pn298_byte_bit_decode(data, len);
	bsp_pn298_flush_rx_fifo();
	bsp_pn298_clear_status();
	return 1;
}
/* @description	: 接收数据，并待ack @param		: *rx_data ack_data len */

/* @brief  【飞机侧收包主路径】收一包数据，并在应答窗口里把 ack_data 作为 ACK 载荷回给对方 【参数】 @param rx_data   收到的数据放这里 @param ack_data  要回传给对方（遥控器）的数据 */
uint8_t bsp_pn298_rx_data_ack_fix(uint8_t *rx_data, uint8_t *ack_data, uint8_t len, uint8_t ack)
{
	if(__CHIP_TYPE)
	{ //pan7421
		if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_0)==GPIO_PIN_SET)
		{
			return 0;
		}
	}
	else
	{
		if(!((bsp_pn298_read_reg(RF_STATUS) & RX_DR_FLAG) == RX_DR_FLAG))
		{
			return 0;
		}
	}

	if(ack)
	{
		bsp_pn298_write_reg(FLUSH_TX, 0);						// 清除发送缓冲
		bsp_pn298_write_buf(W_ACK_PAYLOAD, ack_data, len);	// 发送ACK数据
	}
	delay_us(30);
	bsp_pn298_read_buf(R_RX_PAYLOAD, rx_data, len);			// 接收数据
	bsp_pn298_byte_bit_decode(rx_data, len);
	bsp_pn298_write_reg(FLUSH_RX, 0);										// 清除接收缓冲
	bsp_pn298_write_reg(W_REGISTER + RF_STATUS, 0x70);  //清除状态
	return 1;
}
/* @description	: 1.8v寄存器初始化 @param		: 无 @return		: 无 */
/* @brief  配置芯片 1.8V 电源域的基础寄存器（射频能工作的前提） 【参数】 无 【返回】 无 【算法：每个寄存器块为什么填这些值】 这一段的数值全部来自厂商例程，属于“照抄但必须抄对”的初始化序列。 */
void __bsp_pn298_1v8_reg_init(void)
{
	uint8_t data[5];
	uint8_t demod_cal;
	
	data[0] = 0x20; 
  data[1] = 0xd0; data[2] = 0xff;       /* 电流大 接收灵敏度好 */

  bsp_pn298_write_buf(W_REGISTER + 0x03, data, 3);
    
	data[0] = 0x48; 
	data[1] = 0x2a; 
	data[2] = 0xfd;
  bsp_pn298_write_buf(W_REGISTER + 0x06, data, 3);
	data[0] = 0xfd; 
	data[1] = 0x3f;
  bsp_pn298_write_buf(W_REGISTER + 0x0e, data, 2);
    
	data[0] = 0x8b;
	data[1] = 0xc9; //A版本：0xc9; B版本：0x09
	data[2] = 0x5f;
	bsp_pn298_write_buf(W_REGISTER + 0x18, data, 3);
	
	demod_cal = 0x1b;
	bsp_pn298_write_reg(W_REGISTER + 0x19, demod_cal);
		
// data[0] = 0x60;
// data[1] = 0x0e;
// bsp_pn298_write_buf(W_REGISTER + 0x1e, data, 2);

                /* PN298晶振时钟输出到PAD 寄存器RF_CAL:0x1e [15]:CLK_OUTPAD复用: 0:测试PAD; 1:时钟输出PAD */
    data[0] = 0x60;
    data[1] = 0xfe; //0xce:16Mhz
    bsp_pn298_write_buf(W_REGISTER + 0x1e, data, 2);

                /* Feature, 直接64字节模式, 不使用32字节模式 */
    bsp_pn298_write_reg(W_REGISTER + 0x1d, 0x38);
    
                /* noIRQ_noWORK使能 */
    bsp_pn298_write_reg(W_REGISTER + 0x1c, 0xc0);
}



/* @description	: pn298初始化 @param		: *addr addr_len chn */

void __bsp_pn298_init(uint8_t* addr, uint8_t addr_len, uint8_t data_rate,uint8_t chn, uint8_t payload, uint16_t power, uint8_t mode)
{
	uint8_t data[2];
	uint8_t f1_data[2];

	bsp_pn298_ce_disable();
	delay_us(20);
	
	flash_read(FLASH_USER_START_ADDR);
	get_now_addr();
	
		// uint8_t temp;
		// uint8_t addr2[5] = {0xCF,0x8F,0x34,0xC9,0xa2};
				/* 设置通信接口为spi */
	data[1] = 0x08;
  data[0] = 0x4f;
	bsp_pn298_write_buf(REG1_3V_WRITE, data, 2);
	
				/* 1.8v 逻辑复位 */
	f1_data[0] = 0x00;
	f1_data[1] = 0x01;
  bsp_pn298_write_reg(REG0_3V_WRITE, f1_data[0]);
	delay_ms(2);
  bsp_pn298_write_reg(REG0_3V_WRITE, f1_data[1]);
	
				/* 1.8v 寄存器初始化 */
	__bsp_pn298_1v8_reg_init();
	
				/* 打开ce */
	bsp_pn298_ce_enable();



// temp = bsp_pn298_read_reg(0x02);
// temp &= ~(0x03 << 6);
// temp |= ((addr_len - 2) << 6);
// printf("sdsdsd %x\r\n",temp);
// bsp_pn298_write_reg(W_REGISTER + 0x02, temp);
	bsp_pn298_write_reg(W_REGISTER + AW_RXADDR, 0xC3);
				/* 设置速率 */
	bsp_pan298_set_rate(data_rate);
				/* 设置模式 */
	bsp_pn298_set_mode(mode);
				/* 设置tx通信地址 */
	bsp_pn298_set_addr(TX_ADDR, addr, addr_len);
                /* 设置rx通信地址 */
	
	bsp_pn298_set_addr(RX_ADDR_P0, pair_addr_arr, addr_len);
	bsp_pn298_set_addr(RX_ADDR_P1, addr, addr_len);
	
				/* 设置发射功率 */
	bsp_pn298_set_power(power);
				/* 设置通信信道 */
	bsp_pn298_set_channel(chn);
				/* 设置rx payload长度 */
// bsp_pn298_set_rx_payload_len(payload);
	bsp_pn298_write_reg(W_REGISTER + RX_PW_P0, payload);
	bsp_pn298_write_reg(W_REGISTER + RX_PW_P1, payload);
	if((uint8_t)mode  == 0x01){
		bsp_pn298_write_reg(ACTIVATE, 0x73);
		bsp_pn298_write_reg(W_REGISTER + FEATURE, 0x3A);
		bsp_pn298_write_reg(W_REGISTER + EN_AA, 0x03);
	}
}

/* @description	: pn298初始化，并设置为rx模式 @param		: 无 @return		: 无 */
/* @brief  【上层调用的初始化入口】用工程默认参数初始化 PN298，然后切到接收模式 【参数】 无（参数全部来自 config_rf.h / bsp_pn298.h 的宏） 【返回】 无 【算法】 */
void bsp_pn298_init(void)
{
		uint8_t addr[] = __PN298_INIT_ADDR;
    __bsp_pn298_init(addr,__PN298_INIT_ADDR_LEN,__PN298_RATE,__PN298_INIT_CHN,__PN298_PAYLOAD,__PN298_POWER, __PN298_MODE);
    bsp_pn298_rx_mode();
}



/* @brief  单载波 @param  None @retval None */
/* @brief  发射单载波（未调制的连续载波，产测/射频认证用） 【参数】 无 【返回】 无 【算法】把芯片配成“只发一个固定频率的载波、不带任何数据”， 用于： */
void pn298_carrier(void)
{
    uint8_t dat[3] = {0x0b, 0xdf, 0x02};
    
    bsp_pn298_write_reg(W_REGISTER + 0x1C, 0x40);
    bsp_pn298_write_reg(W_REGISTER + 0x00, 0x06);
    bsp_pn298_write_reg(W_REGISTER + 0x19, 0xaf);
    bsp_pn298_write_buf(W_REGISTER + 0x1B, dat, 3);

    bsp_pn298_ce_disable();
}



/* @brief  深度睡眠, 进入后所有配置丢失, 通过重新初始化唤醒 @param  None @retval None */
/* @brief  让 PN298 进入深度睡眠（低功耗） 【参数】 无 【返回】 无 【算法/为什么】 先 ce_disable() 让它停下收发，再写 0xF3（REG1_3V_WRITE）0x40 —— */
void pn298_deep_sleep(void)
{
    bsp_pn298_ce_disable();
    bsp_pn298_write_reg(0xF3, 0x40);
}

/* @brief  运行中切换通信地址与信道（对码成功后由 main.c 调用） 【参数】 @param addr  新的 5 字节通信地址（pair_addr_arr，来自遥控器的对码包） @param ch    新的信道（pair_data.rf_ch，对码时协商好的频点） */
void bsp_pn298_change_addr(uint8_t * addr,uint8_t ch){
	
	bsp_pn298_ce_disable();
	delay_us(20);
	
								/* 设置tx通信地址 */
	bsp_pn298_set_addr(TX_ADDR, addr, 5);
                /* 设置rx通信地址 */
	bsp_pn298_set_addr(RX_ADDR_P0, addr, 5);
				/* 设置通信信道 */
	bsp_pn298_set_channel(ch);
	
	bsp_pn298_ce_enable();
}







