// 附录B 探针 7：严格别名（strict aliasing）、重叠 memcpy、printf 格式符不匹配
#include <cstdio>
#include <cstring>
#include <cstdint>

// 严格别名演示：同一个内存既当 float 又当 int 读写
// C++ 规定：通过"与对象实际类型不兼容"的泛左值访问对象是 UB（有个例外是 char/unsigned char）
static int  gAliasBits = 0;
static float gAliasFloat = 0.0f;

float readAsFloat() {
    return *reinterpret_cast<float*>(&gAliasBits);   // 严格别名违规
}

void writeViaInt(float f) {
    *reinterpret_cast<int*>(&gAliasFloat) = (int)f;  // 严格别名违规
}

int main() {
    std::printf("=== 1. 严格别名违规 ===\n");
    gAliasBits = 0x3F800000;                       // 1.0f 的位模式
    std::printf("  用 int* 写、用 float* 读: %f （这次读对了）\n", (double)readAsFloat());
    writeViaInt(2.0f);
    std::printf("  用 int* 写 float 对象: %f\n", (double)gAliasFloat);
    std::puts("  规范做法是用 memcpy 或 std::bit_cast 搬运位模式，不要用指针强转");

    std::printf("=== 2. memcpy 的源和目标重叠 ===\n");
    {
        char buf[16] = "0123456789";
        std::printf("  重叠前: %s\n", buf);
        std::memcpy(buf + 2, buf, 8);              // 区域重叠：UB（应当用 memmove）
        buf[15] = '\0';
        std::printf("  memcpy(buf+2, buf, 8) 之后: %s （结果不保证）\n", buf);
    }
    {
        char buf[16] = "0123456789";
        std::memmove(buf + 2, buf, 8);             // memmove 允许重叠
        buf[15] = '\0';
        std::printf("  memmove 同样参数: %s （正确结果）\n", buf);
    }

    std::printf("=== 3. printf 格式符与实参类型不匹配 ===\n");
    std::printf("  正确: %%d <- 42        = %d\n", 42);
    std::fflush(stdout);
    std::printf("  错误: %%d <- 3.14      = ");
    std::fflush(stdout);
    std::printf("%d\n", 3.14);                     // UB：把 double 当 int 解释
    std::printf("  错误: %%s <- 42        = ");
    std::fflush(stdout);
    std::printf("%s\n", 42);                       // UB：把整数当地址解引用，常直接崩
    std::printf("  错误: %%f <- 42        = %f\n", 42);   // UB：读错栈上的参数
    std::printf("  错误: %%llu <- (int)   = %llu\n", 42); // UB：宽度不匹配

    std::printf("=== 4. 正确的替代写法 ===\n");
    std::printf("  static_cast 后再传: %d\n", static_cast<int>(3.14));
    std::printf("  浮点就用 %%f: %.2f\n", 3.14);
    std::printf("  指针就用 %%p: %p\n", (void*)&gAliasBits);
    return 0;
}
