/**
 * \
 */

/**
 * 文件：common.c                        【角色】BMP390 气压计驱动适配 + 气压数据采集状态机
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bsp.h"
#include "bmp3.h"
#include "common.h"
#include "main.h"

/* I2C1 句柄：本工程气压计走 SPI，此句柄仅被 bmp3_i2c_read/write 使用（当前不会被调用） */
extern I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef Spi1Handle;



uint8_t settings_sel;
struct bmp3_dev dev;
struct bmp3_settings settings = {0};



// FORCED模式状态枚举
/* FORCED（单次强制）模式的三个状态：气压计“测一次”要分三步走， 这样每次调用只需要几十微秒，不会阻塞主循环。见下面 baro_update() 的 switch */
typedef enum {
    BARO_FORCED_TRIGGER_MEASURE,      // 触发测量
    BARO_FORCED_WAIT_COMPLETION,      // 等待测量完成
    BARO_FORCED_READ_DATA,            // 读取数据
} BaroForcedState_e;
// 在common.c开头（变量定义区域）添加

bool baroSensorReady = false;  // 定义并初始化传感器就绪标志（默认未就绪）



/* 气压计节拍计数器：TIM17 的 1ms 中断里自增（drivers/bsp/bsp_timer.c）， baro_update() 与 imu/altitude.c 都读它 */
uint8_t baroUpdateTimer;

/* ! BMP3 shuttle board ID */
#define BMP3_SHUTTLE_ID 0xD3

/* Variable to store the device address */
/* static：只在本文件可见。注意它被下面两个 I2C 回调里的“同名局部变量”遮蔽 （局部 uint8_t dev_addr = *(uint8_t *)intf_ptr;），所以这个全局量当前没人用， 属于历史遗留；未改动代码 */
static uint8_t dev_addr;

/* ! I2C read function map to COINES platform */

BMP3_INTF_RET_TYPE bmp3_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t *)intf_ptr;
    return HAL_I2C_Mem_Read(&hi2c1, dev_addr << 0x01, reg_addr, 1, reg_data, (uint16_t)len, 100);
}

/* ! I2C write function map to COINES platform */
/* @brief  I2C 写回调（本工程未使用，保留以便切换总线） 【参数】同 bmp3_i2c_read() 【返回】HAL 状态码 【调用关系】由 user/bmp3.c 通过 bmp3->write 函数指针调用。 */
BMP3_INTF_RET_TYPE bmp3_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t dev_addr = *(uint8_t *)intf_ptr;
    return HAL_I2C_Mem_Write(&hi2c1, dev_addr << 0x01, reg_addr, 1, (uint8_t *)reg_data, (uint16_t)len, 100);
}

/* ! SPI read function map to COINES platform */
/* @brief  SPI 读回调（本工程实际使用的读路径） 【参数】reg_addr：寄存器地址；reg_data：读出缓冲；len：字节数；intf_ptr：未使用 【返回】HAL 状态码（HAL_OK=0 成功） 【算法】 */
BMP3_INTF_RET_TYPE bmp3_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t rslt = 0;
    uint8_t reg_spi[1] = {reg_addr | 0x80};
    SPI_BMP390_CS_LOW();
		SPI_ICM42688_CS_HIGH();
    HAL_SPI_Transmit(&Spi1Handle, reg_spi, 1, 100);
    rslt = HAL_SPI_Receive(&Spi1Handle, reg_data, (uint16_t)len, 100);
    SPI_BMP390_CS_HIGH();
		SPI_ICM42688_CS_HIGH();
    return rslt;
}

/* ! SPI write function map to COINES platform */
/* @brief  SPI 写回调 【参数】reg_addr：寄存器地址；reg_data：待写数据；len：字节数；intf_ptr：未使用 【返回】HAL 状态码 【算法】 */
BMP3_INTF_RET_TYPE bmp3_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    uint8_t reg_spi[1] = {reg_addr & 0x7f};
    uint8_t rslt = 0;
    SPI_BMP390_CS_LOW();
		SPI_ICM42688_CS_HIGH();
    while (HAL_SPI_GetState(&Spi1Handle) == HAL_SPI_STATE_BUSY_TX) ;
    HAL_SPI_Transmit(&Spi1Handle, reg_spi, 1, 100);
    rslt = HAL_SPI_Transmit(&Spi1Handle, (uint8_t *)reg_data, (uint16_t)len, 100);
    SPI_BMP390_CS_HIGH();
		SPI_ICM42688_CS_HIGH();
    return rslt;
}

