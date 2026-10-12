// ch23_ex22.cpp —— 例 23-22：memcpy / memset 的三个经典坑
// memcpy 是 C/C++ 里被用错最多的函数，没有之一。三个坑都"能编过、能跑、结果是错的"。
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

struct Sensor {
    std::uint16_t id;
    float value;
};

int main() {
        // ==================================================================
        // 坑 1：第三个参数是**字节数**，不是元素个数
        // ==================================================================
    int a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int b[8] = {0};

    std::memcpy(b, a, 8);  // 想搬 8 个 int，实际只搬了 8 个字节 = 2 个 int
    std::printf("坑 1: memcpy(b, a, 8) -> b = %d %d %d %d ...（只搬了 2 个）\n", b[0], b[1], b[2],
                b[3]);
    std::memcpy(b, a, sizeof a);  // 正确
    std::printf("      memcpy(b, a, sizeof a) -> b = %d %d %d %d ...\n", b[0], b[1], b[2], b[3]);

        // ==================================================================
        // 坑 2：源和目标重叠 -> 必须用 memmove
        // ==================================================================
    char buf[32] = "0123456789";
    std::memmove(buf + 2, buf, 10);  // 往后挪 2 位，区域重叠
    std::printf("坑 2: memmove 后 = %s\n", buf);
    std::printf("      （这里换成 memcpy 就是 UB：本机也许对，换编译器/换优化等级就错）\n");

        // ==================================================================
        // 坑 3：对"非平凡可复制"的类型用 memcpy -> 内容对了，语义错了
        // ==================================================================
        // 反例：std::string 内部有指针。memcpy 一个 string，
        // 两个对象会指向同一块堆内存，析构时 double free。
        // std::string s1 = "hello";
        // std::string s2;
        // std::memcpy(&s2, &s1, sizeof s2);   // 能编过，运行期崩溃
    
        // 正确做法：用拷贝构造/赋值，让类型自己决定怎么复制。
    std::string s1 = "hello";
    std::string s2 = s1;  // 正确：调用拷贝构造
    std::printf("坑 3: 非平凡类型请用拷贝构造。s2 = %s（和 s1 %s）\n", s2.c_str(),
                s1 == s2 ? "相同" : "不同");

        // 需要 memcpy 的类型：满足 is_trivially_copyable
    Sensor src{7, 3.5f};
    Sensor dst{};
    static_assert(std::is_trivially_copyable<Sensor>::value, "Sensor 可以安全 memcpy");
    std::memcpy(&dst, &src, sizeof dst);
    std::printf("      平凡类型 memcpy 是安全的: id=%u value=%g\n", dst.id, dst.value);

        // ==================================================================
        // memset 的坑：只能填**字节**，而且只对全 0 / 全 0xFF 有意义
        // ==================================================================
    int c[4];
    std::memset(c, 0, sizeof c);  // 正确：全 0
    std::printf("\nmemset(c, 0, sizeof c)  -> c[0] = %d\n", c[0]);

    int d[4];
    std::memset(d, 1, sizeof d);  // 想填"1"，实际每个字节都是 0x01
    std::printf("memset(d, 1, sizeof d)  -> d[0] = 0x%08X = %d（不是 1！）\n", d[0], d[0]);

    float f[4];
    std::memset(f, 0, sizeof f);  // IEEE754 的全 0 正好是 0.0f，安全
    std::printf("memset(f, 0, sizeof f)  -> f[0] = %g（IEEE754 全 0 就是 0.0）\n", (double)f[0]);

    std::printf("\n什么时候可以放心用 memcpy / memset：\n");
    std::printf("  - 类型是 trivially copyable（没虚函数、没自定义拷贝/析构、没裸指针语义）\n");
    std::printf("  - 源和目标不重叠（重叠用 memmove）\n");
    std::printf("  - 长度用 sizeof 算，别手写数字\n");
    std::printf("  - 只想清零 / 置 0xFF\n");
    std::printf("其它情况请用类型自己的构造/赋值/swap —— 编译器会自己选最优实现。\n");

        // 补充：编译器比你更懂什么时候该用 memcpy
    std::vector<int> v1(1000, 7), v2;
    v2 = v1;  // 内部就是一次 memcpy（vector<int> 是平凡可复制的）
    std::printf("\nstd::vector<int> 的拷贝赋值内部就是 memcpy，你不用自己写\n");
    std::printf("sizeof(Sensor)=%zu（可能有填充字节，但仍然可以整体 memcpy）\n", sizeof(Sensor));
    return 0;
}
