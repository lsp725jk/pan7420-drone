/**
 * @file     bsp_spi_w25q64.c
 * @author   embedfire
 * @version  V1.0
 * @date     2024
 * @brief    spi flash 底层应用函数bsp
 */
/* 一、这个文件在项目里干什么 配置 SPI1 为主机（8bit、低电平空闲、上升沿采样），并给出六个寄存器读写函数： 两个传感器挂在同一条 SPI 总线上，用各自的片选（CS）分时复用。 二、它在整个飞控里的位置（数据流） */
#include "spi.h" 
#include "bsp.h"

extern SPI_HandleTypeDef Spi1Handle;   /**
 * SPI1 句柄：定义在 common.c，配置信息挂在这里
 */
#define SPI_TIMEOUT 200                /* HAL_SPI 超时（单位 ms）。SPI 一次收发是微秒级，200ms 只是兜底，防死等 */



/* @brief  SPI1 总线初始化（主机模式） 【参数】无（历史上曾带 mode 参数，现已固定为主机 2 线模式） 【返回】0 = 成功（原作者模板里的 1 = 失败分支实际未被使用） 【算法/为什么这么写】 */
uint8_t spi_init(void)
{
								/* SPI1 parameter configuration */
		Spi1Handle.Instance = SPI1;
		Spi1Handle.Init.Mode = SPI_MODE_MASTER;
		Spi1Handle.Init.Direction = SPI_DIRECTION_2LINES;
		Spi1Handle.Init.DataSize = SPI_DATASIZE_8BIT;
		Spi1Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
		Spi1Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
		Spi1Handle.Init.NSS = SPI_NSS_SOFT;
		Spi1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		Spi1Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
		if (HAL_SPI_Init(&Spi1Handle) != HAL_OK)
		{
						// Error_Handler();
		}
								/* USER CODE BEGIN SPI1_Init 2 */

								/* USER CODE END SPI1_Init 2 */
		SPI_BMP390_CS_HIGH();
		SPI_ICM42688_CS_HIGH();
		return 0;
}



// 单字节收发函数
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{
    uint8_t RxData = 0;
    HAL_StatusTypeDef status;
    
        // 发送并接收数据
    status = HAL_SPI_TransmitReceive(&Spi1Handle, &TxData, &RxData, 1, SPI_TIMEOUT);
    
        // 检查传输状态
    if(status != HAL_OK) {
        return 0; // 传输失败返回0
    }
    return RxData;
}

/* static：只在本文件可见，避免符号污染（这个“内部小工具”不需要对外暴露）。 它是 inv_io_hal_write_reg() 的底层实现：驱动层给的是“寄存器+数据”，这里负责凑出 “拉 CS → 发地址 → 发数据 → 松 CS”的时序。 */

// 寄存器写函数
static uint8_t io_write_reg(uint8_t reg, uint8_t value)
{
#if defined(ICM_USE_HARD_SPI)
	SPI_BMP390_CS_HIGH();
    SPI_ICM42688_CS_LOW();
    
        // 发送寄存器地址
    SPI2_ReadWriteByte(reg);
        // 发送寄存器值
    SPI2_ReadWriteByte(value);
    
	SPI_ICM42688_CS_HIGH();
	SPI_BMP390_CS_HIGH();
#elif defined(ICM_USE_I2C)
    IICwriteBytes(ICM42688_ADDRESS, reg, 1, &value);
#endif
    return 0;
}

/* @brief  ICM42688 多字节读（ICM42688 驱动层的“串行接口回调”） 【参数】serif  ：驱动层的接口描述结构（本实现不使用其中内容，只占位） reg    ：起始寄存器地址 rbuffer：读出数据的存放缓冲区 */
// 多字节读函数
int inv_io_hal_read_reg(struct inv_icm426xx_serif *serif, uint8_t reg, uint8_t *rbuffer, uint32_t rlen)
{
#if defined(ICM_USE_HARD_SPI)

    reg |= 0x80; // 设置读标志位
    
	SPI_BMP390_CS_HIGH();
    SPI_ICM42688_CS_LOW();
    
        // 发送要读取的寄存器地址
    SPI2_ReadWriteByte(reg);
    
        // 连续读取数据
    for(uint32_t i = 0; i < rlen; i++) {
        rbuffer[i] = SPI2_ReadWriteByte(0x00);
    }
		SPI_ICM42688_CS_HIGH();
		SPI_BMP390_CS_HIGH();
#elif defined(ICM_USE_I2C)
    IICreadBytes(ICM42688_ADDRESS, reg, rlen, rbuffer);
#endif
    return 0;
}

