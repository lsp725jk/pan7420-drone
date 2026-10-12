#ifndef __RF_TELEMETRY_H
#define __RF_TELEMETRY_H

/**
 * 文件：imu/rf_telemetry.h          【角色】射频遥测（飞机 → 遥控器 → 电脑）
 */

#define RF_TELEMETRY_ENABLE   0     /**
 * ← 改成 1 启用射频遥测
 */

#if RF_TELEMETRY_ENABLE

#include <stdint.h>

/**
 * 遥测帧总长（与 ACK 载荷对齐）
 */
#define RF_TLM_FRAME_LEN      10
/* 放在 temp_buff 中的起始下标（前 2 字节被原有功能占用） */
#define RF_TLM_TEMP_BUFF_OFF  2
/* 每组带的数字个数 */
#define RF_TLM_VALS_PER_GROUP 2
/* 一共几组（组号 0 ~ GROUP_COUNT-1） */
#define RF_TLM_GROUP_COUNT    12

/* 同步字节：电脑侧靠它找帧头 */
#define RF_TLM_SYNC           0x5A

/* @brief 把当前飞行状态打包成一帧遥测，写进 temp_buff[2..11] @param temp_buff 由 deal_pack.c 定义、随 ACK 发回遥控器的 12 字节缓冲 @param frame_id  帧序号（调用方每次 +1；电脑侧用它统计丢包） */
void rf_telemetry_pack(uint8_t *temp_buff, uint8_t frame_id);

#endif /* RF_TELEMETRY_ENABLE */
#endif /* __RF_TELEMETRY_H */
