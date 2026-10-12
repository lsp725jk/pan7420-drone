/**
 * ch08_ex07.c —— 变式：宏什么时候该换成函数（static inline）
 */
#include <stdio.h>

#define SQ_MACRO(x)   ((x) * (x))                      /**
 * 宏：无类型，参数求值两次
 */
static inline int sq_inline(int x) { return x * x; }   /**
 * 函数：有类型，求值一次
 */

/**
 * 变式 1：宏能做到而函数做不到的事 —— 让编译器算出"数组有多少个元素"
 */
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

/* 变式 2：函数能做到而宏做不到的事 —— 参数类型检查 */
static inline int clamp_int(int v, int lo, int hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

int main(void)
{
    int arr[7];
    printf("ARRAY_LEN(arr) = %d  （宏能用 sizeof，函数不行）\n",
           (int)ARRAY_LEN(arr));

                /* 求值次数：宏两次，函数一次 */
    int i = 2;
    int r_macro = SQ_MACRO(i++);
    printf("SQ_MACRO(i++) = %d , 之后 i = %d  （被加了两次）\n", r_macro, i);
    i = 2;
    int r_inline = sq_inline(i++);
    printf("sq_inline(i++) = %d , 之后 i = %d  （只加一次）\n", r_inline, i);

                /* 类型安全：函数会做隐式转换；宏会把参数原样塞进表达式，结果类型不受控 */
    printf("clamp_int(15, 0, 10) = %d\n", clamp_int(15, 0, 10));
    printf("clamp_int(-3, 0, 10) = %d\n", clamp_int(-3, 0, 10));

                /* 变式 3：C99 起，真正"零开销且安全"的首选是 static inline 函数； 宏只留给它不可替代的场合：字符串化、记号粘贴、sizeof 取数组长度、条件编译 */
    printf("sizeof(int) 走宏也没问题 : %d\n", (int)(sizeof(int) * 1));
    return 0;
}