/* @brief  ICM42688 多字节写（驱动层的串行接口回调） 【参数】serif：接口描述结构（未使用）；reg：起始寄存器地址； wbuffer：待写数据；wlen：字节数 【返回】0 成功，非 0 为底层写失败 */
// 多字节写函数
int inv_io_hal_write_reg(struct inv_icm426xx_serif *serif, uint8_t reg, const uint8_t *wbuffer, uint32_t wlen)
{
    int rc = 0;
    for(uint32_t i = 0; i < wlen; i++) {
        rc = io_write_reg(reg + i, wbuffer[i]);
        if(rc) break;
    }
    return rc;
}

/* 名    称： icm42688_read_reg 功    能： 读取单个寄存器的值 入口参数： reg: 寄存器地址 出口参数： 当前寄存器地址的值 */

uint8_t icm42688_read_reg(uint8_t reg)
{
    uint8_t regval = 0;

#if defined(ICM_USE_HARD_SPI)
    SPI_BMP390_CS_HIGH();
    SPI_ICM42688_CS_LOW();
    reg |= 0x80;
                /* 写入要读的寄存器地址 */
    SPI2_ReadWriteByte(reg);
                /* 读取寄存器数据 */
    regval = SPI2_ReadWriteByte(0xFF);
	SPI_ICM42688_CS_HIGH();
	SPI_BMP390_CS_HIGH();
#elif defined(ICM_USE_I2C)
	IICreadBytes(ICM42688_ADDRESS, reg, 1, &regval);
#endif

    return regval;
}

/* 名    称： icm42688_read_regs 功    能： 连续读取多个寄存器的值 入口参数： reg: 起始寄存器地址 *buf数据指针,uint16_t len长度 出口参数： 无 */

uint8_t icm42688_read_regs(uint8_t reg, uint8_t* buf, uint16_t len)
{
#if defined(ICM_USE_HARD_SPI)
    reg |= 0x80;
    SPI_BMP390_CS_HIGH();
    SPI_ICM42688_CS_LOW();
                /* 写入要读的寄存器地址 */
    SPI2_ReadWriteByte(reg);
                /* 读取寄存器数据 */
    while(len)
	{
		*buf = SPI2_ReadWriteByte(0x00);
		len--;
		buf++;
	}
	SPI_ICM42688_CS_HIGH();
	SPI_BMP390_CS_HIGH();
	return 0;
#elif defined(ICM_USE_I2C)
	IICreadBytes(ICM42688_ADDRESS, reg, len, buf);
#endif
}



/* 名    称： icm42688_write_reg 功    能： 向单个寄存器写数据 入口参数： reg: 寄存器地址 value:数据 出口参数： 0 */

uint8_t icm42688_write_reg(uint8_t reg, uint8_t value)
{
#if defined(ICM_USE_HARD_SPI)
     SPI_BMP390_CS_HIGH();
    SPI_ICM42688_CS_LOW();
                /* 写入要读的寄存器地址 */
                /* 写入要读的寄存器地址 */
    SPI2_ReadWriteByte(reg);
                /* 读取寄存器数据 */
    SPI2_ReadWriteByte(value);
    SPI_ICM42688_CS_HIGH();
	SPI_BMP390_CS_HIGH();
#elif defined(ICM_USE_I2C)
	IICwriteBytes(ICM42688_ADDRESS, reg, 1, &value);
#endif
    return 0;
}



// 寄存器写函数
bool bmp3_spi_Write(uint8_t reg, uint8_t data)
{

	SPI_BMP390_CS_LOW();
    SPI_ICM42688_CS_HIGH();
        // 发送寄存器地址
    SPI2_ReadWriteByte(reg);
        // 发送寄存器值
    SPI2_ReadWriteByte(data);
   
	SPI_ICM42688_CS_HIGH();
	SPI_BMP390_CS_HIGH();

    return true;
}



// 多字节读函数
bool bmp3_spi_ReadBuf(uint8_t reg, uint8_t *data, uint8_t length)
{
    reg |= 0x80; // 设置读标志位
    
	SPI_BMP390_CS_LOW();
    SPI_ICM42688_CS_HIGH();
	
        // 发送要读取的寄存器地址
    SPI2_ReadWriteByte(reg);
    
        // 连续读取数据
    for(uint32_t i = 0; i < length; i++) {
        data[i] = SPI2_ReadWriteByte(0x00);
    }
		SPI_ICM42688_CS_HIGH();
		SPI_BMP390_CS_HIGH();
	return true;
}



