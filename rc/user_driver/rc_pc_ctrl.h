#ifndef __RC_PC_CTRL_H
#define __RC_PC_CTRL_H

/**
 * 文件：user_driver/rc_pc_ctrl.h
 */

#define RC_PC_CTRL_ENABLE   0     /**
 * ← 改成 1 启用"键盘控飞机"
 */

#if RC_PC_CTRL_ENABLE

#include <stdint.h>

/**
 * 超时：这么久没收到合法帧就把控制权交回物理摇杆
 */
#define RC_PC_TIMEOUT_MS    200

/* @brief 初始化串口接收方向（配置 RX 引脚 + 把 USART2 改成收发双向） 必须在 bsp_uart_init() 之后调用 */
void rc_pc_ctrl_init(void);

/* @brief 收一帧并解析。放在主循环里每圈调用（非阻塞） —— 它同时负责超时判断，超时就取消接管 */
void rc_pc_ctrl_poll(void);

/* @brief 把电脑指令应用到摇杆值。放在 ready_channels_data() 末尾调用 返回 1 表示本次用了电脑指令（接管中），0 表示用物理摇杆 */
uint8_t rc_pc_ctrl_apply(void);

/* @brief 当前是否处于电脑接管状态（供 LED 显示 / 调试用） */
uint8_t rc_pc_ctrl_is_active(void);

#endif /* RC_PC_CTRL_ENABLE */
#endif /* __RC_PC_CTRL_H */
