/**
 * ch01_ex4_pipeline.c —— 预处理都干了什么：#include / #define / #if / 宏
 */
#include <stdio.h>

/**
 * ---- 对象宏（object-like macro）：纯粹的名字替换 ----
 */
#define PI 3.14159f

/* ---- 函数宏（function-like macro）：带参数的替换 ---- 参数两侧加括号是好习惯：不加括号时 SQUARE(1+2) 会展开成 1+2*1+2 = 5， 而不是期望的 9。整个宏再套一层括号，防止它被更大的表达式"咬"到。 */
#define SQUARE(x)  ((x) * (x))
#define BAD_SQUARE(x)  x * x

/* ---- 宏可以"拼接"和"字符串化"：嵌入式里常用于拼寄存器名 ---- */
#define REG(base, idx)  (base##idx)
#define STR(x)          #x

/* ---- 条件编译：同一份源码按需求裁剪 ---- */
#define BOARD_PY32F030 1
#define BOARD_STM32F103 2
#ifndef BOARD
#define BOARD BOARD_PY32F030
#endif

/* ---- 标准预定义宏：编译器自动给的，调试打印很好用 ---- */
static void show_compiler(void)
{
    printf("文件      : %s\n", __FILE__);
    printf("当前行    : %d\n", __LINE__);
    printf("编译日期  : %s %s\n", __DATE__, __TIME__);

#if defined(__STDC_VERSION__)
    printf("符合的标准: C, __STDC_VERSION__ = %ld  (201710 = C17)\n",
           (long)__STDC_VERSION__);
#elif defined(__cplusplus)
    printf("符合的标准: C++, __cplusplus = %ld\n", (long)__cplusplus);
#else
    printf("符合的标准: 未知（既没有 __STDC_VERSION__ 也没有 __cplusplus）\n");
#endif
}

int main(void)
{
    printf("PI = %f\n", PI);

                /* 宏是"文本替换"，所以它可以接受任何类型，也没有类型检查 */
    printf("SQUARE(3)   = %d\n", SQUARE(3));
    printf("SQUARE(1+2) = %d   <-- 括号加对了才等于 9\n", SQUARE(1 + 2));
    printf("BAD_SQUARE(1+2) = %d   <-- 少一对括号，结果变成 1+2*1+2\n",
           BAD_SQUARE(1 + 2));

                /* ## 把两个记号粘成一个新的记号 */
    int REG(reg, 0) = 100;              /* 展开后就是 int reg0 = 100; */
    printf("REG(reg,0) = %d\n", reg0);
    printf("STR(hello) = %s\n", STR(hello));

                /* 条件编译：只有 BOARD 等于 PY32F030 时这段才进入编译 */
#if BOARD == BOARD_PY32F030
    printf("板型：PY32F030\n");
#elif BOARD == BOARD_STM32F103
    printf("板型：STM32F103\n");
#else
#error "未知板型"
#endif

    show_compiler();

                /* __LINE__ 会随所在行变化：下面两个打印出来不是同一个数字 */
    printf("这一行的行号 = %d\n", __LINE__);
    printf("这一行的行号 = %d\n", __LINE__);
    return 0;
}
