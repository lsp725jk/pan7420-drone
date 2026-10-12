#include "bsp.h"

/**
 * Set power mode flag
 */
#define IS_LOW_NOISE_MODE 1



#define USE_CLK_IN 0

/* Private variables --------------------------------------------------------- */
/* Private function prototypes ----------------------------------------------- */
/* Private user code --------------------------------------------------------- */

/* Private function prototypes ----------------------------------------------- */

int self_test_result = 0;

accgyro_s accgyro;// 全局陀螺仪和加速度计数据结构

uint8_t powerupTimer;

static pt1Filter_t gyroFilter[3]; // 三轴陀螺仪PT1滤波器
static biquadFilter_t accFilter[3]; // 三轴加速度计双二阶滤波器

extern Pair_Code_Config pair_data;

int16_t acc_1G=4096;			 // 1G重力加速度对应的原始数据值
float gyroScale=1.0f / 16.4f * (M_PI / 180.0f);    //  // 陀螺仪原始数据转换为弧度/秒的比例因子

void gyro_calbration(accgyro_s* gyro); // 陀螺仪校准函数
void acc_calbration(accgyro_s* acc);	// 加速度计校准函数



// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
void accgyro_init(void)
{
		uint8_t axis;
  
		memset(&accgyro, 0, sizeof(accgyro));		// 陀螺仪数据清零

		accgyro.accgyro_read = bsp_IcmGetRawData;  	

		struct inv_icm426xx_serif icm426xx_serif;
								/* Initialize MCU hardware */
		SetupMCUHardware(&icm426xx_serif);

		int rc = 0;
								/* Initialize Icm426xx */
		rc = SetupInvDevice(&icm426xx_serif);

		check_rc(rc, "error111 while setting up INV device");
		
		rc = ConfigureInvDevice((uint8_t)IS_LOW_NOISE_MODE, ICM426XX_ACCEL_CONFIG0_FS_SEL_8g,
	                        ICM426XX_GYRO_CONFIG0_FS_SEL_2000dps, ICM426XX_ACCEL_CONFIG0_ODR_200_HZ,
	                        ICM426XX_GYRO_CONFIG0_ODR_200_HZ, (uint8_t)USE_CLK_IN);
		check_rc(rc, "error while configuring INV device");
  
				// 初始化滤波器
		for(axis=0; axis<3; axis++) {
			biquadFilterInitLPF(&accFilter[axis], 30, 1250);		// 加速度计：二阶低通滤波器，截止频率30Hz，采样率1250Hz
			pt1FilterInit(&gyroFilter[axis], 240, 1.25e-3);    // 陀螺仪：一阶低通滤波器，截止频率240Hz，时间常数1.25ms
		}

		accgyro.gyroCalibrating = 0;		// 初始化陀螺仪校准
		accgyro.accCalibrating = 0; 		// 初始化加速度计校准
    // 检查是否有有效的校准数据
// if(cfgTemp.good == 0x5a5aa5a5) {
// for(axis=0; axis<3; axis++) {
// accgyro.accRaw_offset[axis] = cfgTemp.acc_offset[axis];
// accgyro.gyroRaw_offset[axis]= cfgTemp.gyro_offset[axis];
// }
// } else {
// cfgTemp.good = 0x5a5aa5a5;
// accgyro.gyroCalibrating = 1;		// 需要陀螺仪校准
// accgyro.accCalibrating = 1; 		// 需要加速度计校准
// }

}

