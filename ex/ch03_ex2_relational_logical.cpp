/**
 * ch03_ex2_relational_logical.c —— 关系运算与逻辑运算
 */
#include <stdio.h>
#include <stdbool.h>
#include <math.h>       /**
 * fabs
 */

int main(void)
{
                /**
         * ---- 1. 关系运算的结果是 1/0，而且是 int ----
         */
    printf("---- 1. 关系运算的结果 ----\n");
    printf("3 > 2      = %d\n", 3 > 2);
    printf("3 < 2      = %d\n", 3 < 2);
    printf("3 == 3     = %d\n", 3 == 3);
    printf("3 != 3     = %d\n", 3 != 3);
    printf("sizeof(3 > 2) = %zu  <-- 结果是 int，占 4 字节\n", sizeof(3 > 2));
    printf("所以可以这样累加'有多少个条件成立'：\n");
    int count = (3 > 2) + (3 < 2) + (1 == 1);
    printf("(3>2)+(3<2)+(1==1) = %d\n", count);

                /* ---- 2. 逻辑运算与真值表 ---- */
    printf("---- 2. 逻辑运算 ----\n");
    int t = 1, f = 0;
    printf("1 && 1 = %d   1 && 0 = %d   0 && 0 = %d\n", t && t, t && f, f && f);
    printf("1 || 1 = %d   1 || 0 = %d   0 || 0 = %d\n", t || t, t || f, f || f);
    printf("!1 = %d   !0 = %d   !!5 = %d   <-- !! 把任意值规整成 0/1\n",
           !t, !f, !!5);
                /* 任何非 0 的值都当"真"，不只是 1： */
    printf("if (5) 成立吗? %d     if (-1) 成立吗? %d\n", 5 ? 1 : 0, -1 ? 1 : 0);
    printf("所以不要写 if (x == 1) 去判断'有一个非 0 的 flag'，写 if (x) 就行\n");

                /* ---- 3. 短路求值：既是优化也是保护 ---- */
    printf("---- 3. 短路求值 ----\n");
    int calls = 0;
                /* 用带副作用的函数观察"右边到底执行了没有" */
    printf("先看 &&：0 && side() —— side() 不会被执行\n");
    if (0 && ++calls) {                   /* 左边是 0，右边不再求值 */
        printf("不会到这里\n");
    }
    printf("calls = %d\n", calls);
    printf("再看 ||：1 || side() —— side() 也不会被执行\n");
    if (1 || ++calls) {
        printf("会到这里（左边已经为真）\n");
    }
    printf("calls = %d\n", calls);

                /* 利用短路保护指针：先判空，再解引用 —— 这是最常见的保护写法 */
    int *p = NULL;
    printf("保护写法：if (p != NULL && *p == 5) —— 左边为假，右边不执行，不会崩\n");
    if (p != NULL && *p == 5) {
        printf("不会到这里\n");
    } else {
        printf("安全地跳过了\n");
    }
                /* 【坑】反过来写 if (*p == 5 && p != NULL) 就会先解引用 NULL，直接崩。 */

                /* ---- 4. 关系运算符不能连写 ---- */
    printf("---- 4. 数学写法在 C 里是错的 ----\n");
    int x = 5;
    printf("想判断 0 < x < 10：写成 (0 < x) < 10\n");
    printf("  0 < x      = %d\n", 0 < x);
    printf("  (0<x) < 10 = %d   <-- 1 < 10 恒真！哪怕 x = -100 也是 1\n", (0 < x) < 10);
    printf("  正确写法   = %d   (0 < x && x < 10)\n", 0 < x && x < 10);
    x = -100;
    printf("  x = -100 时 (0<x)<10 = %d   <-- 错误写法照样为真\n", (0 < x) < 10);
    printf("  x = -100 时正确写法  = %d\n", 0 < x && x < 10);

                /* ---- 5. 浮点数不能直接用 == ---- */
    printf("---- 5. 浮点比较 ----\n");
    double d1 = 0.1 + 0.2;
    double d2 = 0.3;
    printf("0.1+0.2 == 0.3  ->  %d\n", d1 == d2);
    printf("fabs(d1-d2) < 1e-9 -> %d   (用容差比较)\n", fabs(d1 - d2) < 1e-9);

                /* ---- 6. bool 类型（C99 起）让意图更清楚 ---- */
    printf("---- 6. 用 bool 让代码更可读 ----\n");
    bool armed = true;
    bool failsafe = false;
    printf("armed=%d failsafe=%d\n", (int)armed, (int)failsafe);
    printf("armed && !failsafe = %d   <-- 可读性比写 1/0 好\n",
           (int)(armed && !failsafe));
    printf("sizeof(bool) = %zu   (C 里它其实是 _Bool，占 1 字节)\n", sizeof(bool));
    printf("C23 起 bool/true/false 成为关键字，不必再包含 stdbool.h；\n");
    printf("但实测 MSVC 19.51 仍需包含（见第 02 章例 2-12）。\n");
    return 0;
}
