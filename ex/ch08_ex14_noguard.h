/**
 * ch08_ex14_noguard.h —— 故意"不写头文件卫士"的错误示范
 */
struct ImuSample {
    int gyro_x;
    int gyro_y;
};

int imu_read_raw(void);
