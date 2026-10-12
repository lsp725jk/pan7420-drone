/**
 * ch03_ex5_ternary_comma.c —— 三目运算符与逗号运算符
 */
#include <stdio.h>

/**
 * ---- 1. 三目最直观的用法：按条件取值 ----
 */
static const char *state_text(int state)
{
                /**
         * 比 if-else 少写几行，适合"二选一"这种短逻辑
         */
    return state ? "ON" : "OFF";
}

int main(void)
{
    printf("---- 1. 三目基本用法 ----\n");
    printf("flag=1 -> %s    flag=0 -> %s\n",
           state_text(1), state_text(0));

                /* 嵌在表达式里：求两个数的较大值 */
    int a = 7, b = 12;
    printf("max(%d,%d) = %d\n", a, b, a > b ? a : b);
    printf("min(%d,%d) = %d\n", a, b, a < b ? a : b);
    printf("绝对值 |-5| = %d\n", -5 < 0 ? -(-5) : -5);

                /* 变式：用三目给变量赋初值，比 if-else 紧 */
    int limit = (a > b) ? a : b;
    printf("limit = %d\n", limit);

                /* ---- 2. 三目可以嵌套（但请克制）---- */
    printf("---- 2. 嵌套三目 ----\n");
    for (int s = 0; s <= 3; s++) {
                /* 嵌套时一定加括号，不然读代码的人要靠优先级推 */
        const char *name = (s == 0) ? "IDLE"
                         : (s == 1) ? "ARMED"
                         : (s == 2) ? "FLYING"
                         : "FAILSAFE";
        printf("  state %d -> %s\n", s, name);
    }
    printf("超过三层的嵌套请改写成 if-else 或 switch + 查表。\n");

                /* ---- 3. 类型协调：两个分支类型不同会怎样 ---- */
    printf("---- 3. 两个分支类型不同 ----\n");
    int    i = 3;
    double d = 1.5;
                /* int 和 double 混用：结果类型是 double，i 会被转成 3.0 */
    printf("(1 ? i : d) = %f   sizeof = %zu\n",
           (1 ? i : d), sizeof(1 ? i : d));
    printf("(0 ? i : d) = %f\n", (0 ? i : d));
                /* 有符号和无符号混用：结果类型变成 unsigned，负数会被误解 */
    int          si = -1;
    unsigned int ui = 1u;
                /* 下面这行会触发 warning C4018/C4554 那一类"有符号无符号不匹配"的提示， 这里就是要演示这个坑，所以保留并压掉警告，让输出干净。 */
#pragma warning(push)
#pragma warning(disable : 4018)
    printf("(1 ? si : ui) = %u   <-- 期望 -1，实际被当成无符号打印\n",
           (1 ? si : ui));
#pragma warning(pop)
    printf("(0 ? si : ui) = %u   <-- 换成取 ui 那一支就正常\n", (0 ? si : ui));
    printf("解决办法：显式转换 (1 ? (int)si : (int)ui) = %d\n",
           (1 ? (int)si : (int)ui));

                /* ---- 4. 逗号运算符 ---- */
    printf("---- 4. 逗号运算符 ----\n");
    int x = (1, 2, 3);                      /* 依次求值，取最后一个 */
    printf("int x = (1,2,3);  -> x = %d\n", x);

    int n = 0;
    int y = (n = 5, n + 1);                 /* 先赋值，再用它的值加 1 */
    printf("int y = (n=5, n+1);  -> y = %d, n = %d\n", y, n);

                /* 最常见的用处：for 里同时推进两个变量 */
    printf("for 里同时推进 i 和 j：\n  ");
                /* 注意这里换了变量名 ii/jj：如果沿用外面的 i，会触发 warning C4456（内层声明遮蔽了外层），初学时容易被这个警告吓到。 */
    for (int ii = 0, jj = 10; ii < jj; ii++, jj--) {
        printf("(%d,%d) ", ii, jj);
    }
    printf("\n");
                /* 注意 for 的第一个分号段里的逗号是**声明分隔符**（int i=0, j=10）， 第三个分号段里的逗号才是逗号运算符（i++, j--）。 */

                /* ---- 5. 逗号运算符的坑 ---- */
    printf("---- 5. 逗号运算符的坑 ----\n");
    int arr[3] = { 10, 20, 30 };
    int idx = 0;

    printf("带括号：(idx=1, arr[idx]) = %d\n", (idx = 1, arr[idx]));
    printf("【坑】数组初始化里 arr[3] = {1,2,3} 的逗号是分隔符，不是运算符；\n");
    printf("     写成 arr[3] = {(1,2,3)} 就只会得到 1 个元素等于 3。\n");
    int one[3] = { (1, 2, 3) };
    printf("     int one[3] = {(1,2,3)};  -> one[0]=%d one[1]=%d one[2]=%d\n",
           one[0], one[1], one[2]);

                /* ---- 6. 三目 vs if-else：什么时候用哪个 ---- */
    printf("---- 6. 选择建议 ----\n");
    printf("三目：一行能写完的二选一取值，且两个分支都**没有副作用**\n");
    printf("if-else：分支里有多条语句、需要提前 return、或者逻辑超过两层\n");
                /* 三目的分支只求值一个，所以副作用只发生一次： */
    int cnt = 0;
    int r = (1 > 0) ? ++cnt : ++cnt;            /* 只有一个 ++cnt 会执行 */
    printf("(1>0) ? ++cnt : ++cnt  执行后 cnt = %d  (只加了一次)\n", r);
    return 0;
}
