#ifndef __DECODE_H
#define __DECODE_H

#include "bsp.h"



/**
 * 文件：imu/decode.h                 【角色】遥控数据协议：字段定义与解码接口
 */

/**
 * RC CMD INDEX
 */
/**
 * 一个遥控数据包的总字节数
 */
#define FC_CMD_LENGTH     10

/**
 * ===== 无线包内的字段下标（注意与 fc.h 的 rcCommand 下标不同！）=====
 */
#define FC_CMD_ROLL       0
#define FC_CMD_PITCH      1
#define FC_CMD_THROTTLE   2      /* 包里油门在 2，程序内部是 3 */
#define FC_CMD_YAW        3

/* 四个通道的微调值（遥控器 trim 按键产生的偏移量） */
#define FC_CMD_ROLL_TRIM       4
#define FC_CMD_PITCH_TRIM      5
#define FC_CMD_THROTTLE_TRIM   6
#define FC_CMD_YAW_TRIM        7

/* 两个标志字节：每一位对应一个开关（解锁、模式、翻滚触发等） */
#define FC_CMD_FLAGS_1    8
#define FC_CMD_FLAGS_2    9      



/* 微调量的单字节编码范围：以 128 为中位，可表达 ±63 的偏差。 【为什么用 128 而不是 0 当中位】 单字节无符号编码更容易做校验与比较；居中后 (code-128) 直接就是 有符号偏差，不需要额外处理符号位。 */
#define STICK_TRIM_LIMIT_RANGE             63      /* 最大微调幅度：±63 */
#define STICK_ENCODE_LIMIT_MIDDLE          128     /* 中位编码值 */

extern uint8_t calibrationEnbTimer;   /* 校准倒计时（配合 accgyro.c 校准流程） */
extern float speed_level;			//速度档位（遥控器切换，用于缩放摇杆灵敏度）

/* 解码后的原始命令数组（10 字节包的内容，已去掉包头与校验） */
extern uint16_t fcCmd[FC_CMD_LENGTH];

extern int16_t rcCommand[4], rcCommandTrim[3];
/* 定高模式下的目标速度（由油门摇杆位置映射而来） */
extern int16_t setVelocity;

/* 主解码入口：把一包原始字节翻译成 fcCmd[] / rcCommand[] 等。 由 deal_pack.c 在收到完整且校验通过的包之后调用。 */
void decode(uint8_t* cmd);
/* 处理"辅助通道"逻辑：解锁判定、模式切换、翻滚触发等按键解释。 是"按键 → 飞行状态"的翻译层。 */
void annexCode(void);



#endif