/* ! Delay function map to COINES platform */
/* @brief  延时回调（原厂驱动要求“等 period 微秒”） 【参数】period：驱动要求的微秒数（本实现忽略）；intf_ptr：未使用 【返回】无 【算法/为什么这么写】直接 HAL_Delay(1)（1 毫秒）——比要求的微秒级更长。 */
void bmp3_delay_us(uint32_t period, void *intf_ptr)
{
    HAL_Delay(1);
}

/* @brief  把原厂驱动的返回码翻译成人能看懂的串口打印（调试用） 【参数】api_name：出错的 API 名字；rslt：返回码 【返回】无 【为什么这么写】原厂库用一堆负数错误码（BMP3_E_COMM_FAIL 等）表示失败， */
void bmp3_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
    case BMP3_OK:

                                /* Do nothing */
        break;
    case BMP3_E_NULL_PTR:
        printf("API [%s] Error [%d] : Null pointer\r\n", api_name, rslt);
        break;
    case BMP3_E_COMM_FAIL:
        printf("API [%s] Error [%d] : Communication failure\r\n", api_name, rslt);
        break;
    case BMP3_E_INVALID_LEN:
        printf("API [%s] Error [%d] : Incorrect length parameter\r\n", api_name, rslt);
        break;
    case BMP3_E_DEV_NOT_FOUND:
        printf("API [%s] Error [%d] : Device not found\r\n", api_name, rslt);
        break;
    case BMP3_E_CONFIGURATION_ERR:
        printf("API [%s] Error [%d] : Configuration Error\r\n", api_name, rslt);
        break;
    case BMP3_W_SENSOR_NOT_ENABLED:
        printf("API [%s] Error [%d] : Warning when Sensor not enabled\r\n", api_name, rslt);
        break;
    case BMP3_W_INVALID_FIFO_REQ_FRAME_CNT:
        printf("API [%s] Error [%d] : Warning when Fifo watermark level is not in limit\r\n", api_name, rslt);
        break;
    default:
        printf("API [%s] Error [%d] : Unknown error code\r\n", api_name, rslt);
        break;
    }
}

/* @brief  选择并注册气压计的总线接口（I2C 或 SPI） 【返回】BMP3_OK(0) 成功 【算法/为什么这么写】把“用哪条总线”抽象成函数指针： 之后原厂驱动内部读写就自动走对应的回调，驱动本身不需要知道是 SPI 还是 I2C。 */
BMP3_INTF_RET_TYPE bmp3_interface_init(struct bmp3_dev *bmp3, uint8_t intf)
{
    int8_t rslt = BMP3_OK;

                /* Bus configuration : I2C（两线串行总线） */
    if (intf == BMP3_I2C_INTF)
    {
        printf("I2C Interface\n");
        dev_addr = BMP3_ADDR_I2C_SEC;
        bmp3->read = bmp3_i2c_read;
        bmp3->write = bmp3_i2c_write;
        bmp3->intf = BMP3_I2C_INTF;
    }
                /* Bus configuration : SPI（串行外设接口） */
    else if (intf == BMP3_SPI_INTF)
    {
        printf("SPI Interface\n");
                // dev_addr = COINES_SHUTTLE_PIN_7;
        bmp3->read = bmp3_spi_read;
        bmp3->write = bmp3_spi_write;
        bmp3->intf = BMP3_SPI_INTF;
    }

    HAL_Delay(100);
    bmp3->delay_us = bmp3_delay_us;
    bmp3->intf_ptr = &dev_addr;

    return rslt;
}
// //BMP390初始化
// void BMP390_init(void)
// {
// int8_t rslt = 0;

// rslt = bmp3_interface_init(&dev, BMP3_SPI_INTF);  //选择SPI通信方式
// bmp3_check_rslt("bmp3_interface_init", rslt);

// rslt = bmp3_init(&dev);		//选择接口，读取传感器的芯片id和校准数据
// bmp3_check_rslt("bmp3_init", rslt);

// settings.int_settings.drdy_en = BMP3_DISABLE;		//使能中断结构
// settings.press_en = BMP3_ENABLE;			// 开启压力传感器
// settings.temp_en = BMP3_ENABLE;			  //启用温度传感器

