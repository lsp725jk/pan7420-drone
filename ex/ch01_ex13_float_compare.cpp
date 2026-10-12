/**
 * ch01_ex13_float_compare.c —— 浮点比较：什么时候能用 ==，什么时候不能
 */
#include <stdio.h>
#include <math.h>
#include <float.h>

/**
 * 只用绝对容差：量级很大时失效
 */
static int eq_abs(double a, double b)
{
    return fabs(a - b) < 1e-9;
}

/**
 * 相对 + 绝对容差：通用做法
 */
static int eq_rel(double a, double b)
{
    double diff = fabs(a - b);
    if (diff <= DBL_EPSILON) {
        return 1;
    }
    double scale = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return diff <= scale * DBL_EPSILON * 8.0;
}

int main(void)
{
    printf("---- 1. 什么时候 == 是安全的 ----\n");
    double x = 0.1;
    double y = x;                               /* 直接复制：比特完全相同 */
    printf("  double y = x;  y == x 吗? %d   <-- 安全（同一批比特）\n", y == x);
    double z = 0.0;
    printf("  与 0.0 比较：z == 0.0 吗? %d   <-- 安全（0.0 可精确表示）\n",
           z == 0.0);
    printf("  整数赋值：1.0 == 1 吗? %d      <-- 安全（小整数可精确表示）\n",
           1.0 == 1);
    printf("  【结论】不涉及运算、或者参与运算的都是可精确表示的值时，== 没问题。\n");

    printf("---- 2. 什么时候 == 不安全 ----\n");
    double a = 0.1 + 0.2;
    double b = 0.3;
    printf("  0.1+0.2 == 0.3 吗? %d\n", a == b);
    printf("    a = %.20f\n", a);
    printf("    b = %.20f\n", b);
    printf("    差 = %.20e\n", a - b);
    printf("  这就是「运算后不能用 == 」的典型。\n");

    printf("---- 3. 两种容差策略对比 ----\n");
                /* 量级很大时，绝对容差会误判 */
    double big1 = 1.0e12 + 1.0;                 /* double 在 1e12 附近的 ULP 约 0.000122 */
    double big2 = 1.0e12;
    printf("  big1 = %.1f, big2 = %.1f\n", big1, big2);
    printf("    |big1-big2| = %.6f\n", fabs(big1 - big2));
    printf("    eq_abs(big1,big2) = %d   <-- 绝对容差说不等\n",
           eq_abs(big1, big2));
    printf("    eq_rel(big1,big2) = %d   <-- 相对容差也说不等（因为确实大得多）\n",
           eq_rel(big1, big2));
    printf("    big1 == big2 ? %d\n", big1 == big2);

                /* 量级很小的情况 */
    double small1 = 1.0e-12;
    double small2 = 1.0e-12 + 1.0e-20;
    printf("  small1 = %.20e, small2 = %.20e\n", small1, small2);
    printf("    eq_abs = %d, eq_rel = %d, == = %d\n",
           eq_abs(small1, small2), eq_rel(small1, small2), small1 == small2);

    printf("---- 4. 实用建议：按业务选容差 ----\n");
    printf("  a) 角度：1e-6 弧度足够（约 0.00006 度）；\n");
    printf("  b) 电压：1e-4 V 足够（ADC 分辨率远大于此）；\n");
    printf("  c) 时间：1e-6 s；\n");
    printf("  d) 通用场景：用 eq_rel 那种「相对 + 绝对」的组合。\n");
    printf("  e) 千万别用 == 去判断传感器读数有没有变化 —— 用阈值！\n");
    double last = 3.3000001, now = 3.3000002;
    printf("     例：last=%.7f now=%.7f，用 == 判断变化会一直为真，\n", last, now);
    printf("     应该写 if (fabs(now - last) > 0.001) 才算变了。\n");
    return 0;
}
