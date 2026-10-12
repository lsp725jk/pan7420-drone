/**
 * ch02_ex12_c23.c —— C23 带来的新写法（本机 MSVC 19.51 实测支持情况）
 */
#include <stdio.h>
#include <stdbool.h>            /**
 * C23 起 bool 是关键字，但实测 MSVC 19.51 仍需这个头
 */

/**
 * 想知道自己是不是真的在 C23 模式，就看这个宏
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#define HAVE_C23 1
#else
#define HAVE_C23 0
#endif

/* ---- C23：二进制字面量（0b 开头）和数字分隔符（单引号）---- */
static unsigned int binary_literal_demo(void)
{
        /* 这两个写法 MSVC 在 C17 模式下也接受（当扩展），但标准里是 C23 才有的。 位掩码用二进制写，比 0x5A 直观得多： 0b0101'1010 一眼能看出第 1、3、4、6、7 位是 1。 */
    unsigned int v = 0b0101'1010u;
    return v;
}

/* ---- C23：typeof / typeof_unqual ----
 * typeof(expr) 得到一个表达式的类型，常用来写"万能宏"：
 *   不用再为 int/float/double 各写一遍同样的宏。 */
#define MAX_GENERIC(a, b)          \
    do {                           \
        typeof(a) _ta = (a);       \
        typeof(b) _tb = (b);       \
        printf("    typeof 推出的类型占 %zu 字节, 较大者 = ", sizeof(_ta)); \
        printf(_Generic(_ta, int: "%d", double: "%f", default: "%d"),     \
               (_ta > _tb) ? _ta : _tb);                                  \
        printf("\n");                                                     \
    } while (0)

int main(void)
{
    printf("---- 0. 当前是不是 C23 模式 ----\n");
#if defined(__STDC_VERSION__)
    printf("__STDC_VERSION__ = %ld\n", (long)__STDC_VERSION__);
#else
    printf("没有 __STDC_VERSION__（说明按 C++ 编译了）\n");
#endif
    printf("HAVE_C23 = %d   （1 表示确实在 C23/C23 以上模式）\n", HAVE_C23);

    printf("---- 1. 二进制字面量与数字分隔符 ----\n");
    unsigned int v = binary_literal_demo();
    printf("0b0101'1010 = %u = 0x%02X\n", v, v);
    printf("数位分隔符 1'000'000 = %d, 0xDEAD'BEEF = 0x%X\n",
           1'000'000, 0xDEAD'BEEFu);

    printf("---- 2. typeof：用表达式的类型去声明变量 ----\n");
#if HAVE_C23
    int i = 3;
    double d = 2.5;
    typeof(i)   another_i = i + 1;              /* another_i 就是 int */
    typeof(d)   another_d = d * 2.0;            /* another_d 就是 double */
    const int ci = 9;
    typeof_unqual(ci) plain = 1;                /* 去掉 const：plain 可以改 */
    plain = 2;
    printf("typeof(i) another_i = %d    typeof(d) another_d = %f\n",
           another_i, another_d);
    printf("typeof_unqual(const int) 得到的是可修改的 int: %d\n", plain);

                /* 用 typeof 写的宏，同一个宏能吃 int 也能吃 double */
    printf("  MAX_GENERIC(3, 7):\n");
    MAX_GENERIC(3, 7);
    printf("  MAX_GENERIC(1.5, 2.25):\n");
    MAX_GENERIC(1.5, 2.25);
#else
    printf("当前不是 C23 模式，typeof 不可用（用 /std:clatest 重新编译）\n");
#endif

    printf("---- 3. static_assert 成了关键字 ----\n");
        /* C23 起 static_assert 直接是关键字，不用包含头文件。 它在**编译期**检查条件，运行期一点开销都没有 —— 很适合检验 "结构体布局是否和硬件寄存器一致"这种事。 */
    static_assert(sizeof(int) == 4, "本教程假定 int 是 32 位");
    struct reg_layout {
        unsigned int cr;
        unsigned int sr;
    };
    static_assert(sizeof(struct reg_layout) == 8, "寄存器块必须是 8 字节");
    printf("static_assert 全部通过\n");

    printf("---- 4. bool：C23 标准说它是关键字，但 MSVC 19.51 还不认 ----\n");

    bool ok = true;
    printf("bool ok = %d, true = %d, false = %d\n", (int)ok, (int)true, (int)false);

    printf("---- 5. 标准属性 [[...]] ----\n");
    printf("[[nodiscard]] / [[maybe_unused]] / [[fallthrough]] / [[deprecated]]\n");
    printf("这些在 C23 里成为标准写法；C17 下 MSVC 支持 [[...]] 会报语法错误。\n");

    printf("---- 6. 本机实测不支持的 C23 特性 ----\n");
    printf("constexpr、nullptr 在 MSVC 19.51 + /std:clatest 下仍然报\n");
    printf("  error C2065: constexpr/nullptr 未声明的标识符。\n");
    printf("所以：写跨编译器的代码时，先查编译器文档，再决定能不能用。\n");
    return 0;
}