int CalCount = 0;
uint8_t accgyro_update(void)
{
  uint8_t axis;

  if(bsp_IcmGetRawData(accgyro.accRaw, accgyro.gyroRaw))  // 读取原始传感器数据
		{      
			apply_sensor_alignment(accgyro.accRaw, accgyro.accRaw, CW90_DEG);		// 应用传感器安装方向校正（90度顺时针旋转）
			apply_sensor_alignment(accgyro.gyroRaw, accgyro.gyroRaw, CW90_DEG);


			
			if((accgyro.gyroCalibrating == 1) &&(accgyro.accCalibrating == 1))
			{
							Led_SetAlert(LED_PRIORITY_CALIBRATING, LED_STATE_BLINKING, 200);
			}
			else if((accgyro.gyroCalibrating == 0) &&(accgyro.accCalibrating == 0))
			{
							Led_SetAlert(LED_PRIORITY_CALIBRATING, LED_STATE_OFF, 0);
			}
						// 偏差较准，执行传感器校准
			if(accgyro.gyroCalibrating) {
				gyro_calbration(&accgyro);
			}
			if(accgyro.accCalibrating) {
				acc_calbration(&accgyro);
			}
			
        // 原始数据-偏差，应用校准偏移量
    for(axis=0; axis<3; axis++){
      accgyro.accValue[axis] = accgyro.accRaw[axis]  - accgyro.accRaw_offset[axis];
      accgyro.gyroValue[axis]= accgyro.gyroRaw[axis] - accgyro.gyroRaw_offset[axis];
    }
// printf("ROLL:%f  PITCH:%f  YAW:%f  \r\n",accgyro.accRaw[0],accgyro.accRaw[1],accgyro.accRaw[2]);
        // 数据滤波 执行数据滤波处理
    for(axis=0; axis<3; axis++){
      accgyro.accData[axis] = biquadFilterApply(&accFilter[axis], accgyro.accValue[axis]);
      accgyro.gyroData[axis]= pt1FilterApply(&gyroFilter[axis], accgyro.gyroValue[axis]);
    }     
// printf("FilterROLLacc:%d  FilterPITCHacc:%d  FilterYAWacc:%d  \r\n",accgyro.accData[0],accgyro.accData[1],accgyro.accData[2]);
// printf("FilterROLLgyro:%d  FilterPITCHgyro:%d  FilterYAWgyro:%d  \r\n",accgyro.gyroData[0],accgyro.gyroData[1],accgyro.gyroData[2]);
    return TRUE;	// 数据更新成功
  }
  return FALSE;		 // 数据读取失败
}



void apply_sensor_alignment(float * dest, float * src, sensor_align_e rotation)
{
        // Create a copy so we could use the same buffer for src & dest
    const float x = src[X];
    const float y = src[Y];
    const float z = src[Z];

    switch (rotation) {
    default:
    case CW0_DEG:
        dest[X] = x;
        dest[Y] = y;
        dest[Z] = z;
        break;
    case CW90_DEG:
        dest[X] = y;
        dest[Y] = -x;
        dest[Z] = z;
        break;
    case CW180_DEG:
        dest[X] = -x;
        dest[Y] = -y;
        dest[Z] = z;
        break;
    case CW270_DEG:
        dest[X] = -y;
        dest[Y] = x;
        dest[Z] = z;
        break;
    case CW0_DEG_FLIP:
        dest[X] = -x;
        dest[Y] = y;
        dest[Z] = -z;
        break;
    case CW90_DEG_FLIP:
        dest[X] = y;
        dest[Y] = x;
        dest[Z] = -z;
        break;
    case CW180_DEG_FLIP:
        dest[X] = x;
        dest[Y] = -y;
        dest[Z] = -z;
        break;
    case CW270_DEG_FLIP:
        dest[X] = -y;
        dest[Y] = -x;
        dest[Z] = -z;
        break;
    }
}



// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// 函数名：gyro_calbration
// 功能：陀螺仪校准程序（计算零偏）
// 参数：accgyro_s* gyro - 陀螺仪数据结构指针
// 返回值：无
// ------------------------------------------------------------------------------------
void gyro_calbration(accgyro_s* gyro)
{
  static int32_t gyroSum[3];     // 各轴数据累加和
  static int32_t gyroInit[3];    // 初始采样值
  static uint16_t count;         // 采样计数器
  static uint8_t gyroCalInit = 1; // 校准初始化标志
  
  uint8_t axis;
  int32_t dif;
  
    // 开始较准，初始化变量。
  if(gyroCalInit) {
    for(axis=0; axis<3; axis++){
      gyroSum[axis] = 0;
      gyroInit[axis] = gyro->gyroRaw[axis];			// 记录初始值
      count = 0;
    }    
    gyroCalInit = 0;		// 清除初始化标志
  }
  
    // 较准中，如果偏差大于设定值，重新开始。
  #define GYRO_CALI_DIFF_LIMIT    50
  for(axis=0; axis<3; axis++) {
    dif = gyroInit[axis] - gyro->gyroRaw[axis];
    if(mlib_abs(dif) < GYRO_CALI_DIFF_LIMIT) {
      gyroSum[axis] += gyro->gyroRaw[axis];			// 有效采样累加
    } else {
      gyroCalInit = 1;			// 超过阈值则重置校准
      count = 0;
    }
  }
  
    // 较准完成，取均值作为offset。同时置位gyroCalInit，留作下次较准使用。
		// 完成256次采样后计算平均值
  if(++count >= 512) {
    for(axis=0; axis<3; axis++) {
      gyro->gyroRaw_offset[axis] = gyroSum[axis] / count;			// 计算零偏
			
			pair_data.gyroRaw_offset[axis] = gyro->gyroRaw_offset[axis];
					
// cfgTemp.gyro_offset[axis] = gyro->gyroRaw_offset[axis]; // 保存到配置
    }
    gyro->gyroCalibrating = 0;	// 清除校准标志
    gyroCalInit = 1;
		flash_write(FLASH_USER_START_ADDR);//写入flash
// cfg_save_temp();
  }  
}
// ------------------------------------------------------------------------------------
// 函数名：acc_calbration
// 功能：加速度计校准程序（计算零偏和比例因子）
// 参数：accgyro_s* acc - 加速度计数据结构指针
// 返回值：无
// ------------------------------------------------------------------------------------
void acc_calbration(accgyro_s* acc)
{
  
	static int32_t accSum[3];      // 各轴加速度累加和
  static int32_t gyroInit[3];    // 陀螺仪初始值（用于检查静止）
  static uint16_t count;         // 采样计数器
  static uint8_t accCalInit = 1;  // 校准初始化标志
  
  uint8_t axis;
  int32_t dif;
  
    // 开始较准，初始化变量。
  if(accCalInit) {
    for(axis=0; axis<3; axis++){
      accSum[axis] = 0;
      gyroInit[axis] = acc->gyroRaw[axis];		// 记录陀螺仪初始值
      count = 0;
    }    
    accCalInit = 0;
  }
  
    // 较准中，如果陀螺仪偏差大于设定值，重新开始。
  #define ACC_CALI_GYRO_DIFF_LIMIT    50
  for(axis=0; axis<3; axis++) {
    dif = gyroInit[axis] - acc->gyroRaw[axis];
    if(mlib_abs(dif) < ACC_CALI_GYRO_DIFF_LIMIT) {
      accSum[axis] += acc->accRaw[axis];
    } else {
      accCalInit = 1;
      count = 0;
    }
  }
  
    // 较准完成，取均值作为offset。同时置位accCalInit，留作下次较准使用。
  if(++count >= 512) {
    for(axis=0; axis<3; axis++) {
      acc->accRaw_offset[axis] = accSum[axis] / count;
      if(axis == 2) {
        acc->accRaw_offset[axis] -= acc_1G;
      }
			pair_data.accRaw_offset[axis] = acc->accRaw_offset[axis];
// cfgTemp.acc_offset[axis] = acc->accRaw_offset[axis];
    }
    acc->accCalibrating = 0;
    accCalInit = 1;
    powerupTimer = 0;
		flash_write(FLASH_USER_START_ADDR);		//写入flash
// cfg_save_temp();
  }  
}



// void accgyro_test(void)
// {
// static uint8_t printfcnt;

// float deltaT;
// static uint16_t previousTime;
// uint16_t currentTime = micros16();

// deltaT = (uint16_t)(currentTime-previousTime)*1e-6;

// // 2ms
// if(deltaT >= 0.002f) {
// previousTime = currentTime;

// accgyro_update();

// if(printfcnt++ >= 5) {
// printfcnt = 0;
// printf("%f,%d,%f,%d\n", accgyro.gyroRaw[0] , accgyro.gyroData[0], accgyro.accRaw[0] , accgyro.accData[0]);
// }
// }

// }



