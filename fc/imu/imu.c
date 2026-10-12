#include "bsp.h"



// 传感器帧相对于地球帧的四元数（初始状态为无旋转）
float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;             // quaternion of sensor frame relative to earth frame  传感器帧相对于地球帧的四元数
static float rMat[3][3];		// 旋转矩阵

int16_t angle[XYZ_AXIS_COUNT];                               // 欧拉角（单位：度*100，提高精度）



int32_t accSmooth[XYZ_AXIS_COUNT] = {0};			// 平滑后的加速度数据
static imuRuntimeConfig_t imuConfig;					// IMU运行时配置
static imuRuntimeConfig_t *imuRuntimeConfig = &imuConfig;			 // 指向配置的指针

static double Gyro_fill[3][10];
static double Gyro_total[3];
static double sqrGyro_total[3];
static int GyroinitFlag = 0;
static int GyroCount = 0;
// 方差变形公式 S^2 = (X1^2 + X2^2 + X3^2 + ... +Xn^2)/n - X平均^2
// 函数名称: calVariance
// 功能描述: 计算方差
// 输入参数: data[] --- 参与计算方差的数据缓冲区
// length --- 数据长度
// 输出参数:
// sqrResult[] --- 方差结果
// avgResult[] --- 平均数

void calGyroVariance(float data[], int length, float sqrResult[], float avgResult[])
{
		int i;
		double tmplen;
		if (GyroinitFlag == 0)
		{
			for (i = 0; i< 3; i++)
			{
				Gyro_fill[i][GyroCount] = data[i];
				Gyro_total[i] += data[i];
				sqrGyro_total[i] += data[i] * data[i];
				sqrResult[i] = 100;
				avgResult[i] = 0;
			}
		}
		else
		{
			for (i = 0; i< 3; i++)
			{
				Gyro_total[i] -= Gyro_fill[i][GyroCount];
				sqrGyro_total[i] -= Gyro_fill[i][GyroCount] * Gyro_fill[i][GyroCount];
				Gyro_fill[i][GyroCount] = data[i];
				Gyro_total[i] += Gyro_fill[i][GyroCount];
				sqrGyro_total[i] += Gyro_fill[i][GyroCount] * Gyro_fill[i][GyroCount];
			}
		}
		GyroCount++;
		if (GyroCount >= length)
		{
			GyroCount = 0;
			GyroinitFlag = 1;
		}
		if (GyroinitFlag == 0)
		{
			return;
		}
		tmplen = length;
		for (i = 0; i< 3; i++)
		{
			avgResult[i] = (float)(Gyro_total[i] / tmplen);
			sqrResult[i] = (float)((sqrGyro_total[i] - Gyro_total[i] * Gyro_total[i] / tmplen));
		}
}

// ------------------------------------------------------------------------------------------------
// 计算旋转矩阵
// 参数: _rMat - 输出的3x3旋转矩阵, q - 输入的四元数
// ------------------------------------------------------------------------------------------------
void imuComputeRotationMatrix(float (*_rMat)[3], float* q)
{
				// 计算四元数分量的平方
    float q1q1 = SQ(q[1]);
    float q2q2 = SQ(q[2]);
    float q3q3 = SQ(q[3]);

				// 计算四元数分量间的乘积
    float q0q1 = q[0] * q[1];
    float q0q2 = q[0] * q[2];
    float q0q3 = q[0] * q[3];
    float q1q2 = q[1] * q[2];
    float q1q3 = q[1] * q[3];
    float q2q3 = q[2] * q[3];

				// 根据四元数计算旋转矩阵的元素
    _rMat[0][0] = 1.0f - 2.0f * q2q2 - 2.0f * q3q3;
    _rMat[0][1] = 2.0f * (q1q2 + -q0q3);
    _rMat[0][2] = 2.0f * (q1q3 - -q0q2);

    _rMat[1][0] = 2.0f * (q1q2 - -q0q3);
    _rMat[1][1] = 1.0f - 2.0f * q1q1 - 2.0f * q3q3;
    _rMat[1][2] = 2.0f * (q2q3 + -q0q1);

    _rMat[2][0] = 2.0f * (q1q3 + -q0q2);
    _rMat[2][1] = 2.0f * (q2q3 - -q0q1);
    _rMat[2][2] = 1.0f - 2.0f * q1q1 - 2.0f * q2q2;          
}
// ------------------------------------------------------------------------------------------------
// 将向量从机体坐标系转换到地球坐标系
// 参数: v - 要转换的向量指针, _rMat - 旋转矩阵
// ------------------------------------------------------------------------------------------------
void imuTransformVectorBodyToEarth(t_fp_vector * v, float (*_rMat)[3])
{
    float x,y,z;

        // 使用旋转矩阵进行坐标变换
    x = _rMat[0][0] * v->V.X + _rMat[0][1] * v->V.Y + _rMat[0][2] * v->V.Z;
    y = _rMat[1][0] * v->V.X + _rMat[1][1] * v->V.Y + _rMat[1][2] * v->V.Z;
    z = _rMat[2][0] * v->V.X + _rMat[2][1] * v->V.Y + _rMat[2][2] * v->V.Z;

				// 更新向量值
    v->V.X = x;
    v->V.Y = y;//-y;
    v->V.Z = z;
}

