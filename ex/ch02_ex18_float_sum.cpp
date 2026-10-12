/**
 * ch02_ex18_float_sum.c —— 浮点累加误差：三种求和方式对比
 */
#include <stdio.h>
#include <math.h>

/**
 * 朴素累加：小学加法的直接翻译，误差会累积
 */
static double sum_naive(const double *v, int n)
{
    double s = 0.0;
    for (int i = 0; i < n; i++) {
        s += v[i];
    }
    return s;
}

/**
 * Kahan 补偿求和：用 c 记住"被丢掉的那部分"，下一轮补回去
 */
static double sum_kahan(const double *v, int n)
{
    double s = 0.0;
    double c = 0.0;                         /* 补偿量（compensation） */
    for (int i = 0; i < n; i++) {
        double y = v[i] - c;                        /* 把上次丢掉的补进来 */
        double t = s + y;                           /* 这一步会丢精度 */
        c = (t - s) - y;                            /* 精确算出这一步丢了多少 */
        s = t;
    }
    return s;
}

/* 定点累加：把 0.1 放大 10 倍变成整数 1，最后再除回去 */
static double sum_fixed_point(const double *v, int n)
{
    long long acc = 0;
    for (int i = 0; i < n; i++) {
        acc += (long long)llround(v[i] * 10.0);                     /* 单位 0.1 */
    }
    return (double)acc / 10.0;
}

int main(void)
{
                /* ---- 1. 加 10000 次 0.1 ---- */
    enum { N = 10000 };
    static double v[N];                         /* 放静态区，避免栈上开 80KB */
    for (int i = 0; i < N; i++) {
        v[i] = 0.1;
    }

    double exact = 1000.0;
    double a = sum_naive(v, N);
    double b = sum_kahan(v, N);
    double c = sum_fixed_point(v, N);

    printf("---- 1. 累加 %d 次 0.1（正确值 1000.0）----\n", N);
    printf("朴素累加   = %.20f\n", a);
    printf("Kahan 求和 = %.20f\n", b);
    printf("定点累加   = %.20f\n", c);
    printf("朴素误差   = %.20e\n", a - exact);
    printf("Kahan 误差 = %.20e\n", b - exact);
    printf("定点误差   = %.20e\n", c - exact);
    printf("结论：Kahan 把误差压小了好几个数量级；定点累加直接是精确的。\n");

                /* ---- 2. 误差随项数增长 ---- */
    printf("---- 2. 误差随项数增长 ----\n");
    printf("  项数     朴素误差            Kahan 误差\n");
    for (int n = 100; n <= 10000; n *= 10) {
        printf("  %5d   %+.6e      %+.6e\n",
               n, sum_naive(v, n) - n * 0.1, sum_kahan(v, n) - n * 0.1);
    }
    printf("  朴素累加的误差大致按项数线性增长；Kahan 基本不增长。\n");

                /* ---- 3. 大数吃小数：换个数据就崩 ---- */
    printf("---- 3. 大数吃小数：1e16 + 1 == 1e16 ----\n");
    double big = 1.0e16;
    printf("  1e16        = %.17g\n", big);
    printf("  1e16 + 1    = %.17g   <-- 1 被完全吞掉！\n", big + 1.0);
    printf("  1e16 + 1 - 1e16 = %.17g   <-- 期望 1，实际 0\n", big + 1.0 - big);
    printf("  原因：double 的尾数 52 位，能精确表示的最大连续整数是 2^53。\n");
    printf("        1e16 已经很大，它的步长(ULP)是 2，比 2 小的增量进不去。\n");
    double bigf = 1.0e8f;
    printf("  单精度：float 的 ULP 在 1e8 附近是 8，所以小数加进去会被量化：\n");
    printf("        1e8f + 1 = %.9g\n", (double)(bigf + 1.0f));
    printf("        1e8f + 1 + 1 = %.9g\n", (double)(bigf + 1.0f + 1.0f));
    printf("        而 (1e8f + 1) == 1e8f 吗? %d\n",
           ((bigf + 1.0f) == bigf) ? 1 : 0);
    printf("  【嵌入式对策】姿态解算里先减去均值/去偏置再累加，\n");
    printf("  或者干脆全程用定点数（把角度按 0.01 度存成整数）。\n");

                /* ---- 4. 什么时候值得用 Kahan ---- */
    printf("---- 4. 实践建议 ----\n");
    printf("  a) 项数少（<100）且量级接近：朴素累加足够，别过度设计；\n");
    printf("  b) 项数多、量级差大（传感器长时间积分、统计）：用 Kahan；\n");
    printf("  c) 单片机（无 FPU）上：定点数 > float + Kahan > double；\n");
    printf("     Kahan 每次迭代多 4 次浮点运算，在 Cortex-M0+ 上不便宜。\n");
    return 0;
}
