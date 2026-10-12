/**
 * ch09_ex15.c —— math.h：常用数学函数、浮点比较，以及"链接 -lm"的问题
 */
#include <stdio.h>
#include <math.h>

int main(void)
{
                /**
         * 1) 绝对值 / 取整家族：注意 fabs 是浮点版，abs 是整数版（在 stdlib.h）
         */
    double x = -3.7;
    printf("x = %.2f\n", x);
    printf("fabs(x)  = %-8.3f 绝对值\n", fabs(x));
    printf("floor(x) = %-8.3f 向下取整\n", floor(x));
    printf("ceil(x)  = %-8.3f 向上取整\n", ceil(x));
    printf("trunc(x) = %-8.3f 截断取整\n", trunc(x));
    printf("round(x) = %-8.3f 四舍五入（远离 0）\n", round(x));

                /* 2) 幂与根 */
    printf("\nsqrt(2)      = %.10f\n", sqrt(2.0));
    printf("pow(2, 10)   = %.1f\n", pow(2.0, 10.0));
    printf("hypot(3, 4)  = %.1f  （= sqrt(3^2+4^2)，比手写更不易溢出）\n",
           hypot(3.0, 4.0));
    printf("fmod(7.5, 2) = %.1f  （浮点取余；整数用 %% ）\n", fmod(7.5, 2.0));

                /* 3) 三角函数用的是【弧度】，不是角度 —— 这是最常见的错 */
    const double pi = 3.14159265358979323846;
    printf("\nsin(30°)=sin(pi/6) = %.6f\n", sin(pi / 6));
    printf("cos(60°)=cos(pi/3) = %.6f\n", cos(pi / 3));
    printf("atan2(1,1) 转角度   = %.1f°  （atan2 能分辨象限，atan 不能）\n",
           atan2(1.0, 1.0) * 180.0 / pi);

                /* 4) 浮点不能用 == 比较。正确做法：看差值是否小于一个容差 */
    double a = 0.1 + 0.2;
    double b = 0.3;
    printf("\n0.1 + 0.2 = %.20f\n", a);
    printf("0.3       = %.20f\n", b);
    printf("a == b          -> %s  （错）\n", (a == b) ? "相等" : "不相等");
    printf("fabs(a-b) < 1e-12 -> %s  （对）\n",
           (fabs(a - b) < 1e-12) ? "相等" : "不相等");

                /* 5) 特殊值：NAN / INFINITY 与判断它们 */
    double zero = 0.0;
    double inf  = 1.0 / zero;
    double nan  = zero / zero;
    printf("\n1.0/0.0 = %f , isnan=%d , isinf=%d\n", inf, isnan(inf), isinf(inf));
    printf("0.0/0.0 = %f , isnan=%d   （NaN 和任何数比较都为假，包括它自己）\n",
           nan, isnan(nan));
    printf("nan == nan -> %s\n", (nan == nan) ? "真" : "假");
    printf("sqrt(-1) 也返回 NaN 并且置 errno（见 9.10 节）\n");

        /* 6) 变式：嵌入式里没有 FPU 时（Cortex-M0+ 就没有）， 这些函数会走"软件浮点库"，一次 sin 可能要几百个周期。 飞控里常用查表 + 线性插值代替，见第 10 章。 */
    return 0;
}