// ------------------------------------------------------------------------------------------------
// 从欧拉角创建四元数
// 参数: roll - 滚转角, pitch - 俯仰角, yaw - 偏航角, q - 输出的四元数
// ------------------------------------------------------------------------------------------------
void from_euler(float roll, float pitch, float yaw, float* q)
{
				// 计算半角的正余弦值
    float cr2 = cosf(roll*0.5f);
    float cp2 = cosf(pitch*0.5f);
    float cy2 = cosf(yaw*0.5f);
    float sr2 = sinf(roll*0.5f);
    float sp2 = sinf(pitch*0.5f);
    float sy2 = sinf(yaw*0.5f);

				// 计算四元数的各个分量
    q[0] = cr2*cp2*cy2 + sr2*sp2*sy2;
    q[1] = sr2*cp2*cy2 - cr2*sp2*sy2;
    q[2] = cr2*sp2*cy2 + sr2*cp2*sy2;
    q[3] = cr2*cp2*sy2 - sr2*sp2*cy2;
}
// 反正弦近似函数
// 使用多项式近似实现 asin(x)，x 在 [-1, 1] 范围内
// 最大绝对误差：约 0.000027 度
float asin_approx(float x)
{
        // 确保输入在 [-1, 1] 范围内
    if (x > 1.0f) x = 1.0f;
    if (x < -1.0f) x = -1.0f;
    
        // 使用公式：asin(x) = π/2 - acos(x)
    return M_PI/2.0f - acos_approx(x);
}

// ------------------------------------------------------------------------------------------------
// 从四元数创建欧拉角
// 参数: roll, pitch, yaw - 输出的欧拉角（弧度）
// ------------------------------------------------------------------------------------------------
void to_euler(float *roll, float *pitch, float *yaw)
{
    if (roll) {
        *roll = (atan2f(2.0f*(q0*q1 + q2*q3), 1 - 2.0f*(q1*q1 + q2*q2)));
    }
// if (pitch) {
// // we let safe_asin() handle the singularities near 90/-90 in pitch
// // 使用近似正弦函数处理俯仰角在90/-90度附近的奇点
// *pitch = sin_approx(2.0f*(q0*q2 - q3*q1));
// }
		if (pitch) {
                // 计算 sinp 并约束在 [-1, 1] 范围内
        float sinp = 2.0f*(q0*q2 - q3*q1);
        
// // 防止数值溢出
// if (sinp > 1.0f) sinp = 1.0f;
// else if (sinp < -1.0f) sinp = -1.0f;
        
                // 使用 asin_approx 计算反正弦
        *pitch = asin_approx(sinp);
    }
    if (yaw) {
        *yaw = atan2f(2.0f*(q0*q3 + q1*q2), 1 - 2.0f*(q2*q2 + q3*q3));
    }
}

