/**
 * ch04_ex10_inline_fnptr.c —— inline 与函数指针
 */
#include <stdio.h>

/**
 * ---- 1. static inline：小工具函数的标准写法 ----
 */
static inline int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

static inline int clamp_int(int v, int lo, int hi)
{
    if (v < lo) { return lo; }
    if (v > hi) { return hi; }
    return v;
}

/* ---- 2. 函数指针的基本用法 ---- */
static int add(int a, int b) { return a + b; }
static int sub(int a, int b) { return a - b; }
static int mul(int a, int b) { return a * b; }

/* 用 typedef 让函数指针类型好写（强烈推荐） */
typedef int (*binop_t)(int, int);
/* 上面这一行的读法：binop_t 是"指向 (int,int)->int 函数"的指针类型 */

/* ---- 3. 函数指针作参数：回调 ---- */
static int apply(binop_t op, int a, int b)
{
    return op(a, b);                    /* 通过指针调用，写 (*op)(a,b) 也完全等价 */
}

/* ---- 4. 函数指针数组：代替长 switch，做"跳转表" ---- */
static void state_idle(void)     { printf("    状态：空闲，等解锁指令\n"); }
static void state_armed(void)    { printf("    状态：已解锁，等油门\n"); }
static void state_flying(void)   { printf("    状态：飞行中\n"); }
static void state_failsafe(void) { printf("    状态：失控保护，正在降落\n"); }

typedef void (*state_fn_t)(void);

int main(void)
{
    printf("---- 1. inline 的效果 ----\n");
    printf("max_int(3,7) = %d\n", max_int(3, 7));
    printf("clamp_int(150, 0, 100) = %d\n", clamp_int(150, 0, 100));
    printf("clamp_int(-5, 0, 100) = %d\n", clamp_int(-5, 0, 100));
    printf("【怎么确认真的内联了】用 /O2 编译后看反汇编，或者\n");
    printf("用 __forceinline（MSVC 专有）强制内联。\n");
    printf("【inline 的坑】把普通 inline 函数写在头文件里被多个 .c 包含，\n");
    printf("可能报 LNK2019（找不到符号）或 LNK2005（重复定义）。\n");
    printf("最稳的写法：static inline（每个 .c 各有一份，互不冲突）。\n");

    printf("---- 2. 函数指针怎么声明和赋值 ----\n");
    binop_t op = add;                           /* 函数名会退化成函数地址，不用写 &add */
    printf("op(3,4)   = %d\n", op(3, 4));
    printf("(*op)(3,4)= %d   （两种写法等价）\n", (*op)(3, 4));
    op = &sub;                                  /* 写 & 也可以，完全一样 */
    printf("换指向 sub: op(3,4) = %d\n", op(3, 4));

                /* 不用 typedef 的写法，对比一下就知道 typedef 多重要 */
    int (*raw_fp)(int, int) = mul;
    printf("不用 typedef: int (*raw_fp)(int,int) = mul; raw_fp(3,4) = %d\n",
           raw_fp(3, 4));

    printf("---- 3. 函数指针作参数（回调）----\n");
    printf("apply(add, 10, 4) = %d\n", apply(add, 10, 4));
    printf("apply(sub, 10, 4) = %d\n", apply(sub, 10, 4));
    printf("apply(mul, 10, 4) = %d\n", apply(mul, 10, 4));
    printf("【嵌入式场景】把'按键按下时做什么'存成函数指针，\n");
    printf("主循环检测到按键就直接调用，不用写一堆 if-else。\n");

    printf("---- 4. 函数指针数组（跳转表）----\n");
    static state_fn_t state_table[4] = { state_idle, state_armed,
                                         state_flying, state_failsafe };
    for (int s = 0; s < 4; s++) {
        printf("  state %d:\n", s);
        state_table[s]();                               /* 直接按下标调用 */
    }
    printf("  好处：加一个状态只要在表里加一项，不用改分派逻辑；\n");
    printf("  代价：没有 switch 的编译期检查，下标写错会调到别的函数。\n");

    printf("---- 5. 函数指针的几个坑 ----\n");
    printf("a) 类型必须完全匹配：返回类型、参数类型和个数都要一样。\n");
    printf("   把 int f(int) 赋给 int (*)(double) 会编译报错（C 里是警告）。\n");
    printf("b) 一定要判空再调用：\n");
    binop_t maybe = NULL;
    if (maybe != NULL) {
        printf("    （不会执行）\n");
    } else {
        printf("    未初始化的函数指针直接调用 = 跳到 0 地址 = 必崩。\n");
    }
    printf("c) 函数指针打印地址要用 (void*) 转换：%p\n", (void *)add);

    printf("---- 6. 语法速查：四种难读的声明 ----\n");
    printf("  int  *f(void);        f 是函数，返回 int*\n");
    printf("  int (*f)(void);       f 是指针，指向'返回 int 的无参函数'\n");
    printf("  int (*f[3])(void);    f 是数组，元素是函数指针\n");
    printf("  int (*f)(int (*)(int)); f 是函数指针，它的参数也是函数指针\n");
    printf("  读法：从变量名出发，先看右边有没有 ()，再看左边有没有 *。\n");
    return 0;
}
