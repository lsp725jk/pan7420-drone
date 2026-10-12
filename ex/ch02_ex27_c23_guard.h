/**
 * ch02_ex27_c23_guard.h —— 两个编译器都能编的"特性探测"头文件（配对示例）
 */
#ifndef CH02_EX27_C23_GUARD_H
#define CH02_EX27_C23_GUARD_H

#include <stdint.h>

/**
 * 探测标准版本：C23 的 __STDC_VERSION__ 是 202311 或 202312
 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#  define HAVE_C23_LANG 1
#else
#  define HAVE_C23_LANG 0
#endif

/* 探测 typeof：MSVC 在 /std:clatest 下提供 typeof； GCC/Clang 从很早就有 __typeof__ 扩展。 */
#if HAVE_C23_LANG
#  define MY_TYPEOF(x) typeof(x)
#elif defined(__GNUC__) || defined(__clang__)
#  define MY_TYPEOF(x) __typeof__(x)
#else
#  define MY_TYPEOF(x) int      /* 最后的退路：只能写死（会失去泛型能力） */
#endif

/* 用它写一个"类型跟随实参"的宏：求两个值里的较大者并打印类型大小 */
#define PRINT_MAX(a, b)  do {                          \
        MY_TYPEOF(a) _ma = (a);                        \
        MY_TYPEOF(b) _mb = (b);                        \
        printf("  两者中较大的 = ");                   \
        printf(_Generic(_ma, int: "%d", double: "%f",  \
                        default: "%d"),                \
               (_ma > _mb) ? _ma : _mb);               \
        printf("   (类型大小 %zu 字节)\n", sizeof(_ma)); \
    } while (0)

#endif /* CH02_EX27_C23_GUARD_H */