// settings.odr_filter.press_os = BMP3_OVERSAMPLING_16X;		//压力过采样设置
// settings.odr_filter.temp_os = BMP3_OVERSAMPLING_8X;			//温度过采样设置
// settings.odr_filter.odr = BMP3_ODR_12_5_HZ;							//数据输出速率设置
// settings.odr_filter.iir_filter = BMP3_IIR_FILTER_COEFF_7;			//数据过滤器设置

// settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS | BMP3_SEL_ODR | BMP3_SEL_IIR_FILTER | BMP3_SEL_DRDY_EN;// 选择要应用的设置

// rslt = bmp3_set_sensor_settings(settings_sel, &settings, &dev);		// 应用设置
// bmp3_check_rslt("bmp3_set_sensor_settings", rslt);

// settings.op_mode = BMP3_MODE_NORMAL;
// rslt = bmp3_set_op_mode(&settings, &dev);			//设置模式
// bmp3_check_rslt("bmp3_set_op_mode", rslt);
// }

/* 上面被整段注释掉的是“NORMAL（连续测量）模式”的旧初始化代码， 现改用下面的 FORCED 模式版本（原因见本文件文件头：按需测量、不阻塞主循环）。 注释保留是作者的历史记录，未改动。 */
/* @brief  初始化 BMP390 并配置为 FORCED（单次强制测量）模式 【参数】无 【返回】无（失败只打印错误码，不阻塞启动） 【算法步骤】 */
// BMP390 FORCED模式初始化
void BMP390_init(void)
{
    int8_t rslt = 0;

        // 初始化接口（SPI）
    rslt = bmp3_interface_init(&dev, BMP3_SPI_INTF);
    bmp3_check_rslt("bmp3_interface_init", rslt);

        // 设备初始化
    rslt = bmp3_init(&dev);
    bmp3_check_rslt("bmp3_init", rslt);

        // 配置传感器参数
    settings.int_settings.drdy_en = BMP3_DISABLE;  // 禁用数据就绪中断
    settings.press_en = BMP3_ENABLE;               // 启用气压测量
    settings.temp_en = BMP3_ENABLE;                // 启用温度测量

        // 配置过采样率
    settings.odr_filter.press_os = BMP3_OVERSAMPLING_8X;  // 气压过采样16X
    settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;    // 温度过采样8X
    
        // FORCED模式下ODR设置不影响，但需要有效值
    settings.odr_filter.odr = BMP3_ODR_12_5_HZ;
    settings.odr_filter.iir_filter = BMP3_IIR_FILTER_COEFF_3;

        // 选择配置项
    settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | 
                   BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS | 
                   BMP3_SEL_ODR | BMP3_SEL_IIR_FILTER;

        // 应用配置
    rslt = bmp3_set_sensor_settings(settings_sel, &settings, &dev);
    bmp3_check_rslt("bmp3_set_sensor_settings", rslt);

        // 设置为FORCED模式
    settings.op_mode = BMP3_MODE_FORCED;
    rslt = bmp3_set_op_mode(&settings, &dev);
    bmp3_check_rslt("bmp3_set_op_mode", rslt);

                /* 置就绪标志：baro_update() 每次进来先检查它（false 就直接返回，不碰 SPI） */
    baroSensorReady = true;
}



// // 获取校准后的传感器数据
// bool baro_update(void)
// {
// struct bmp3_data data = {0};

// uint8_t sensor_comp = BMP3_PRESS_TEMP; // 同时采集压力和温度
// if(baroUpdateTimer>=40)
// {
// baroUpdateTimer = 0;
// if(bmp3_get_sensor_data(sensor_comp, &data, &dev) == BMP3_OK) // 获取并补偿数据
// {
// // 数据单位转换
// baroPressure=data.pressure;    // Pa转hPa
// baroTemperature=data.temperature; // 摄氏温度
// //					printf("baro:%d\r\n",baroPressure);
// return true;
// }
// }
// return false;
// }