// ------------------------------------------------------------------------------------------------
// 计算地球坐标系下的加速度（移除重力影响）
// 参数: deltaT - 时间间隔（秒）
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// rotate acc into Earth frame and calculate acceleration in it 将acc旋转到地球框架中并计算其中的加速度
void imuCalculateAcceleration(float deltaT)
{			
    t_fp_vector accel_ned;
		float q[4], _rMat[3][3];
    float angleRad[3];			// 存储滚转和俯仰角（弧度）
		
				// 获取当前姿态角（滚转和俯仰）
    to_euler(&angleRad[0], &angleRad[1], 0);
				// 使用滚转和俯仰重建四元数（忽略偏航）
    from_euler(angleRad[0], angleRad[1], 0, q);
				// 计算旋转矩阵
    imuComputeRotationMatrix(_rMat, q);
				
				// 加载加速度数据（调整轴顺序）90度
    accel_ned.V.X = accgyro.accData[1]; 
    accel_ned.V.Y = -accgyro.accData[0]; 
    accel_ned.V.Z = accgyro.accData[2];
	
// accel_ned.V.X = accgyro.accData[0];
// accel_ned.V.Y = accgyro.accData[1];
// accel_ned.V.Z = accgyro.accData[2];
		
				// 将加速度从机体坐标系转换到地球坐标系（忽略偏航）
    imuTransformVectorBodyToEarth(&accel_ned, _rMat);                 // 机体加速度转换成地球加速度(已经去除航向角影响)
		
				// 移除重力影响（假设地球坐标系Z轴重力为acc_1G）
	  accel_ned.V.X -= 0;
	  accel_ned.V.Y -= 0;
    accel_ned.V.Z -= acc_1G;	
  
// printf("%d, %d, %d\n", (int)accel_ned.V.Z, angle[0]/100, angle[1]/100);
		
        // apply Deadband to reduce integration drift and vibration influence
				// 应用死区并累加速度值（减少漂移和振动影响）
    accSumZ[0] += applyDeadband((int32_t)(accel_ned.V.X), 0);
    accSumZ[1] += applyDeadband((int32_t)(accel_ned.V.Y), 0); 
    accSumZ[2] += applyDeadband((int32_t)(accel_ned.V.Z), 0); 		

        // sum up Values for later integration to get velocity and distance
				// 更新积分时间和计数器
	  accSumZTimer += deltaT;			
    accSumZCount++;
}
// ------------------------------------------------------------------------------------------------
// 检查是否使用快速增益（上电后2秒内）
// ------------------------------------------------------------------------------------------------
static bool imuUseFastGains(void)
{
  if(powerupTimer >= 100) { // 2s
    powerupTimer = 100;
  } 
  return (powerupTimer < 100);
}
// ------------------------------------------------------------------------------------------------
// 获取比例增益缩放因子
// ------------------------------------------------------------------------------------------------
static float imuGetPGainScaleFactor(void)
{
    if (imuUseFastGains()) {
        return 10.0f;
		} else {
        return 1.0f;
    }
}
// ------------------------------------------------------------------------------------------------
// Mahony AHRS 姿态更新算法
// 参数: dt - 时间间隔, gx/gy/gz - 陀螺仪数据, useAcc - 是否使用加速度计, ax/ay/az - 加速度计数据
// ------------------------------------------------------------------------------------------------
static void imuMahonyAHRSupdate(float dt, float gx, float gy, float gz,
                                bool useAcc, float ax, float ay, float az)
{
    static float integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;    // integral error terms scaled by Ki// 积分误差项
    float recipNorm;
    float ex = 0, ey = 0, ez = 0;			 // 误差项
    float qa, qb, qc;
	  float dcmKpGain;										// 比例增益
	  float q[4];													// 临时存储四元数

        // Calculate general spin rate (rad/s)
				// 计算总角速度（rad/s）
    float spin_rate = sqrtf(SQ(gx) + SQ(gy) + SQ(gz));

        // Use measured acceleration vector
				// 如果使用加速度计且数据有效
    recipNorm = SQ(ax) + SQ(ay) + SQ(az);
    if (useAcc && recipNorm > 0.01f) {
                // Normalise accelerometer measurement
								// 归一化加速度向量
        recipNorm = invSqrt(recipNorm);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

                // Error is sum of cross product between estimated direction and measured direction of gravity
								// 计算重力方向的误差（叉积）
        ex += (ay * rMat[2][2] - az * rMat[2][1]);
        ey += (az * rMat[2][0] - ax * rMat[2][2]);
        ez += (ax * rMat[2][1] - ay * rMat[2][0]);
    }

        // Compute and apply integral feedback if enabled
				// 应用积分反馈（如果使能）
    if(imuRuntimeConfig->dcm_ki > 0.0f) {
                // Stop integrating if spinning beyond the certain limit
								// 超过角速度限制时停止积分
        if (spin_rate < DEGREES_TO_RADIANS(SPIN_RATE_LIMIT)) {
            float dcmKiGain = imuRuntimeConfig->dcm_ki;
            integralFBx += dcmKiGain * ex * dt;    // integral error scaled by Ki
            integralFBy += dcmKiGain * ey * dt;
            integralFBz += dcmKiGain * ez * dt;
        }
    }
    else {// 防止积分饱和
        integralFBx = 0.0f;    // prevent integral windup
        integralFBy = 0.0f;
        integralFBz = 0.0f;
    }

        // Calculate kP gain. If we are acquiring initial attitude (not armed and within 20 sec from powerup) scale the kP to converge faster
        // 计算比例增益（上电初期增益更高）
		dcmKpGain = imuRuntimeConfig->dcm_kp * imuGetPGainScaleFactor();

        // Apply proportional and integral feedback
				// 应用比例和积分反馈到陀螺仪数据
    gx += dcmKpGain * ex + integralFBx;
    gy += dcmKpGain * ey + integralFBy;
    gz += dcmKpGain * ez + integralFBz;

        // Integrate rate of change of quaternion
				// 积分四元数变化率
    gx *= (0.5f * dt);
    gy *= (0.5f * dt);
    gz *= (0.5f * dt);
				
    qa = q0;
    qb = q1;
    qc = q2;
				// 四元数积分步骤
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

// printf("q0: %f q1: %f q2: %f q3: %f\r\n",q0,q1,q2,q3);
        // Normalise quaternion
				// 四元数归一化
    recipNorm = invSqrt(SQ(q0) + SQ(q1) + SQ(q2) + SQ(q3));
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;

        // Pre-compute rotation matrix from quaternion
				// 更新旋转矩阵
		q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;
    imuComputeRotationMatrix(rMat, q);                            
}

