/**
 * ch03_ex16_arith_ops.c —— 复合赋值、一元运算符与一个常见的陷阱
 */
#include <stdio.h>

static int g_side_calls = 0;

/**
 * 带副作用的下标函数：用来观察"下标被求值几次"
 */
static int next_index(void)
{
    g_side_calls++;
    return g_side_calls - 1;                    /**
     * 第一次返回 0，第二次返回 1 ...
     */
}

int main(void)
{
    printf("---- 1. 复合赋值只对左边求值一次 ----\n");
    int arr[3] = { 10, 20, 30 };
    g_side_calls = 0;
    arr[next_index()] += 5;                     /* 下标表达式求值一次 */
    printf("  arr[next_index()] += 5;  -> 副作用调用次数 = %d\n", g_side_calls);
    printf("    arr[0] = %d\n", arr[0]);

    g_side_calls = 0;
    arr[next_index()] = arr[next_index()] + 5;           /* 下标表达式求值两次！ */
    printf("  arr[next_index()] = arr[next_index()] + 5;\n");
    printf("    -> 副作用调用次数 = %d   <-- 两次！读和写用的是不同下标\n",
           g_side_calls);
    printf("    实际结果：arr[0]=%d arr[1]=%d（写进了 arr[1]）\n", arr[0], arr[1]);
    printf("  【这就是复合赋值的价值】\n");

    printf("---- 2. 复合赋值家族的完整清单 ----\n");
    int x = 100;
    printf("  初始 x = %d\n", x);
    x += 5;  printf("  x += 5   -> %d\n", x);
    x -= 3;  printf("  x -= 3   -> %d\n", x);
    x *= 2;  printf("  x *= 2   -> %d\n", x);
    x /= 4;  printf("  x /= 4   -> %d\n", x);
    x %= 7;  printf("  x %%= 7   -> %d\n", x);
    x <<= 2; printf("  x <<= 2  -> %d\n", x);
    x >>= 1; printf("  x >>= 1  -> %d\n", x);
    x |= 0x0F; printf("  x |= 0x0F -> 0x%X\n", x);
    x &= 0x3C; printf("  x &= 0x3C -> 0x%X\n", x);
    x ^= 0xFF; printf("  x ^= 0xFF -> 0x%X\n", x);

    printf("---- 3. 一元运算符的优先级 ----\n");
    int a = 5, b = 3;
    printf("  -a + b      = %d   （一元 - 先算，再和 b 相加）\n", -a + b);
    printf("  -(a + b)    = %d\n", -(a + b));
    printf("  !a + b      = %d   （!a = 0，再 +3）\n", !a + b);
    printf("  ~a          = %d   （按位取反）\n", ~a);
    printf("  -a * b      = %d   （一元 - 作用于 a，再乘 b）\n", -a * b);
    printf("  -(a * b)    = %d\n", -(a * b));
    printf("  【规则】一元运算符（! ~ + - ++ -- * & sizeof）优先级**最高**（除后缀）。\n");

    printf("---- 4. 常见的一元/二元混淆 ----\n");
    int p = 4;
    printf("  p++ + ++p 是 UB（见 03.7），这里演示合法的等价拆写：\n");
    int old = p++;
    int r = old + ++p;
    printf("    old = %d, 之后 p = %d, r = old + ++p = %d\n", old, p, r);
    printf("  【建议】看到混用就拆开写。\n");

    printf("---- 5. 一个真实的算术陷阱：先加后除 ----\n");
    int sum = 1000, cnt = 3;
    printf("  sum=%d cnt=%d\n", sum, cnt);
    printf("  sum / cnt + 1     = %d   （先除后加）\n", sum / cnt + 1);
    printf("  sum / (cnt + 1)   = %d   （先加后除）\n", sum / (cnt + 1));
    printf("  (sum + cnt - 1) / cnt = %d   （ceil 除法的整数写法）\n",
           (sum + cnt - 1) / cnt);
    printf("  算「平均每个包多少字节」「向上取整要几个包」时，这两个式子很容易写反。\n");
    return 0;
}
