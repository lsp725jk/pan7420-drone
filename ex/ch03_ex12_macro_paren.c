/**
 * ch03_ex12_macro_paren.c —— 宏参数括号：一个字符的差别
 */
#include <stdio.h>

/**
 * ---- 错误与正确版本对照 ----
 */
#define BAD_SQ(x)     x * x
#define GOOD_SQ(x)    ((x) * (x))
#define BAD_MAX(a, b) a > b ? a : b
#define GOOD_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define GOOD_ABS(x)   ((x) < 0 ? -(x) : (x))

/* 用 static inline 写的等价物：有类型检查，参数只求值一次 */
static inline int sq_inline(int x)  { return x * x; }
static inline int max_inline(int a, int b) { return (a > b) ? a : b; }

/* 用宏给"计数"加副作用，用来演示参数被求值多次 */
static int g_counter = 0;
static int next_id(void)
{
    g_counter++;
    return g_counter;
}

int main(void)
{
    printf("---- 1. 少一对括号就错 ----\n");
    printf("BAD_SQ(1+2)  = %d   <-- 展开成 1+2*1+2 = 5\n", BAD_SQ(1 + 2));
    printf("GOOD_SQ(1+2) = %d   <-- 展开成 ((1+2)*(1+2)) = 9\n", GOOD_SQ(1 + 2));
    printf("BAD_SQ(3)    = %d（单个值看不出问题，所以这个 bug 很隐蔽）\n",
           BAD_SQ(3));
    printf("GOOD_SQ(3)   = %d\n", GOOD_SQ(3));

    printf("---- 2. 条件宏不加括号更危险 ----\n");
    printf("BAD_MAX(1, 2) + 1  = %d   <-- 展开成 1>2?1:2+1 = 3\n",
           BAD_MAX(1, 2) + 1);
    printf("GOOD_MAX(1, 2) + 1 = %d   <-- 展开成 ((1>2)?1:2) + 1 = 3\n",
           GOOD_MAX(1, 2) + 1);
    printf("BAD_MAX(3, 2) * 2  = %d   <-- 展开成 3>2?3:2*2 = 3，丢了那次乘 2\n",
           BAD_MAX(3, 2) * 2);
    printf("GOOD_MAX(3, 2) * 2 = %d\n", GOOD_MAX(3, 2) * 2);
    printf("【症状】宏在简单测试里通过，被放进更大的表达式就出错。\n");

    printf("---- 3. 宏参数会被求值多次 ----\n");
    g_counter = 0;
    printf("next_id() 目前计数 = %d\n", g_counter);
                /* GOOD_SQ(next_id()) 会展开成 ((next_id()) * (next_id()))， 也就是调用两次 —— 这是 UB，而且计数器会跳 2。 */
    int v = GOOD_SQ(next_id());
    printf("GOOD_SQ(next_id()) 之后：结果 = %d，计数器 = %d\n", v, g_counter);
    printf("  参数被求值了 **两次**，所以计数器从 0 直接跳到 2。\n");
    printf("  这是 UB（同一表达式里对同一状态改两次），千万别这么写。\n");

    g_counter = 0;
    v = sq_inline(next_id());
    printf("sq_inline(next_id()) 之后：结果 = %d，计数器 = %d   <-- 只调用一次\n",
           v, g_counter);
    printf("  inline 函数是真正的函数，参数只求值一次，这是它的关键优势。\n");

    printf("---- 4. 什么时候还不得不用宏 ----\n");
    printf("  a) 需要类型泛化：#define MAX(a,b) ((a)>(b)?(a):(b)) 能同时吃 int/float\n");
    printf("  b) 需要 # 或 ##：字符串化、拼记号（日志宏、寄存器名）；\n");
    printf("  c) 需要 __LINE__ / __FILE__ 这类预定义宏；\n");
    printf("  d) 需要在预处理期算条件（#if）。\n");
    printf("  其余场合优先写 static inline 函数。\n");
    printf("  本机实测 GOOD_ABS(-5) = %d（宏版本可以吃任意类型）\n",
           GOOD_ABS(-5));
    printf("  max_inline(3, 7) = %d（函数版本有类型检查）\n",
           max_inline(3, 7));
    return 0;
}
