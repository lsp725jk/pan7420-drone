/**
 * and other intellectual property rights laws.
 */



#include "bsp.h"
#include "icm42688.h"

#include "helperClockCalib.h"



/**
 * Static and extern variables
 */

/**
 * Just a handy variable to handle the icm426xx object
 */
struct inv_icm426xx icm_driver;



/* Buffer to keep track of the timestamp when icm426xx data ready interrupt fires. */
// extern RINGBUFFER(timestamp_buffer, 64, uint64_t);

/* ICM mounting matrix ICM安装矩阵 Coefficients are coded as Q30 integer */
#if defined(ICM_FAMILY_CPLUS)
static int32_t icm_mounting_matrix[9] = { 0, -(1 << 30), 0, (1 << 30), 0, 0, 0, 0, (1 << 30) };
#else
static int32_t icm_mounting_matrix[9] = { (1 << 30), 0, 0, 0, (1 << 30), 0, 0, 0, (1 << 30) };

#endif

/* static function declaration 静态函数声明 */

static void apply_mounting_matrix(const int32_t matrix[9], int16_t raw[3]);



/* Functions definition */

int SetupInvDevice(struct inv_icm426xx_serif *icm_serif)

{
	int     rc = 0;
	uint8_t who_am_i;

				/* Initialize device 设备初始化 */
	INV_MSG(INV_MSG_LEVEL_INFO, "Initialize Icm426xx");

	rc = inv_icm426xx_init(&icm_driver, icm_serif, HandleInvDeviceDataRegisters);

				/* Disable fifo usage, data will be read from sensors registers禁用fifo使用，数据将从传感器寄存器读取 */
		/* 让芯片不再往 FIFO 里塞数据；之后 inv_icm426xx_get_data_from_registers() 就改成直接读 ACCEL_DATA_X0_UI 这组数据寄存器（本项目走的就是这条路）。 */
	rc |= inv_icm426xx_configure_fifo(&icm_driver, INV_ICM426XX_FIFO_DISABLED);
	
	if (rc != INV_ERROR_SUCCESS) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR : failed to initialize Icm426xx.");
		return rc;
	}

				/* Check WHOAMI */
		/* 【中文补充】WHOAMI 是芯片的身份寄存器，ICM42688 固定返回 0x47。 读它的意义：SPI 是"只写不报错"的总线，配置写错了不会有任何异常， 唯一能确认链路通的办法就是读一个已知值回来比对。 */
	INV_MSG(INV_MSG_LEVEL_INFO, "Check Icm426xx whoami value");

	rc = inv_icm426xx_get_who_am_i(&icm_driver, &who_am_i);
	if (rc != INV_ERROR_SUCCESS) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR : failed to read Icm426xx whoami value.");
		return rc;
	}

	if (who_am_i != ICM_WHOAMI) {
		INV_MSG(INV_MSG_LEVEL_ERROR, "!!! ERROR :  bad WHOAMI value. Got 0x%02x (expected: 0x%02x)",
		        who_am_i, ICM_WHOAMI);
		return INV_ERROR;
	}
	printf("WHOAMI value: 0x%02x\r\n",who_am_i);
		// RINGBUFFER_CLEAR(&timestamp_buffer);

	return rc;
}

int ConfigureInvDevice(uint8_t is_low_noise_mode, ICM426XX_ACCEL_CONFIG0_FS_SEL_t acc_fsr_g,

                       ICM426XX_GYRO_CONFIG0_FS_SEL_t gyr_fsr_dps,
                       ICM426XX_ACCEL_CONFIG0_ODR_t acc_freq, ICM426XX_GYRO_CONFIG0_ODR_t gyr_freq,
                       uint8_t is_rtc_mode)
{
	int rc = 0;

#if defined(ICM42633)
				/* - INV_ICM426XX_SINGLE_INTERFACE - INV_ICM426XX_DUAL_INTERFACE_SPI4 */
	if (is_rtc_mode)
		rc |= inv_icm426xx_interface_change_procedure(&icm_driver, INV_ICM426XX_DUAL_INTERFACE);
#endif

	rc |= inv_icm426xx_enable_clkin_rtc(&icm_driver, is_rtc_mode);

	rc |= inv_icm426xx_set_accel_fsr(&icm_driver, acc_fsr_g);
	rc |= inv_icm426xx_set_gyro_fsr(&icm_driver, gyr_fsr_dps);

	rc |= inv_icm426xx_set_accel_frequency(&icm_driver, acc_freq);
	rc |= inv_icm426xx_set_gyro_frequency(&icm_driver, gyr_freq);

	if (is_low_noise_mode)
		rc |= inv_icm426xx_enable_accel_low_noise_mode(&icm_driver);
	else
		rc |= inv_icm426xx_enable_accel_low_power_mode(&icm_driver);

	rc |= inv_icm426xx_enable_gyro_low_noise_mode(&icm_driver);

				/* Wait Max of ICM426XX_GYR_STARTUP_TIME_US and ICM426XX_ACC_STARTUP_TIME_US */
	(ICM426XX_GYR_STARTUP_TIME_US > ICM426XX_ACC_STARTUP_TIME_US) ?
        inv_icm426xx_sleep_us(ICM426XX_GYR_STARTUP_TIME_US) :
        inv_icm426xx_sleep_us(ICM426XX_ACC_STARTUP_TIME_US);

	return rc;
}

int GetDataFromInvDevice(void)

