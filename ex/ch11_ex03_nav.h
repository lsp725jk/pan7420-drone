// ch11_ex03_nav.h —— 头文件：只有声明 + 头文件保护
#ifndef CH11_EX03_NAV_H
#define CH11_EX03_NAV_H

namespace navx {
    struct ImuData {          // 结构体定义放头文件里（类型信息必须到处可见）
        double ax, ay, az;    // 加速度，单位 m/s^2
        double gx, gy, gz;    // 角速度，单位 deg/s
    };

    ImuData read_imu(int step);            // 读一帧（假数据）
    double  fuse_angle(const ImuData& d);  // 互补滤波（假的，只看数量级）
    void    print_imu(const ImuData& d);
}

#endif // CH11_EX03_NAV_H
