/**
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * 文件：user/common.c / common.h          【角色】BMP390 气压计驱动的“板级适配层”
 */
#ifdef __cplusplus
extern "C" {
#endif /**
 * __cplusplus
 */
#include "bsp.h"
#include "bmp3.h"



extern bool baroSensorReady;  // 声明BMP390传感器就绪标志

/* 气压计节拍计数器：在 TIM17 的 1ms 中断回调（drivers/bsp/bsp_timer.c）里自增， baro_update() 用它实现“隔一段时间才真的操作一次传感器”的软定时。 之所以不在中断里直接读气压计：SPI 读一次要几十微秒，会拖长中断 */
extern uint8_t baroUpdateTimer;



BMP3_INTF_RET_TYPE bmp3_interface_init(struct bmp3_dev *bmp3, uint8_t intf);



BMP3_INTF_RET_TYPE bmp3_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);



BMP3_INTF_RET_TYPE bmp3_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);



BMP3_INTF_RET_TYPE bmp3_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);



BMP3_INTF_RET_TYPE bmp3_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);



void bmp3_delay_us(uint32_t period, void *intf_ptr);



void bmp3_check_rslt(const char api_name[], int8_t rslt);

/* 配置 BMP390 并进入 FORCED（单次强制）模式；main.c 初始化时调用一次 */
void BMP390_init(void);
/* 气压计三态机：触发测量 → 等数据就绪 → 读数据。返回 true 表示本次真的取到了新数据 */
bool baro_update(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
