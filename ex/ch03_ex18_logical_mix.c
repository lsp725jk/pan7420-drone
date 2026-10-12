/**
 * ch03_ex18_logical_mix.c —— 逻辑运算与位运算不能混用：& vs &&、| vs ||
 */
#include <stdio.h>

static int g_calls = 0;
static int side(void)
{
    g_calls++;
    return 1;
}

int main(void)
{
    printf("---- 1. & 与 && 的结果对照 ----\n");
    int vals[4] = { 0, 1, 2, 3 };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("  %d & %d = %d   %d && %d = %d\n",
                   vals[i], vals[j], vals[i] & vals[j],
                   vals[i], vals[j], (vals[i] && vals[j]) ? 1 : 0);
        }
    }
    printf("  【结论】& 得到的是位模式，&& 得到的是 0/1。\n");

    printf("---- 2. 掩码判断的三种写法 ----\n");
    unsigned int flags = 0x05;                  /* bit0 和 bit2 置位 */
    unsigned int mask  = 0x04;                  /* 关心 bit2 */
    printf("  flags = 0x%02X, mask = 0x%02X\n", flags, mask);
    printf("  if (flags & mask)              -> %s（推荐：判断这一位是不是 1）\n",
           (flags & mask) ? "真" : "假");
    printf("  if ((flags & mask) == mask)    -> %s（判断这一组位都置位了）\n",
           ((flags & mask) == mask) ? "真" : "假");
#pragma warning(push)
#pragma warning(disable : 4554)
                /* 下面这一行是**故意**写错顺序的，用来演示 == 的优先级高于 & */
    printf("  if (flags & mask == mask)      -> %s（错！== 优先级高于 &）\n",
           (flags & mask == mask) ? "真" : "假");
#pragma warning(pop)
    printf("  【症状】第三种写法的结果和第二种完全不同，MSVC /W4 报 C4554。\n");

    printf("---- 3. | 与 || 的结果对照 ----\n");
    printf("  0x01 | 0x02 = 0x%02X   （按位或：保留两边的 1）\n", 0x01 | 0x02);
    printf("  1 || 2     = %d        （逻辑或：结果是 1）\n", (1 || 2) ? 1 : 0);
    printf("  0x01 || 0x02 = %d      （逻辑或：非 0 就是真）\n",
           (0x01 || 0x02) ? 1 : 0);
    printf("  【常见 bug】想置位却写成了逻辑或（flags ||= 0x04 是语法错误）；\n");
    printf("             而 if (a || b) 想判断「两个标志有一个置位」也是错的 ——\n");
    printf("             应该写 if (a != 0 || b != 0)，或者用位或再判断非 0。\n");

    printf("---- 4. 短路只属于逻辑运算 ----\n");
    g_calls = 0;
    int r1 = side() | side();                   /* 位或：两边都求值 */
    printf("  side() | side()  -> 结果 %d, 调用次数 %d（都求值）\n", r1, g_calls);
    g_calls = 0;
    int r2 = side() || side();                  /* 逻辑或：右边被短路 */
    printf("  side() || side() -> 结果 %d, 调用次数 %d（右边被短路）\n", r2, g_calls);
    printf("  【注意】用位运算时，所有操作数**一定**都会被求值。\n");
    printf("  所以带副作用的表达式千万别放进位运算里图省事。\n");

    printf("---- 5. 实用建议 ----\n");
    printf("  a) 位掩码判断：if (flags & MASK) 或 if ((flags & MASK) == MASK)；\n");
    printf("  b) 逻辑判断：if (a && b) / if (a || b)（还能顺便利用短路）；\n");
    printf("  c) 两者绝不混用；也不要用 %%d 打印位运算结果去验证逻辑。\n");
    return 0;
}
