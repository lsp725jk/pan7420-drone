/**
 * ch02_ex5_float.c —— 浮点类型与精度陷阱
 */
#include <stdio.h>
#include <float.h>
#include <math.h>

/**
 * 浮点比较的标准做法：相对 + 绝对容差
 */
static int nearly_equal(double a, double b)
{
    double diff = fabs(a - b);
    if (diff <= DBL_EPSILON) {                  /* 两者几乎重合 */
        return 1;
    }
    double scale = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return diff <= scale * DBL_EPSILON * 8.0;           /* 相对误差足够小 */
}

int main(void)
{
                /* ---- 1. 0.1 + 0.2 不等于 0.3 ---- */
    double x = 0.1;
    double y = 0.2;
    double z = x + y;
    printf("---- 经典陷阱：0.1 + 0.2 ----\n");
    printf("0.1        = %.20f\n", x);
    printf("0.2        = %.20f\n", y);
    printf("0.1 + 0.2  = %.20f\n", z);
    printf("0.3        = %.20f\n", 0.3);
    printf("(0.1+0.2) == 0.3 ?  %d   <-- 0 表示不相等\n", (z == 0.3));
    printf("差值        = %.20f\n", z - 0.3);

                /* ---- 2. 正确比较：容差法 ---- */
    printf("---- 正确比较 ----\n");
    printf("nearly_equal(0.1+0.2, 0.3) = %d\n", nearly_equal(z, 0.3));
    printf("直接 == 比较               = %d\n", (z == 0.3));
    float fz = 0.1f + 0.2f;
    printf("float 版本 (0.1f+0.2f)==0.3f = %d\n", (fz == 0.3f));

                /* ---- 3. float 和 double 的有效位数差别 ---- */
    printf("---- 有效位数 ----\n");
    float  f = 3.14159265358979323846f;
    double d = 3.14159265358979323846;
    printf("float  存 pi = %.10f   (第 8 位起就不对了)\n", (double)f);
    printf("double 存 pi = %.15f\n", d);
    printf("FLT_DIG=%d  DBL_DIG=%d  FLT_EPSILON=%.10e  DBL_EPSILON=%.10e\n",
           FLT_DIG, DBL_DIG, (double)FLT_EPSILON, DBL_EPSILON);

                /* ---- 4. 大数吃小数：量级差太多时小数会被"吞掉" ---- */
    printf("---- 大数吃小数 ----\n");
    float big = 1.0e8f;
    float small = 1.0f;
    printf("1.0e8f + 1.0f - 1.0e8f = %.1f   <-- 期望 1.0\n",
           (double)((big + small) - big));
    double dbig = 1.0e16;
    printf("1.0e16 + 1.0 - 1.0e16  = %.1f   <-- double 也一样会吞\n",
           (dbig + 1.0) - dbig);

                /* ---- 5. 浮点当循环计数器：累计误差 ---- */
    printf("---- 别用浮点当计数器 ----\n");
    double acc = 0.0;
    for (int k = 0; k < 10; k++) {
        acc += 0.1;                                     /* 加 10 次 0.1 */
    }
    printf("累加 10 次 0.1 = %.20f\n", acc);
    printf("等于 1.0 吗?   %d\n", (acc == 1.0));
    printf("差多少?        %.20f\n", acc - 1.0);

                /* ---- 6. 特殊值：除以 0 得到 inf，0/0 得到 nan ---- */
    printf("---- 特殊值 ----\n");
    double zero = 0.0;
    double inf  =  1.0 / zero;
    double ninf = -1.0 / zero;
    double nan  =  zero / zero;
    printf("1.0/0.0  = %f   (inf 无穷大)\n", inf);
    printf("-1.0/0.0 = %f\n", ninf);
    printf("0.0/0.0  = %f   (nan 非数字)\n", nan);
    printf("nan == nan ? %d   <-- 连自己都不等于自己，判断要用 isnan()\n",
           (nan == nan));
    printf("isnan(nan) = %d   isinf(inf) = %d   isfinite(1.0) = %d\n",
           isnan(nan) != 0, isinf(inf) != 0, isfinite(1.0) != 0);

                /* ---- 7. 转成整数是"向零截断"，不是四舍五入 ---- */
    printf("---- 浮点转整数 ----\n");
    printf("(int)3.9  = %d   (int)-3.9 = %d   (int)3.5 = %d\n",
           (int)3.9, (int)-3.9, (int)3.5);
    printf("想四舍五入要自己加 0.5 或用 round(): round(3.5)=%.0f round(-3.5)=%.0f\n",
           round(3.5), round(-3.5));
    printf("floor(3.9)=%.0f ceil(3.1)=%.0f trunc(-3.9)=%.0f\n",
           floor(3.9), ceil(3.1), trunc(-3.9));
    return 0;
}