// ------------------------------------------------------------------------------------------------
// 检查加速度计数据是否有效（0.9g-1.1g范围）
// ------------------------------------------------------------------------------------------------
static bool imuIsAccelerometerHealthy(void)
{
    int32_t axis;
    int32_t accMagnitude = 0;
				// 计算加速度向量模长的平方
    for (axis = 0; axis < 3; axis++) {
        accMagnitude += (int32_t)accSmooth[axis] * accSmooth[axis];
    }
				// 转换为重力单位百分比
    accMagnitude = accMagnitude * 100 / (SQ((int32_t)acc_1G));

        // Accept accel readings only in range 0.90g - 1.10g
				// 检查是否在有效范围内
    return (bool)((81 < accMagnitude) && (accMagnitude < 121));
}
// ------------------------------------------------------------------------------------------------
// 更新欧拉角（从旋转矩阵计算）
// ------------------------------------------------------------------------------------------------
void imuUpdateEulerAngles(void)
{		
				// 计算滚转、俯仰、偏航角（转换为度*100）
    angle[ROLL]  = (int32_t)(atan2_approx(rMat[2][1], rMat[2][2]) * (18000.0f / M_PI));
    angle[PITCH] = (int32_t)(((0.5f * M_PI) - acos_approx(-rMat[2][0])) * (18000.0f / M_PI));
    angle[YAW]   = (int32_t)((-atan2_approx(rMat[1][0], rMat[0][0]) * (18000.0f / M_PI)));		


		
				// 当滚转角接近±90°时调整俯仰角表示
				// 输出-180 ~ +180
    if(mlib_abs(angle[ROLL]) >= 9000){
  	  if(angle[PITCH] > 0) angle[PITCH] = 18000 - angle[PITCH];
		  else angle[PITCH] = -18000 - angle[PITCH];		
    }				
// printf("ROLL:%f  PITCH:%f  YAW:%f  \r\n",angle[ROLL]/100.0,angle[PITCH]/100.0,angle[YAW]/100.0);
}
// ------------------------------------------------------------------------------------------------
// 主姿态解算函数（每2ms调用）
// ------------------------------------------------------------------------------------------------
void imuCalculateEstimatedAttitude(void)
{
    static uint32_t previousIMUUpdateTime;		// 上次更新时间
    uint32_t currentTime;			
    float deltaT;															// 时间间隔（秒）
  
    bool useAcc = FALSE;											// 是否使用加速度计标志
	  int32_t gyroData[3];											// 存储陀螺仪数据
	  uint8_t axis;

        // -------------------------------------------------------------------------------------------------
				// 计算时间差
	  currentTime = micros16();
    deltaT = (uint16_t)(currentTime-previousIMUUpdateTime)*1e-6;
    previousIMUUpdateTime = currentTime;  
// currentTime = micros16();
// if(currentTime<previousIMUUpdateTime)		//超时
// {
// //			printf("currentTime:%d  previousIMUUpdateTime:%d\r\n",currentTime,previousIMUUpdateTime);
// deltaT = (uint16_t)(currentTime+(0xffff-previousIMUUpdateTime))*1e-6;
// }
// else
// {
// deltaT = (uint16_t)(currentTime-previousIMUUpdateTime)*1e-6;
// }
// previousIMUUpdateTime = currentTime;
// printf("deltaT:%f\r\n",deltaT);
				// 跳过过大的时间间隔（>0.2秒）
	  if (deltaT > 0.2f) {
				printf("超时");
        return;
    }
		
				// 平滑加速度数据并获取陀螺仪数据
    for(axis=0; axis<3; axis++) {
      accSmooth[axis] = (accSmooth[axis]*7 + accgyro.accData[axis]) / 8;
// accSmooth[axis]= accgyro.accData[axis];
      gyroData[axis] = accgyro.gyroData[axis];      
    }    
				// 检查加速度计数据是否有效
    if (imuIsAccelerometerHealthy()) {
        useAcc = TRUE;
    }
// printf("ROLLacc:%d  PITCHacc:%d  YAWacc:%d  \r\n",accSmooth[0],accSmooth[1],accSmooth[2]);
// printf("ROLLgyro:%d  PITCHgyro:%d  YAWgyro:%d  \r\n",gyroData[0],gyroData[1],gyroData[2]);
        // -------------------------------------------------------------------------------------------------
				// 1) 使用Mahony算法更新四元数
    imuMahonyAHRSupdate(deltaT,
                        gyroData[0] * gyroScale, gyroData[1] * gyroScale, gyroData[2] * gyroScale,
                        useAcc, accSmooth[0], accSmooth[1], accSmooth[2]);
// printf("q0: %f q1: %f q2: %f q3: %f\r\n",q0,q1,q2,q3);
        // 2) 从旋转矩阵更新欧拉角
    imuUpdateEulerAngles();		             

				// 3) 由转移矩阵计算出机身各轴加速度， 计算地球坐标系下的加速度（移除重力）
    imuCalculateAcceleration(deltaT); 
        
}
// ------------------------------------------------------------------------------------------------
// IMU初始化
// ------------------------------------------------------------------------------------------------
void imu_init(void)
{  
	  float q[4];			// 临时存储四元数
	
				// 设置姿态控制参数
	  imuRuntimeConfig->dcm_kp = 2500.0f / 10000.0f;
	  imuRuntimeConfig->dcm_ki = 25.0f / 10000.0f;
		
				// 初始化四元数和旋转矩阵
		q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;
    imuComputeRotationMatrix(rMat, q);
  
}
// //------------------------------------------------------------------------------------------------
// // IMU测试函数（主循环调用）
// //------------------------------------------------------------------------------------------------
// void imu_test(void)
// {
// //  static uint8_t printfcnt;

// float deltaT;
// static uint16_t previousTime;
// uint16_t currentTime = micros16();

// deltaT = (uint16_t)(currentTime-previousTime)*1e-6;

// // 2ms
// if(deltaT >= 0.002f) {
// previousTime = currentTime;

// accgyro_update();
// imuCalculateEstimatedAttitude();

// //    if(printfcnt++ >= 5) {
// //      printfcnt = 0;
// //      printf("%02f,%02f,%02f\n", (float)angle[ROLL]/100.0f, (float)angle[PITCH]/100.0f, (float)angle[YAW]/100.0f);
// //    }
// }

// }