/* @brief  气压计数据采集三态机（触发 → 等待 → 读取） 【参数】无 【返回】true = 本次真的读到了新数据（气压与温度已更新到全局量） 【算法/为什么这么写】 */
// FORCED模式数据采集函数
bool baro_update(void)
{   
    static uint8_t baroUpdateCyc = 0;           /* 本次状态要在多少“计数”之后才继续（软定时阈值） */
    static BaroForcedState_e baroState = BARO_FORCED_TRIGGER_MEASURE;           /* 当前处于哪一步 */
    bool getPressureAndTempeture = false;
    int8_t rslt = BMP3_OK;
    
                /* 传感器未就绪（BMP390_init 没跑成功）就直接退出：不碰 SPI 更安全 */
    if(baroSensorReady != true) return false;
    
                /* 软件定时：baroUpdateTimer 由 1ms 中断累加，达到本次设定的阈值才推进一步 */
    if(baroUpdateTimer >= baroUpdateCyc) {			
        baroUpdateTimer = 0;
        
        switch(baroState) {  
            case BARO_FORCED_TRIGGER_MEASURE:		//触发测量
                                // 触发新的测量
                                                                /* 写 FORCED 模式寄存器：传感器立刻开始一次测量，测完自动回到待机 */
                settings.op_mode = BMP3_MODE_FORCED;
                rslt = bmp3_set_op_mode(&settings, &dev);
                bmp3_check_rslt("bmp3_set_op_mode(forced)", rslt);
                
                                // 根据过采样率设置等待时间
                                // 16X气压 + 8X温度 ≈ 6ms，加上安全余量
                                /* 等约 25 个计数（1ms/计数）再来问结果，远大于测量所需时间， 宁可多等也不空轮询（每次轮询都是一次 SPI 事务，费时间）。 注意原作者注释里同时写着“≈6ms”“8ms”“25”，与代码值不一致， 属于改参数时没改注释，未改动代码。 */
                baroUpdateCyc = 25;  // 8ms等待时间
                baroState = BARO_FORCED_WAIT_COMPLETION;
                break;
            
            case BARO_FORCED_WAIT_COMPLETION:		// 等待测量完成
                                // 检查数据是否就绪
                {
                                                                                /* 读状态寄存器，看“气压/温度数据就绪（drdy）”标志。 struct bmp3_status 里的 sensor.drdy_press/drdy_temp 是原厂库解析好的位域 */
                    struct bmp3_status status = {0};
                    rslt = bmp3_get_status(&status, &dev);
                    if (rslt == BMP3_OK && 
                        (status.sensor.drdy_press || status.sensor.drdy_temp)) {
                        baroState = BARO_FORCED_READ_DATA;
                        baroUpdateCyc = 1;                                                   /* 就绪了：下次调用（1 个计数后）就去读 */
                    } else {
                                                // 数据未就绪，继续等待
                                                                                                /* 未就绪也只等 1 个计数就再来问：状态 2 是“只读一个状态字节”的轻量操作 */
                        baroUpdateCyc = 1;
                    }
                }
                break;
            
            case BARO_FORCED_READ_DATA:					// 读取数据
                                // 读取并补偿数据
                {
                    struct bmp3_data data = {0};
                    uint8_t sensor_comp = BMP3_PRESS_TEMP;
                    
                                                                                /* 读气压+温度原始值，原厂库内部会用出厂校准系数做二阶补偿并返回浮点结果。 BMP3_PRESS_TEMP 表示“两样都要” */
                    rslt = bmp3_get_sensor_data(sensor_comp, &data, &dev);
                    if(rslt == BMP3_OK) {
                        baroPressure = data.pressure;      // 单位：Pa
                                                                                                /* 写入全局量：imu/altitude.c 用 baroPressure 算高度， baroTemperature 用于温度补偿 */
                        baroTemperature = data.temperature; // 单位：°C
                        getPressureAndTempeture = true;                                                     /* 告诉调用者“这次有新数据” */
                    }
                    
                                        // 准备下一次测量
                                                                                /* 回到状态 1 并等 5 个计数再触发下一次，所以测量周期 ≈ 25+5 ms（约 30~40Hz）， 对定高足够（高度变化比姿态慢得多） */
                    baroState = BARO_FORCED_TRIGGER_MEASURE;
                    baroUpdateCyc = 5;  // 两次测量间隔10ms
                }
                break;
                
            default:
                baroState = BARO_FORCED_TRIGGER_MEASURE;
                baroUpdateCyc = 1;
                break;
        }			
    }	
    
    return getPressureAndTempeture;
}

