// ch11_ex21.cpp —— C 风格 enum 的三个毛病
// 编译：check.bat ch11_ex21.cpp c++latest
#include <cstdio>

// C 风格的枚举：枚举名直接跑进外层作用域
enum Color { RED, GREEN, BLUE };
enum State { IDLE, RUNNING, ERROR };      // 只要有一个名字撞车就编不过

// 枚举名可以悄悄变成整数，也可能悄悄和整数混用
int mix(int c) { return c + 1; }

int main()
{
        // 毛病 1：枚举名污染外围作用域，还要担心重名
    std::printf("1) RED=%d GREEN=%d BLUE=%d\n", RED, GREEN, BLUE);
    std::printf("   IDLE=%d RUNNING=%d ERROR=%d\n", IDLE, RUNNING, ERROR);

        // 毛病 2：能隐式转成 int，编译器不拦
    int c = RED;
    std::printf("2) int c = RED; 合法，c=%d（类型信息丢了）\n", c);
    std::printf("   mix(RED)=%d\n", mix(RED));

        // 毛病 3：枚举和整数、以及不同枚举之间都能混着用
    if (RED == 0) std::printf("3) RED == 0 成立（枚举悄悄变成了整数）\n");
    if (GREEN == 1) std::printf("   GREEN == 1 也成立，编译器不会拦\n");

        // 毛病 4：底层类型不确定，跨平台 sizeof 可能不一样
    std::printf("4) sizeof(Color)=%llu\n", (unsigned long long)sizeof(Color));

    return 0;
}
