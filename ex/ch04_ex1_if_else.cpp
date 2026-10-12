/**
 * ch04_ex1_if_else.c —— if / else：分支怎么写才不容易错
 */
#include <stdio.h>

/**
 * 把一个区间判定写成函数，比在主函数里堆 if 清楚
 */
static const char *level_text(int percent)
{
    if (percent < 0) {
        return "非法";
    } else if (percent < 20) {
        return "极低";
    } else if (percent < 50) {
        return "偏低";
    } else if (percent < 80) {
        return "正常";
    } else if (percent <= 100) {
        return "充足";
    } else {
        return "非法";
    }
}

int main(void)
{
                /* ---- 1. 最基本的形式 ---- */
    printf("---- 1. 基本 if ----\n");
    int battery = 35;
    if (battery < 50) {
        printf("battery=%d 偏低，该提醒了\n", battery);
    } else {
        printf("battery=%d 还行\n", battery);
    }

                /* ---- 2. else if 链：顺序很重要 ---- */
    printf("---- 2. else if 链 ----\n");
    int samples[7] = { -5, 0, 19, 49, 79, 100, 120 };
    for (int k = 0; k < 7; k++) {
        printf("  %4d%% -> %s\n", samples[k], level_text(samples[k]));
    }
    printf("【为什么顺序重要】逻辑是从上往下，第一个成立的说了算。\n");
                /* 反例：如果把 percent < 80 写在最前面，那 30 也会被判成"正常" */
    printf("  如果把 <80 的判断提到最前，30%% 也会被误判成正常。\n");

                /* ---- 3. 条件里的真假 ---- */
    printf("---- 3. 什么算真 ----\n");
    int flags[5] = { 0, 1, -1, 100, 0 };
    for (int k = 0; k < 5; k++) {
        if (flags[k]) {
            printf("  if (%4d) 成立\n", flags[k]);
        } else {
            printf("  if (%4d) 不成立\n", flags[k]);
        }
    }
    printf("只有 0 是假，其它都真。指针也一样：if (p) 等价于 if (p != NULL)。\n");

                /* ---- 4. 用括号和大括号防错 ---- */
    printf("---- 4. 大括号的必要性 ----\n");
    int x = 5;
    if (x > 0)
        printf("  只有这一条在 if 里\n");
    printf("  这一条永远执行（它在 if 外面）\n");
    if (x > 0) {
        printf("  加上大括号后，一眼就知道范围在哪\n");
    }
    printf("【症状】漏大括号时，if 只罩住紧随其后的那一**条**语句。\n");

                /* ---- 5. 三目 vs if-else 的选择 ---- */
    printf("---- 5. if-else 还是三目 ----\n");
    int mode = 2;
                /* 只取值：三目更紧凑 */
    const char *m1 = (mode == 2) ? "手动" : "自动";
                /* 有副作用或多条语句：必须 if-else */
    const char *m2;
    if (mode == 2) {
        m2 = "手动";
        printf("  （顺带做了别的事：记录一次切换）\n");
    } else {
        m2 = "自动";
    }
    printf("  三目: %s     if-else: %s\n", m1, m2);

                /* ---- 6. 悬挂 else（dangling else）---- */
    printf("---- 6. 悬挂 else ----\n");
    int a = 1, b = 0;
                /* C 的规则：else 总是配对**最近的那个还没配对的 if**（就近原则）。 所以下面这段的 else 属于内层 if，不是外层。 */
    if (a > 0)
        if (b > 0)
            printf("  两个都大于 0\n");
        else
            printf("  a>0 但 b<=0（这个 else 配给了内层 if）\n");
    printf("  想表达 a<=0 时做别的事，必须给外层加括号：\n");
    if (a > 0) {
        if (b > 0) {
            printf("  两个都大于 0\n");
        }
    } else {
        printf("  a<=0\n");
    }
    printf("【症状】以为 else 属于外层 if，程序行为却按内层走，\n");
    printf("        尤其当内层 if 藏在宏或者续行里时最难发现。\n");

                /* ---- 7. 把 = 写成 == 的坑 ---- */
    printf("---- 7. = 和 == ----\n");
    int v = 3;
        /* if (v = 0) { ... }    <-- 把 v 赋成 0，条件恒假，而且 v 被改了。 MSVC /W4 会报 warning C4706，看到就当错误改。 防手滑写法：把常量放左边 —— if (0 == v)，写成 = 会直接编译错误。 */
    if (0 == v) {
        printf("  恒假分支（这里只是示范安全写法）\n");
    } else {
        printf("  0 == v 不成立，v = %d（v 没被改）\n", v);
    }

                /* ---- 8. 浮点条件别用 == ---- */
    printf("---- 8. 浮点别用 == ----\n");
    double measured = 1.0 / 3.0 * 3.0;
    if (measured == 1.0) {
        printf("  恰好等于 1.0（侥幸）\n");
    } else {
        printf("  不等于 1.0，实际是 %.20f\n", measured);
    }
    if (measured > 1.0 - 1e-9 && measured < 1.0 + 1e-9) {
        printf("  用容差判断：在 1.0 附近，认为相等\n");
    }
    return 0;
}