{
				/* FIFO. */
	return inv_icm426xx_get_data_from_registers(&icm_driver);
}
// 解算结果输出在这个回调函数里

void HandleInvDeviceDataRegisters(inv_icm426xx_sensor_event_t *event)
{
	uint64_t irq_timestamp = 0;

		/* ext_interrupt_cb() in main.c for more details. interrupts disabled to avoid any concurrent access. */

	apply_mounting_matrix(icm_mounting_matrix, event->accel);
	apply_mounting_matrix(icm_mounting_matrix, event->gyro);

				/* Output data on UART link */
	if ((event->accel[0] != INVALID_VALUE_FIFO) && (event->gyro[0] != INVALID_VALUE_FIFO))
		INV_MSG(INV_MSG_LEVEL_INFO, "%u: %d, %d, %d, %d, %d, %d, %d", (uint32_t)irq_timestamp,
		        event->accel[0], event->accel[1], event->accel[2], event->temperature,
		        event->gyro[0], event->gyro[1], event->gyro[2]);
	else if (event->gyro[0] != INVALID_VALUE_FIFO)
		INV_MSG(INV_MSG_LEVEL_INFO, "%u: NA, NA, NA, %d, %d, %d, %d", (uint32_t)irq_timestamp,
		        event->temperature, event->gyro[0], event->gyro[1], event->gyro[2]);
	else if (event->accel[0] != INVALID_VALUE_FIFO)
		INV_MSG(INV_MSG_LEVEL_INFO, "%u: %d, %d, %d, %d, NA, NA, NA", (uint32_t)irq_timestamp,
		        event->accel[0], event->accel[1], event->accel[2], event->temperature);
}

/* Static functions definition */

static void apply_mounting_matrix(const int32_t matrix[9], int16_t raw[3])
{
	unsigned i;

	int64_t  data_q30[3];

	for (i = 0; i < 3; i++) {
		data_q30[i] = ((int64_t)matrix[3 * i + 0] * raw[0]);
		data_q30[i] += ((int64_t)matrix[3 * i + 1] * raw[1]);
		data_q30[i] += ((int64_t)matrix[3 * i + 2] * raw[2]);
	}
	raw[0] = (int16_t)(data_q30[0] >> 30);
	raw[1] = (int16_t)(data_q30[1] >> 30);
	raw[2] = (int16_t)(data_q30[2] >> 30);
}

/* 名    称： bsp_IcmGetRawData 功    能： 读取Icm42688加速度陀螺仪数据 入口参数： 六轴 出口参数： 无 */
uint8_t bsp_IcmGetRawData(float* accData, float* GyroData)
/* @brief  【本项目最核心的传感器读取函数】一次 SPI 突发读，取回 6 轴原始数据 【参数】 @param accData  输出：加速度 3 轴原始值。**注意参数名是 float*， 但函数里实际写入的是 int16 转 float 的整数值**， */
{
	  uint8_t axis;
	
  uint8_t buffer[12] = {0};
	int16_t accRaw[3];
	int16_t gyroRaw[3];

	if(inv_icm426xx_read_reg(&icm_driver, MPUREG_ACCEL_DATA_X0_UI, 12, buffer)==0)		//成功读取
        // icm42688_read_regs(ICM42688_ACCEL_DATA_X1, buffer, 12);
	{
			accRaw[0]  = ((uint16_t)buffer[0] << 8)  | buffer[1];
			accRaw[1]  = ((uint16_t)buffer[2] << 8)  | buffer[3];
			accRaw[2]  = ((uint16_t)buffer[4] << 8)  | buffer[5];
			gyroRaw[0] = ((uint16_t)buffer[6] << 8)  | buffer[7];
			gyroRaw[1] = ((uint16_t)buffer[8] << 8)  | buffer[9];
			gyroRaw[2] = ((uint16_t)buffer[10] << 8) | buffer[11];

// printf("ROLL:%d  PITCH:%d  YAW:%d  \r\n",gyroRaw[0],gyroRaw[1],gyroRaw[2]);
// accData[0]= (float)(accRaw[0] * 8000 / 32768.0f); // 8G
// accData[1] = (float)(accRaw[1] * 8000 / 32768.0f);
// accData[2] = (float)(accRaw[2] * 8000 / 32768.0f);

// GyroData[0] = (float)(gyroRaw[0] * 1000.0f / 32768.0f);  // 2000dps
// GyroData[1] = (float)(gyroRaw[1] * 1000.0f / 32768.0f);
// GyroData[2] = (float)(gyroRaw[2] * 1000.0f / 32768.0f);
// printf("ROLL:%f  PITCH:%f  YAW:%f  \r\n",GyroData[0],GyroData[1],GyroData[2]);
		
			for(axis=0; axis<3; axis++){
				accData[axis] = accRaw[axis];
				GyroData[axis] = gyroRaw[axis];
			}    
			return 1;
	}
	else
	{
    return 0;
	}
}

void check_rc(int rc, const char *msg_context)
/* @brief  统一的错误检查：出错就打印错误码并**死循环停住 【参数】@param rc          驱动函数返回码，>=0 视为正常 @param msg_context 出错时打印的上下文说明，便于定位是哪一步挂了 【算法 / 为什么这样写】 */
{
	if (rc < 0) {
		printf("%s: error %d (%s)\r\n", msg_context, rc, inv_error_str(rc));
		while (1)
			;
	}
}

