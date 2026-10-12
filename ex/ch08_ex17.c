/**
 * ch08_ex17.c —— 预定义宏（predefined macros）清单
 */
#include <stdio.h>

int main(void)
{
    printf("__FILE__          = %s\n", __FILE__);
    printf("__LINE__          = %d\n", __LINE__);
    printf("__DATE__          = %s\n", __DATE__);
    printf("__TIME__          = %s\n", __TIME__);

                /* 变式：跨平台代码里必须这样写，不能假设宏存在 */
#ifdef __STDC__
    printf("__STDC__          = %d   （1 表示遵循标准 C）\n", __STDC__);
#else
    printf("__STDC__          未定义  <-- MSVC 就是这样，GCC/Clang 会给出 1\n");
#endif

                /* __STDC_VERSION__ 的值：C99=199901L, C11=201112L, C17=201710L, C23=202311L */
#ifdef __STDC_VERSION__
    printf("__STDC_VERSION__  = %ldL （C17 应为 201710L）\n", (long)__STDC_VERSION__);
#else
    printf("__STDC_VERSION__  未定义（C89 环境）\n");
#endif

                /* 裸机（freestanding）环境下这个宏是 0，带着 RTOS/裸机启动代码时通常是 1 */
    printf("__STDC_HOSTED__   = %d   （0 = 独立环境/裸机，1 = 有宿主操作系统）\n",
           __STDC_HOSTED__);

                /* 变式：编译器识别宏。写跨平台代码时就靠它们分支 */
#if defined(_MSC_VER)
    printf("_MSC_VER          = %d （MSVC 主版本号，本机为 VS 2026 系列）\n", _MSC_VER);
    printf("_MSC_FULL_VER     = %d\n", _MSC_FULL_VER);
#elif defined(__GNUC__)
    printf("__GNUC__          = %d\n", __GNUC__);
#elif defined(__ARMCC_VERSION)
    printf("__ARMCC_VERSION   = %d\n", __ARMCC_VERSION);
#endif

                /* 变式：__COUNTER__ 每次出现就 +1，常用来生成"唯一名字"（MSVC/GCC 扩展，非标准） */
    printf("__COUNTER__ 连续三次 = %d , %d , %d\n", __COUNTER__, __COUNTER__, __COUNTER__);

                /* __func__ 不是宏，是每个函数里自动存在的字符串常量（C99 起） */
    printf("当前函数名 __func__ = %s\n", __func__);
    return 0;
}
