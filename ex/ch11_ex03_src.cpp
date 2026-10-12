// ch11_ex03_src.cpp —— 实现文件：把声明对应的定义写在这里
#include "ch11_ex03_nav.h"     // 先包含自己的头文件，保证声明与定义一致
#include <cmath>
#include <cstdio>

namespace navx {

ImuData read_imu(int step)
{
        // 用正弦波造假数据，保证结果可复现
    double t = step * 0.01;
    return ImuData{ 0.0, 0.30 * step, 9.8,               // ay 慢慢变大
                    0.0, 12.0 * std::sin(t), 5.0 * step };  // gz 也在动
}

double fuse_angle(const ImuData& d)
{
        // 样子货互补滤波：只用加速度算倾角（单位：度）
    return std::atan2(d.ay, d.az) * 180.0 / 3.14159265358979323846;
}

void print_imu(const ImuData& d)
{
    std::printf("  acc=(%.2f, %.2f, %.2f) gyro=(%.2f, %.2f, %.2f)\n",
                d.ax, d.ay, d.az, d.gx, d.gy, d.gz);
}

} // namespace navx
