/**
 * ch08_ex14.c —— 头文件卫士实测：同一个头文件被包含两次也不出错
 */
#include <stdio.h>

#include "ch08_ex14_guard.h"     /**
 * 第 1 次包含（有 #ifndef 卫士）
 */
#include "ch08_ex14_guard.h"     /**
 * 第 2 次包含：卫士生效，整个文件被跳过
 */
#include "ch08_ex14_once.h"      /**
 * 第 1 次包含（有 #pragma once）
 */
#include "ch08_ex14_once.h"      /**
 * 第 2 次包含：被跳过
 */
#include "ch08_ex14_guard.h"     /* 再来一次也没关系 */

int imu_read_raw(void)  { return 42; }   /* 头文件里只放声明，定义放 .c */
int baro_read_raw(void) { return 77; }

int main(void)
{
    struct ImuSample  s;
    struct BaroSample b;
    s.gyro_x = 11;
    s.gyro_y = -7;
    b.pressure_pa = 101325;
    b.temperature = 25;
    printf("guard 头文件被包含 3 次、once 头文件被包含 2 次，编译通过\n");
    printf("gyro_x = %d , gyro_y = %d , imu_read_raw()  = %d\n",
           s.gyro_x, s.gyro_y, imu_read_raw());
    printf("pressure_pa = %d , temperature = %d , baro_read_raw() = %d\n",
           b.pressure_pa, b.temperature, baro_read_raw());
    return 0;
}
