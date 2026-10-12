/**
 * 文件：imu/rf_telemetry.c          【角色】射频遥测打包（飞机 → 遥控器 → 电脑）
 */

#include "bsp.h"
#include "rf_telemetry.h"

#if RF_TELEMETRY_ENABLE

#include "imu.h"
#include "accgyro.h"
#include "altitude.h"
#include "fc.h"
#include "decode.h"
#include "bsp_pwm.h"

/* ---- 外部依赖（都在各自文件里定义，这里只声明）---- */
extern int16_t  angle[XYZ_AXIS_COUNT];       /* 欧拉角（度×100）  imu/imu.c:8 */
extern int32_t  accSmooth[XYZ_AXIS_COUNT];   /* 平滑加速度        imu/imu.c:11 */

extern accgyro_s accgyro;                    /* 定义在 imu/accgyro.c */
extern int32_t  EstAlt;                      /* 估计高度（毫米）  altitude.c:34 */
extern int32_t  BaroPID;                     /* 定高 PID 输出     altitude.c:40 */
extern uint16_t motorPWMValue[4];            /* 电机占空比 0~1000 bsp_pwm.c:56 */
extern int16_t  rcCommand[4];                /* 内部指令          decode.c:11 */
extern uint16_t fcCmd[FC_CMD_LENGTH];        /* 遥控原始指令      decode.c:10 */
extern float    battery_voltage;             /* 最新电池电压（毫伏）user/adc.c */
extern uint32_t connection_nowtime;          /* 最近有效遥控包时刻 deal_pack.c */

/* 低电压阈值（毫伏），与 user/adc.c 的 LOW_VOLTAGE_THRESHOLD 语义一致 */
#define RF_TLM_LOW_MV     3300u



#define RF_TLM_SHIFT_GYRO 2
#define RF_TLM_SHIFT_ACC  4
#define RF_TLM_SHIFT_ALT  4

/* ---- 限幅与缩放 ---- */

static int16_t sat16(int32_t v)
{
	if (v >  32767) return  32767;
	if (v < -32768) return -32768;
	return (int16_t)v;
}

/* 先把 32 位右移缩放，再饱和到 int16 */
static int16_t sat_shift16(int32_t v, uint8_t rshift)
{
	return sat16(rshift ? (v >> rshift) : v);
}

/* ---- CRC-8，多项式 0x07（电脑侧脚本里的实现与此逐位一致）---- */
static uint8_t rf_crc8(const uint8_t *p, uint8_t len)
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

/* 把 int16 按**大端**写进 2 字节。 为什么选大端：与网络字节序一致，用串口助手肉眼看也更直观 （高位在前）。这是**本模块自己的协议**，与芯片端序无关。 */
static void put_be16(uint8_t *p, int16_t v)
{
	p[0] = (uint8_t)((uint16_t)v >> 8);
	p[1] = (uint8_t)((uint16_t)v & 0xFF);
}

/* ---- 状态标志打包成一个 uint16（位定义见 .h）---- */
static int16_t rf_status_word(void)
{
	uint16_t w = 0;
	if (fc.flags.motorArmed)             w |= (1u << 0);
	if (fc.flags.motorIdle)              w |= (1u << 1);
	if (fc.flags.bTakeoff)               w |= (1u << 2);
	if (fc.flags.bLanding)               w |= (1u << 3);
	if (fc.flags.uartCmdNew)             w |= (1u << 4);
	if (fc.flags.bAccGyroCalibrationEnb) w |= (1u << 5);
				/* bit9 = 失联：与 main.c:470 的判据完全一致（5 秒） */
	if (HAL_GetTick() - connection_nowtime > 5000u) w |= (1u << 9);
				/* bit10 = 低电：与 user/adc.c 的阈值语义一致 */
	if (battery_voltage < (float)RF_TLM_LOW_MV)     w |= (1u << 10);
	return (int16_t)w;
}

