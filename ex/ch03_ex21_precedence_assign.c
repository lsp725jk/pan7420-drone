/**
 * ch03_ex21_precedence_assign.c —— 赋值表达式与链式赋值 / 复合赋值的优先级
 */
#include <stdio.h>

static int read_next(int *state)
{
    static const int seq[4] = { 10, 20, 30, 0 };
    int v = seq[*state];
    if (*state < 3) {
        (*state)++;
    }
    return v;
}

int main(void)
{
    printf("---- 1. 赋值是右结合 ----\n");
    int a, b, c;
    a = b = c = 7;                      /* a = (b = (c = 7)) */
    printf("  a = b = c = 7  -> a=%d b=%d c=%d\n", a, b, c);
    int x = 1, y = 2, z = 3;
    x = y = z + 1;                      /* x = (y = (z+1)) */
    printf("  x = y = z + 1  -> x=%d y=%d z=%d\n", x, y, z);

    printf("---- 2. 赋值表达式的值 ----\n");
    int p = 0;
    printf("  (p = 5) 的值是 %d（被赋的值本身）\n", (p = 5));
    printf("  所以 printf(\"%%d\", p = 9) 打印 %d\n", (p = 9));
    printf("  【为什么有用】链式赋值和「边赋值边判断」都依赖这个特性。\n");

    printf("---- 3. 赋值的优先级很低：if (a = b > 0) 发生了什么 ----\n");
    int m = 0, n = 5;
                /* 下面这一行是**故意**写的，用来演示优先级： 被解析成 m = (n > 0)，也就是 m 被赋成 1，而不是"n 赋给 m 再判断 m > 0" */
    int r = (m = n > 0);
    printf("  m = n > 0  -> m = %d（等于 (n>0) 的结果），n = %d，r = %d\n", m, n, r);
    printf("  MSVC /W4 会为此报 warning C4706（条件表达式中的赋值）。\n");
    printf("  想表达「先赋值再判断」必须加括号：\n");
    int m2 = 0, n2 = 5;
    if ((m2 = n2) > 0) {
        printf("    if ((m2 = n2) > 0) -> m2 = %d，条件成立\n", m2);
    }

    printf("---- 4. 边读边判断的惯用法：读循环 ----\n");
    int state = 0;
    int v;
    int count = 0;
                /* 这是 C 里非常常见（且合法）的写法：赋值表达式的值当条件用 */
    while ((v = read_next(&state)) != 0) {
        printf("  读到 %d\n", v);
        count++;
        if (count > 10) { break; }                      /* 保险 */
    }
    printf("  共读到 %d 个非零值（读到 0 就停）\n", count);
    printf("  【注意】((v = read(&state)) != 0) 里的两层括号都是必须的：\n");
    printf("          外层给 while，内层防止被解析成 v = (read(...) != 0)。\n");

    printf("---- 5. 复合赋值的优先级也低 ----\n");
    int arr[3] = { 1, 2, 3 };
    int i = 0;
    arr[i] += 10;                       /* 等价于 arr[i] = arr[i] + 10 */
    printf("  arr[i] += 10  -> arr[0] = %d\n", arr[0]);
    printf("  写成 arr[i] = arr[i] + 10 也对，但复合赋值只对下标求值一次（更快更安全）。\n");
    int total = 0;
    total += i++ + 1;                   /* 合法：+= 的右边是一个完整表达式 */
    printf("  total += i++ + 1  -> total = %d, i = %d\n", total, i);
    printf("  【提醒】把自增和复合赋值混在一行也要小心，能拆就拆。\n");
    return 0;
}
