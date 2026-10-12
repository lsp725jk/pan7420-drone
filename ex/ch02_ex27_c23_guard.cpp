/**
 * ch02_ex27_c23_guard.c —— C23 特性的"探测 + 回退"写法
 */
#include <stdio.h>
#include "ch02_ex27_c23_guard.h"

int main(void)
{
    printf("---- 1. 探测结果 ----\n");
#if defined(__STDC_VERSION__)
    printf("__STDC_VERSION__ = %ld\n", (long)__STDC_VERSION__);
#else
    printf("这不是 C 编译器（没有 __STDC_VERSION__）\n");
#endif
    printf("HAVE_C23_LANG = %d   （1 表示在 C23 模式下）\n", HAVE_C23_LANG);

    printf("---- 2. 用探测出来的能力写泛型宏 ----\n");
#if HAVE_C23_LANG
    printf("当前用 typeof（C23 原生写法）：\n");
#elif defined(__GNUC__) || defined(__clang__)
    printf("当前用 __typeof__（GCC/Clang 扩展）：\n");
#else
    printf("当前没有 typeof，退化成 int（会丢泛型能力）：\n");
#endif
    PRINT_MAX(3, 7);
    PRINT_MAX(1.5, 2.25);

    printf("---- 3. 其它 C23 特性也要这样探测 ----\n");
    printf("  constexpr  -> #if HAVE_C23_LANG ... #else 用 #define / enum\n");
    printf("  nullptr    -> #if HAVE_C23_LANG ... #else 用 NULL\n");
    printf("  static_assert -> C11 起有 _Static_assert，可以一直用它\n");
    printf("  bool       -> C99 起包含 <stdbool.h> 就能用，不必等 C23\n");
    printf("  【本机实测】MSVC 19.51 + /std:clatest 下：\n");
    printf("    typeof       可用；\n");
    printf("    constexpr    不可用（error C2065 未声明的标识符）；\n");
    printf("    nullptr      不可用（同上）；\n");
    printf("    bool 关键字  不可用（仍需 #include <stdbool.h>）。\n");

    printf("---- 4. 为什么工程上一定要这么写 ----\n");
    printf("  你的飞控工程要同时支持 Keil(ARMCC) 和 GCC，两个编译器的\n");
    printf("  C 标准版本、扩展、内建宏都不一样。用 __STDC_VERSION__ +\n");
    printf("  编译器标识宏（__GNUC__ / __CC_ARM）做分支，是唯一的可移植办法。\n");
    printf("  【反面教材】直接写 typeof / constexpr 而不做探测 ——\n");
    printf("  换一个编译器就满屏 error，而且报错位置看起来毫不相关。\n");
    return 0;
}
