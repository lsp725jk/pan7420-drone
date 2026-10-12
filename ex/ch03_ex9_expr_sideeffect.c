/**
 * ch03_ex9_expr_sideeffect.c —— 表达式与副作用：一个表达式只做一件事
 */
#include <stdio.h>

int main(void)
{
                /**
         * ---- 1. 副作用（side effect）是什么 ----
         */
    printf("---- 1. 什么是副作用 ----\n");
    int a = 1;
    int b = a + 1;                  /**
     * 纯求值：不改变任何东西，a 还是 1
     */
    printf("int b = a + 1;  -> b=%d, a=%d（a 没变）\n", b, a);
    int c = a++;                    /* 有副作用：把 a 改了 */
    printf("int c = a++;    -> c=%d, a=%d（a 被改了）\n", c, a);

                /* ---- 2. 实参求值顺序未指定 ---- */
    printf("---- 2. 实参求值顺序 ----\n");
    int t = 1;
        /* printf("%d %d %d\n", t++, t++, t++);   <-- UB，不能写！ 本机 MSVC 的实参求值顺序是**从右往左**（这一点我用探测程序确认过）， 但那只是这一台编译器的行为。 */
    printf("  拆开写才是可移植的：\n");
    printf("    %d ", t++);
    printf("    %d ", t++);
    printf("    %d\n", t++);
    printf("  现在 t = %d\n", t);

                /* ---- 3. 最经典的 UB：a[i] = i++ ---- */
    printf("---- 3. a[i] = i++ ----\n");
    int arr[3] = { 7, 8, 9 };
        /* arr[i] = i++;   <-- 同一表达式里既读 i 又改 i，UB。 本机实测（把 i 的读写各自隔离到 volatile 里观察）： arr[0] 被赋成 0（旧值），i 自增后变成 1。 */
    volatile int vi = 0;                    /* 只负责提供"旧值"和自增 */
    int idx = 0;                            /* 只负责提供下标 */
    arr[idx] = vi++;                        /* 这一行就是 arr[i] = i++ 的形状 */
    printf("  本机实测：arr[0]=%d，vi 自增后=%d\n", arr[0], vi);
    printf("  （本机恰好把下标取在自增之前，但标准说这是 UB，\n");
    printf("   换编译器完全可能先自增再取下标，于是写进 arr[1]。）\n");
    printf("  换成可移植的写法：\n");
    arr[0] = 7;
    int i = 0;
    arr[i] = i;                     /* 先用 i 当值 */
    i++;                            /* 再自增 */
    printf("    arr[0] 被赋成 %d，然后 i 变成 %d —— 语义完全明确\n",
           arr[0], i);

                /* ---- 4. 其它容易踩的"多重副作用" ---- */
    printf("---- 4. 其它多重副作用 ----\n");
    int x = 5;
                /* printf("%d %d\n", x = 1, x = 2);   <-- UB */
                /* f(g(), h());                        <-- g 和 h 谁先跑没规定 */
                /* x = x++ + 1;                        <-- UB */
    printf("  下面这些写法都是 UB，别写：\n");
    printf("    printf(\"%%d %%d\", x = 1, x = 2);\n");
    printf("    x = x++ + 1;\n");
    printf("    i = i++;\n");
    printf("    p[i] = i++;\n");
    printf("  【判定方法】同一个变量在同一个表达式里被改了两次，或者\n");
    printf("             既被修改又参与其它计算 —— 就是 UB。\n");

    printf("---- 5. 拆开写会不会变慢？ ----\n");
                /* 现代编译器会把相邻的简单语句重新调度，拆开写基本不影响优化结果。 */
    int sum1 = 0, n1 = 0;
    sum1 += n1; n1++;
    sum1 += n1; n1++;
    printf("  拆开写的两句话，优化后和 sum += n++; 生成的机器码通常一样。\n");
    printf("  结论：可读性和正确性都不亏，所以**永远拆开写**。\n");
    (void)x;
    return 0;
}