/* ---- 取某一组的两个数字 ---- */
static void rf_get_group(uint8_t g, int16_t *a, int16_t *b)
{
	*a = 0; *b = 0;
	switch (g) {
	case 0:   		/* 横滚角、俯仰角（度×100）—— 最常看的放最前面 */
		*a = angle[0];
		*b = angle[1];
		break;
	case 1:   		/* 偏航角（度×100）、电池电压（毫伏） */
		*a = angle[2];
		*b = sat16((int32_t)battery_voltage);
		break;
	case 2:   		/* 估计高度（毫米÷16）、定高 PID 输出 */
		*a = sat_shift16(EstAlt, RF_TLM_SHIFT_ALT);
		*b = sat16(BaroPID);
		break;
	case 3:   		/* 加速度 X、Y（LSB÷16） */
		*a = sat_shift16(accgyro.accData[0], RF_TLM_SHIFT_ACC);
		*b = sat_shift16(accgyro.accData[1], RF_TLM_SHIFT_ACC);
		break;
	case 4:   		/* 加速度 Z、平滑加速度 Z（LSB÷16） */
		*a = sat_shift16(accgyro.accData[2], RF_TLM_SHIFT_ACC);
		*b = sat_shift16(accSmooth[2], RF_TLM_SHIFT_ACC);
		break;
	case 5:   		/* 角速度 X、Y（LSB÷16） */
		*a = sat_shift16(accgyro.gyroData[0], RF_TLM_SHIFT_GYRO);
		*b = sat_shift16(accgyro.gyroData[1], RF_TLM_SHIFT_GYRO);
		break;
	case 6:   		/* 角速度 Z、平滑加速度 X（LSB÷16） */
		*a = sat_shift16(accgyro.gyroData[2], RF_TLM_SHIFT_GYRO);
		*b = sat_shift16(accSmooth[0], RF_TLM_SHIFT_ACC);
		break;
	case 7:   		/* 电机 PWM 0、1（千分比） */
		*a = sat16((int32_t)motorPWMValue[0]);
		*b = sat16((int32_t)motorPWMValue[1]);
		break;
	case 8:   		/* 电机 PWM 2、3（千分比） */
		*a = sat16((int32_t)motorPWMValue[2]);
		*b = sat16((int32_t)motorPWMValue[3]);
		break;
	case 9:   		/* 内部指令：横滚、俯仰（-500~+500） */
		*a = rcCommand[0];
		*b = rcCommand[1];
		break;
	case 10:  		/* 内部指令：油门（1000~2000）、偏航（-500~+500） 注意 rcCommand 的下标顺序见 imu/decode.h（THROTTLE 在 2） */
		*a = rcCommand[2];
		*b = rcCommand[3];
		break;
	case 11:  		/* 状态标志位、遥控原始横滚 */
		*a = rf_status_word();
		*b = sat16((int32_t)fcCmd[0]);
		break;
	default:
		break;
	}
}

/* ---- 对外唯一函数 ---- */
void rf_telemetry_pack(uint8_t *temp_buff, uint8_t frame_id)
{
	static uint8_t group = 0;
	int16_t a, b;
	uint8_t *f;

	if (temp_buff == 0) return;
	f = &temp_buff[RF_TLM_TEMP_BUFF_OFF];

	if (group >= RF_TLM_GROUP_COUNT) group = 0;
	rf_get_group(group, &a, &b);

	f[0] = RF_TLM_SYNC;      		/* 同步字节 */
	f[1] = frame_id;         		/* 帧计数 */
	f[2] = group;            		/* 组号 */
	put_be16(&f[3], a);      		/* 数值 A */
	put_be16(&f[5], b);      		/* 数值 B */
	f[7] = rf_crc8(f, 7);    		/* CRC-8 覆盖 f[0..6] */
	f[8] = 0;                		/* 预留 */
	f[9] = 0;                		/* 预留 */

	group = (uint8_t)((group + 1) % RF_TLM_GROUP_COUNT);
}

#endif /* RF_TELEMETRY_ENABLE */
