/**
 * ch04_ex12_fallthrough.c —— switch 的贯穿：什么时候该用，怎么防漏 break
 */
#include <stdio.h>

/**
 * ---- 1. 故意贯穿：把多个 case 归到同一段处理 ----
 */
static int is_whitespace(char c)
{
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        return 1;                                       /* 4 个 case 共用这一个 return */
    default:
        return 0;
    }
}

/* ---- 2. 故意贯穿：逐级累加（这种写法要显式注释）---- 顺序是 case 3 -> case 2 -> case 1，等级越高累加越多。 */
static int level_to_threshold(int level)
{
    int base = 0;
    switch (level) {
    case 3:
        base += 300;                                    /* 故意贯穿：level 3 包含 2 和 1 的份额 */
                                /* fallthrough */
    case 2:
        base += 200;
                                /* fallthrough */
    case 1:
        base += 100;
        break;
    default:
        base = -1;                                      /* 非法等级 */
        break;
    }
    return base;
}

/* ---- 3. 顺序写反：本来想"逐级累加"，结果把顺序倒过来写成"逐级相减" ---- */
static int level_to_threshold_buggy(int level)
{
    int base = 0;
    switch (level) {
    case 1:
        base += 100;                                    /* 正确写法应该是 300，这里写反了 */
                                /* fallthrough */
    case 2:
        base += 200;                                    /* 正确应该是 200 */
                                /* fallthrough */
    case 3:
        base += 300;                                    /* 正确应该是 100 */
        break;
    default:
        base = -1;
        break;
    }
    return base;
}

/* ---- 4. 更稳的写法：先归一化，再查表，不给自己漏 break 的机会 ---- */static int level_to_threshold_table(int level)
{
    static const int table[4] = { 0, 100, 300, 600 };           /* 累加后的结果直接写死 */
    if (level < 0 || level > 3) {
        return -1;
    }
    return table[level];
}

int main(void)
{
    printf("---- 1. 故意贯穿：多个 case 归并 ----\n");
    const char *samples = "a \tZ\n";
    const char *labels[5] = { "a", "空格", "\\t", "Z", "\\n" };
    int idx = 0;
    for (const char *p = samples; *p; p++) {
        printf("  '%s' 是空白字符吗? %d\n",
               labels[(idx < 5) ? idx : 4], is_whitespace(*p));
        idx++;
    }
    printf("  四个 case 写在一起、共用一个 return，这是标准写法。\n");

    printf("---- 2. 故意贯穿：逐级累加 ----\n");
    for (int lv = 0; lv <= 4; lv++) {
        printf("  level %d -> 正确版本 %4d   顺序写反版本 %4d   查表版本 %4d\n",
               lv, level_to_threshold(lv),
               level_to_threshold_buggy(lv), level_to_threshold_table(lv));
    }
    printf("  正确版本：3->600, 2->300, 1->100（等级越高总量越大）。\n");
    printf("  顺序写反：3->300, 2->500, 1->600 —— 等级越高总量反而越小，\n");
    printf("            这就是\"贯穿顺序写反\"的典型症状：数值单调性反了。\n");

    printf("---- 3. 贯穿顺序写反为什么难查 ----\n");
    printf("  · 编译器不报警（它无法区分\"故意\"和\"写错顺序\"）；\n");
    printf("  · 每个单独的 case 都\"看起来对\"，只有连起来跑才错；\n");
    printf("  · 症状是数值关系反了（等级越高总量越小），容易被当成标定问题。\n");
    printf("  对策：1) 每个 case 都以 break/return/continue 结束；\n");
    printf("        2) 真要贯穿就写一行 fallthrough 注释，并把顺序写清楚；\n");
    printf("        3) 能用查表就用查表（table 版本永远不会漏 break）。\n");

    printf("---- 4. 查表版本的可维护性 ----\n");
    printf("  table[] 把\"当前等级对应的总量\"直接写出来：\n");
    printf("    level 0 -> 0, 1 -> 100, 2 -> 300, 3 -> 600\n");
    printf("  加一个 level 4 只需要在数组里补一个数，\n");
    printf("  而 switch 版本要在每个 case 里重新推导累加关系 —— 容易改错。\n");
    printf("  【嵌入式场景】这种\"等级->阈值/占空比/波特率\"的映射，\n");
    printf("  几乎都应该写成 const 数组。\n");
    return 0;
}
