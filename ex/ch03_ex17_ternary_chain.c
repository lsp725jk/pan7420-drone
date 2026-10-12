/**
 * ch03_ex17_ternary_chain.c —— 三目 / if-else / 查表：同一个问题的三种写法
 */
#include <stdio.h>
#include <string.h>

/**
 * 写法一：嵌套三目（超过两层就开始难读）
 */
static const char *by_ternary(int code)
{
    return (code == 0) ? "IDLE"
         : (code == 1) ? "ARM"
         : (code == 2) ? "FLY"
         : (code == 3) ? "FAULT"
         : "UNKNOWN";
}

/* 写法二：if-else 链（等价，更长但更好加逻辑） */
static const char *by_ifelse(int code)
{
    if (code == 0) {
        return "IDLE";
    } else if (code == 1) {
        return "ARM";
    } else if (code == 2) {
        return "FLY";
    } else if (code == 3) {
        return "FAULT";
    } else {
        return "UNKNOWN";
    }
}

/* 写法三：查表（取值连续时最优） */
static const char *const g_names[] = { "IDLE", "ARM", "FLY", "FAULT" };
#define NAME_COUNT ((int)(sizeof(g_names) / sizeof(g_names[0])))

static const char *by_table(int code)
{
    if (code < 0 || code >= NAME_COUNT) {
        return "UNKNOWN";
    }
    return g_names[code];
}

int main(void)
{
    printf("---- 1. 三种写法结果必须一致 ----\n");
    int all_same = 1;
    for (int c = -1; c <= 5; c++) {
        const char *a = by_ternary(c);
        const char *b = by_ifelse(c);
        const char *d = by_table(c);
                                /* 用 strcmp 比内容 —— 三个函数返回的可能是**不同的字符串字面量**， 地址不同但内容相同，这正是"比较字符串必须用 strcmp 而不是 =="的例子。 */
        int same = (strcmp(a, b) == 0) && (strcmp(b, d) == 0);
        if (!same) { all_same = 0; }
        printf("  code %2d: 三目=%-7s if-else=%-7s 查表=%-7s %s\n",
               c, a, b, d, same ? "一致" : "**不一致**");
    }
    printf("  三者结果完全一致：%s\n", all_same ? "是" : "否");
    printf("  【注意】三目和 if-else 对任何输入都返回合法字符串；\n");
    printf("          查表必须先判边界，否则 code = 5 时就越界读了。\n");

    printf("---- 2. 可读性对比 ----\n");
    printf("  三目链：4 层缩进在对齐的 ? 上，改动要动多处；\n");
    printf("  if-else：一行一个分支，加逻辑容易；\n");
    printf("  查表：加一项只改数组，但**没有编译期检查**（写错下标不报错）。\n");

    printf("---- 3. 什么时候该换成查表 ----\n");
    printf("  a) 输入是连续的小整数（状态码、命令字、枚举）—— 查表；\n");
    printf("  b) 每个分支要做的事不一样、好几行 —— if-else 或 switch；\n");
    printf("  c) 只有二选一且两个分支都没副作用 —— 三目；\n");
    printf("  d) 三目超过两层 —— 一定要改（本示例的 by_ternary 已经是上限）。\n");

    printf("---- 4. 数值表的例子（查表也能存参数）----\n");
    static const int g_duty[4] = { 0, 250, 500, 1000 };           /* 各状态的占空比 */
    for (int c = -1; c <= 4; c++) {
        int duty = (c < 0 || c >= 4) ? -1 : g_duty[c];
        printf("  code %2d -> 占空比 %d\n", c, duty);
    }
    printf("  查表同时做到了逻辑清晰和数据与代码分离。\n");
    return 0;
}
