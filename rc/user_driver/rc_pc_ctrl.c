/**
 * 文件：user_driver/rc_pc_ctrl.c
 */

#include "bsp_uart.h"
#include "py32f0xx_hal.h"
#include <string.h>

/**
 * 只有启用时才编译（宏在 .h 里定义，但 .h 用了 #if 包住声明， 所以这里单独再判断一次，保证关掉时整个 .c 是空的）
 */
#include "rc_pc_ctrl.h"

#if RC_PC_CTRL_ENABLE

#include "main.h"        /**
 * 拿 hardware_value[] 的 extern 声明
 */
#include "deal_data.h"

/* ---- 协议常量（必须与电脑侧 pc_keycontrol.py 一致）---- */
#define PC_SYNC          0x5B
#define PC_FRAME_LEN     12
#define PC_FLAG_ENGAGE   0x01     /* bit0：电脑接管 */
#define PC_FLAG_ESTOP    0x02     /* bit1：急停 */

/* 摇杆中位与量程（与飞控约定一致） */
#define PC_MID           1500
#define PC_MIN           1000
#define PC_MAX           2000

/* ---- 接收状态 ---- */
static uint8_t  s_buf[PC_FRAME_LEN];
static uint8_t  s_idx;                 /* 已经收了多少字节 */
static uint8_t  s_last_seq;
static uint8_t  s_has_seq;             /* 是否见过至少一帧（用来查丢帧） */

/* 解析出来的指令 */
static uint16_t s_roll, s_pitch, s_thr, s_yaw;
static uint8_t  s_engage;              /* 帧里 bit0 */
static uint8_t  s_estop;               /* 帧里 bit1 */
static uint32_t s_last_ok_ms;          /* 最近一次收到合法帧的时刻 */
static uint8_t  s_active;              /* 目前是否真的在接管 */

/* 诊断计数（可用调试器看，判断链路是否有问题） */
volatile uint32_t rc_pc_frames_ok;
volatile uint32_t rc_pc_frames_bad;
volatile uint32_t rc_pc_timeouts;

/* ---- CRC-8，多项式 0x07（与电脑侧一致）---- */
static uint8_t pc_crc8(const uint8_t *p, uint8_t len)
{
	uint8_t crc = 0x00;
	while (len--) {
		crc ^= *p++;
		for (uint8_t i = 0; i < 8; i++) {
			crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
		}
	}
	return crc;
}

static uint16_t clamp_stick(uint16_t v)
{
	if (v < PC_MIN) return PC_MIN;
	if (v > PC_MAX) return PC_MAX;
	return v;
}

/* 初始化：把 USART2 改成双向，并配置 RX 引脚 为什么要自己配： 原来的 bsp_uart.c 里 Init.Mode = UART_MODE_TX（只发）， 并且 RX 引脚的配置整段被注释掉了（见 bsp_uart.c 的 58~63 行）。 */
void rc_pc_ctrl_init(void)
{
	GPIO_InitTypeDef gpio = {0};

				/* ---- 1) 配 RX 引脚 ---- PB2 能否复用成 USART2_RX，请查 PY32F030 datasheet 的引脚复用表确认。 */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	gpio.Pin       = GPIO_PIN_2;
	gpio.Mode      = GPIO_MODE_AF_PP;
	gpio.Pull      = GPIO_PULLUP;
	gpio.Speed     = GPIO_SPEED_FREQ_HIGH;
	gpio.Alternate = GPIO_AF4_USART2;
	HAL_GPIO_Init(GPIOB, &gpio);

		/* ---- 2) 把 USART2 改成收发双向 ---- HAL 的 UART_MODE 可以后面直接改，再调 HAL_UART_Init() 生效。 注意：重新 Init 会复位串口状态，所以这里必须在 bsp_uart_init() 之后调用。 */
	sUartxHandle.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&sUartxHandle);

				/* ---- 3) 状态清零 ---- */
	s_idx = 0;
	s_has_seq = 0;
	s_engage = 0;
	s_estop = 0;
	s_active = 0;
	s_roll = s_pitch = s_thr = s_yaw = PC_MID;
	s_last_ok_ms = 0;
	rc_pc_frames_ok = rc_pc_frames_bad = rc_pc_timeouts = 0;
}

