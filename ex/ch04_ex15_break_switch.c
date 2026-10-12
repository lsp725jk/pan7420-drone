/**
 * ch04_ex15_break_switch.c —— 循环里的 switch：break 与 continue 的作用域陷阱
 */
#include <stdio.h>

/**
 * 把 switch 抽成函数，是最省心的做法
 */
static int classify(char c)
{
    switch (c) {
    case 'a': return 1;
    case 'b': return 2;
    case 'x': return -1;                /* 特殊值：表示"该停止扫描" */
    default:  return 0;
    }
}

int main(void)
{
    const char *input = "a b c x d";

    printf("---- 1. switch 里的 break 只跳出 switch ----\n");
    for (const char *p = input; *p; p++) {
        switch (*p) {
        case ' ':
            break;                                              /* 只跳出 switch，循环继续 */
        case 'a':
            printf("  遇到 a\n");
            break;
        default:
            printf("  其它字符 '%c'\n", *p);
            break;
        }
    }
    printf("  循环完整跑完了 —— 说明 break 没有中断 for。\n");

    printf("---- 2. 循环里的 switch 里用 continue ----\n");
    for (const char *p = input; *p; p++) {
        switch (*p) {
        case ' ':
            continue;                                           /* 跳到**循环**下一轮，不是跳出 switch */
        case 'a':
            printf("  a 被跳过（continue）\n");
            continue;
        default:
            printf("  处理 '%c'\n", *p);
            break;                                              /* 跳出 switch，然后循环继续 */
        }
    }
    printf("  【提醒】continue 作用在循环上，即使它写在 switch 里面。\n");

    printf("---- 3. 想同时跳出 switch 和结束循环 ----\n");
    int stop = 0;
    for (const char *p = input; *p && !stop; p++) {
        switch (*p) {
        case 'x':
            printf("  遇到 x，设置停止标志\n");
            stop = 1;                                           /* 标志位 + 循环条件，两层一起退出 */
            break;                                              /* 跳出 switch */
        default:
            printf("  扫描 '%c'\n", *p);
            break;
        }
    }
    printf("  这种「标志位 + 循环条件」的写法最直白。\n");

    printf("---- 4. 抽成函数：最推荐 ----\n");
    for (const char *p = input; *p; p++) {
        int k = classify(*p);
        if (k == -1) {
            printf("  classify('%c') 返回 -1，直接 return/break 出循环\n", *p);
            break;
        }
        if (k > 0) {
            printf("  classify('%c') = %d\n", *p, k);
        }
    }
    printf("  把 switch 抽成函数之后，循环里只剩 break/continue，语义一目了然。\n");

    printf("---- 5. 对照表：关键字的作用范围 ----\n");
    printf("  break    ：跳出最近的 switch 或循环\n");
    printf("  continue ：跳到最近的**循环**的下一轮（跳过 switch 不管）\n");
    printf("  return   ：结束整个函数\n");
    printf("  goto     ：跳到同函数内的标号\n");
    return 0;
}
