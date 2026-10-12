/**
 * 文件：imu/fc.c                    【角色】飞控主调度：把各模块串成一条控制链
 */

#include "bsp.h"



/**
 * 全局飞行状态实体。全项目唯一一份，其他文件通过 fc.h 的 extern 访问。
 */
fc_ctrl_s fc;



void flight_ctrl(void)
{    
  static uint32_t previousIMUUpdateTime;
  uint32_t currentTime;
  float deltaT;

  currentTime = micros16();
        /* 用 uint16_t 相减处理计数器回绕，再乘 1e-6 把微秒换成秒 */
  deltaT = (uint16_t)(currentTime-previousIMUUpdateTime) * 1e-6;
  if(deltaT >= 0.00125f) {
    previousIMUUpdateTime = currentTime;   
    accgyro_update();				//陀螺仪数据更新
    imuCalculateEstimatedAttitude();			//计算四元数和欧拉角
		
    pid(deltaT);
    motor_pwm_output();
  }  
}

/* @brief  飞行状态初始化（上电时调用一次） 【做什么】把 fc 里所有需要明确初值的标志清零、PID 微调清零。 为什么必须显式清零：全局变量虽然在启动时会被清零（.bss 段）， 但这里显式写一遍能让"哪些标志是启动状态"一目了然， */
void fc_init(void)
{
  fc.flags.uartCmdNew = 0;
  fc.flags.motorArmed = 0;          /* 必须为 0：默认不上锁，防止上电电机就转 */
  fc.flags.motorIdle = 0;
  
  pidconf.angleTrim[0] = 0;
  pidconf.angleTrim[1] = 0;
  pidconf.angleTrim[2] = 0;
}
