// ch22_ex08.cpp —— 例 22-8：static_assert——编译期断言
// assert 在运行期检查，程序跑到那一行才知道错；
// static_assert 在编译期检查，编不过就是编不过，一个字节的机器码都不会生成。
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

// 1) 用来把"环境假设"写成白纸黑字
static_assert(sizeof(void*) == 8, "这个例子假设是 64 位目标");
static_assert(sizeof(std::uint32_t) == 4, "uint32_t 必须是 4 字节");
static_assert(alignof(double) == 8, "double 需要 8 字节对齐");

// 2) C++17 起可以不写消息；报错时会直接显示表达式原文
static_assert(sizeof(char) == 1);

// 3) 最有用的场景：锁住结构体内存布局
struct Packet {
    std::uint8_t id;
    std::uint16_t len;
    std::uint32_t payload;
};
static_assert(sizeof(Packet) == 8, "Packet 布局变了，协议会不兼容");
static_assert(offsetof(Packet, payload) == 4, "payload 的偏移必须是 4");

// 4) 也可以断言"某个类型该不该有虚函数"，防止接口被偷偷加东西
struct Plain {
    int a;
    int b;
};
static_assert(sizeof(Plain) == 8, "Plain 里混进了别的东西");

int main() {
    std::printf("sizeof(void*)   = %zu\n", sizeof(void*));
    std::printf("sizeof(Packet)  = %zu\n", sizeof(Packet));
    std::printf("offsetof(Packet,payload) = %zu\n", offsetof(Packet, payload));

        // assert 是运行期的，而且可以被 NDEBUG 关掉
    int a = 1;
    assert(a == 1);
    std::printf("assert(a == 1) 通过，程序继续\n");
    std::printf("关键区别：static_assert 关不掉、不占运行时间；assert 能被 NDEBUG 去掉\n");
    return 0;
}
