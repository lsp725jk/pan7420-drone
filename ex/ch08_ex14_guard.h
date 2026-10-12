/**
 * ch08_ex14_guard.h —— 经典头文件卫士（include guard / header guard）
 */
#ifndef CH08_EX14_GUARD_H
#define CH08_EX14_GUARD_H

struct ImuSample {
    int gyro_x;
    int gyro_y;
};

int imu_read_raw(void);

#endif /**
 * CH08_EX14_GUARD_H
 */