/* 收一帧（非阻塞）。放在主循环里每圈调用。 */
void rc_pc_ctrl_poll(void)
{
	uint8_t b;

				/* ---- 1) 尽量把串口缓冲里的字节都吃掉 ---- 一次最多读 24 个字节（= 两帧），避免极端情况下卡在循环里太久 */
	for (uint8_t n = 0; n < 24; n++) {
		if (HAL_UART_Receive(&sUartxHandle, &b, 1, 0) != HAL_OK) {
			break;              						/* 超时=没有更多数据，正常退出 */
		}

		if (s_idx == 0) {
												/* 等同步字节；不是就丢掉 */
			if (b != PC_SYNC) continue;
			s_buf[s_idx++] = b;
		} else {
			s_buf[s_idx++] = b;
			if (s_idx < PC_FRAME_LEN) continue;

												/* 收满一帧 → 校验 */
			s_idx = 0;
			if (pc_crc8(s_buf, PC_FRAME_LEN - 1) != s_buf[PC_FRAME_LEN - 1]) {
																/* CRC 不过：这多半是**假同步**。 关键点：真帧的起点可能就在这 12 字节中间（数据里出现 0x5B 很正常）， 所以不能整帧丢弃 —— 要从第 2 个字节起重新找同步头。 */
									uint8_t hit = 0;
					for (uint8_t k = 1; k < PC_FRAME_LEN; k++) {
						if (s_buf[k] == PC_SYNC) {
							for (uint8_t m = k; m < PC_FRAME_LEN; m++) s_buf[m - k] = s_buf[m];
							s_idx = (uint8_t)(PC_FRAME_LEN - k);
							hit = 1;
							break;
						}
					}
					if (!hit) s_idx = 0;
				rc_pc_frames_bad++;
				continue;
			}

												/* 丢帧统计（帧序号应逐 1 递增） */
			if (s_has_seq) {
				uint8_t gap = (uint8_t)(s_buf[1] - s_last_seq);
				(void)gap;      								/* 需要的话可以在这里累加丢帧计数 */
			}
			s_last_seq = s_buf[1];
			s_has_seq  = 1;
			rc_pc_frames_ok++;

												/* 解析四个轴（大端） */
			s_roll  = clamp_stick((uint16_t)((s_buf[2]  << 8) | s_buf[3]));
			s_pitch = clamp_stick((uint16_t)((s_buf[4]  << 8) | s_buf[5]));
			s_thr   = clamp_stick((uint16_t)((s_buf[6]  << 8) | s_buf[7]));
			s_yaw   = clamp_stick((uint16_t)((s_buf[8]  << 8) | s_buf[9]));
			s_engage = (s_buf[10] & PC_FLAG_ENGAGE) ? 1 : 0;
			s_estop  = (s_buf[10] & PC_FLAG_ESTOP)  ? 1 : 0;

			s_last_ok_ms = HAL_GetTick();
		}
	}

		/* ---- 2) 超时判断：超时就交还物理摇杆 ---- 这一步**放在函数末尾、每圈都跑**，所以无论电脑是拔线还是崩了， 最多 RC_PC_TIMEOUT_MS 之后就恢复人工控制。 */
	if (s_active) {
		if ((HAL_GetTick() - s_last_ok_ms) > RC_PC_TIMEOUT_MS) {
			s_active = 0;
			s_engage = 0;
			rc_pc_timeouts++;
		}
	} else {
								/* 只有"收到过帧"且"帧里要求接管"才真的接管 */
		if (s_engage && (HAL_GetTick() - s_last_ok_ms) <= RC_PC_TIMEOUT_MS) {
			s_active = 1;
		}
	}

				/* ---- 3) 急停：立刻退出接管，摇杆值交给物理摇杆 ---- */
	if (s_estop) {
		s_active = 0;
	}
}

/* 把电脑指令应用到摇杆值（在 ready_channels_data() 末尾调用） */
uint8_t rc_pc_ctrl_apply(void)
{
	if (!s_active) return 0;

				/* 直接覆盖遥控器刚算出来的摇杆值。 之后遥控器自己的 Send_Remote_Data() 会按原样打包发出去 —— 所以飞机侧完全看不出区别，仍按"摇杆"处理。 */
	hardware_value[0] = s_roll;
	hardware_value[1] = s_pitch;
	hardware_value[2] = s_thr;
	hardware_value[3] = s_yaw;
	return 1;
}

uint8_t rc_pc_ctrl_is_active(void)
{
	return s_active;
}

#endif /* RC_PC_CTRL_ENABLE */
