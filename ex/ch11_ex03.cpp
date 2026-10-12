// ch11_ex03.cpp —— 多文件工程：编译与链接到底做了什么
// 编译（两个源文件一起）：
// _runmulti.bat ch11_ex03.cpp ch11_ex03_src.cpp
// 若只编译 ch11_ex03.cpp 一个文件，编译器没问题，但链接器会报：
// error LNK2019: 无法解析的外部符号 "double __cdecl navx::fuse_angle(...)"
// 这就是"编译通过 != 链接通过"。
#include "ch11_ex03_nav.h"
#include <cstdio>

int main()
{
    for (int step = 0; step < 3; ++step) {
        navx::ImuData d = navx::read_imu(step);
        std::printf("第 %d 帧：\n", step);
        navx::print_imu(d);
        std::printf("  融合倾角 = %.3f 度\n", navx::fuse_angle(d));
    }
    return 0;
}
