/**
 * ch10_ex09.c —— 浮点的坑：没有 FPU 时的三个现实问题
 */
#include <stdio.h>
#include <float.h>
#include <math.h>

static float  g_fsum  = 0.0f;
static double g_dsum  = 0.0;

int main(void)
{
                /**
         * ---------- 1) float 的有效位数只有 6~7 位十进制 ----------
         */
    printf("FLT_DIG = %d , DBL_DIG = %d\n", FLT_DIG, DBL_DIG);
    float  f = 16777216.0f;                      /* 2^24 */
    printf("2^24      的 float = %.1f\n", f);
    printf("2^24 + 1  的 float = %.1f  <-- 加不上去！\n", f + 1.0f);
    printf("float 只有 24 位有效位（含隐含位），超过就丢了\n\n");

                /* ---------- 2) 累加误差：控制周期里最常见的坑 ---------- */
    printf("累加 100000 次 0.1f 与 0.1（float vs double）：\n");
    for (int i = 0; i < 100000; i++) { g_fsum += 0.1f; g_dsum += 0.1; }
    printf("  float  累加 = %.6f ，误差 = %.6f\n", (double)g_fsum, (double)g_fsum - 10000.0);
    printf("  double 累加 = %.6f ，误差 = %.6f\n", g_dsum, g_dsum - 10000.0);
    printf("  积分项（角度积分、高度积分）用 float 长期累加一定会漂\n\n");

                /* ---------- 3) 大数吃小数：飞控里的实际表现 ---------- */
    float base = 100000.0f;
    float tiny = 0.001f;
    printf("100000.0f + 0.001f = %.4f （应该多 0.001，实际没变）\n", base + tiny);
    printf("  -> 传感器零偏只有 0.001 却被大数吃掉，这就是「校准没效果」的一种原因\n\n");

                /* ---------- 4) 正确做法：比较用容差，累加用小量 ---------- */
    double x = 0.1 + 0.2, y = 0.3;
    printf("浮点比较：\n");
    printf("  x == y              -> %s\n", (x == y) ? "相等" : "不相等（错！）");
    printf("  fabs(x-y) < 1e-12   -> %s\n",
           (fabs(x - y) < 1e-12) ? "相等（对）" : "不相等");
    double tol = DBL_EPSILON * fabs(y) * 8;
    printf("  按 DBL_EPSILON 算出的容差 = %.3e\n\n", tol);

                /* ---------- 5) 变式：省 CPU 的三个办法 ---------- */
    printf("省 CPU 的三个办法：\n");
    printf("  · 把「每次都要算」的常数在编译期算好（比如 1/1000.0 写成 0.001）；\n");
    printf("  · 三角函数查表 + 线性插值（sin/cos 在 M0+ 上要几百周期）；\n");
    printf("  · 能整除 2 的幂就别乘：*0.5 写成 /2 或直接改指数位；\n");
    printf("    更进一步就把整条控制律改成定点数的乘加（见 ch10_ex08.c）。\n");
    return 0;
}
