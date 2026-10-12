/**
 * ch02_ex17_overflow_check.c —— 加法/乘法溢出的三种检测手段（不制造 UB）
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * 手段一：先判断再算（加法）。这是唯一"完全安全"的写法。
 */
static bool add_overflow_int(int a, int b, int *out)
{
    if (b > 0 && a > INT_MAX - b) { return false; }           /* 正溢出 */
    if (b < 0 && a < INT_MIN - b) { return false; }           /* 负溢出 */
    *out = a + b;
    return true;
}

/* 手段二：用更宽的类型算，再判断能不能放回来 */
static bool add_via_wide(int a, int b, int *out)
{
    long long w = (long long)a + (long long)b;
    if (w > INT_MAX || w < INT_MIN) { return false; }
    *out = (int)w;
    return true;
}

/* 手段三：乘法溢出用"除法反推"检测 */
static bool mul_overflow_int(int a, int b, int *out)
{
    if (a == 0 || b == 0) { *out = 0; return true; }
    long long w = (long long)a * (long long)b;           /* 64 位乘 32 位不会溢出 */
    if (w > INT_MAX || w < INT_MIN) { return false; }
    *out = (int)w;
    return true;
}

/* 把溢出情况说清楚，而不是默默算错 */
static void show_add(int a, int b)
{
    int r = 0;
    if (add_overflow_int(a, b, &r)) {
        printf("  %11d + %11d = %11d  (OK)\n", a, b, r);
    } else {
        printf("  %11d + %11d = 溢出，已拒绝（这才是正确行为）\n", a, b);
    }
}

int main(void)
{
    printf("---- 1. 加法溢出检测（先判断再算）----\n");
    show_add(100, 200);
    show_add(INT_MAX, 0);
    show_add(INT_MAX, 1);
    show_add(INT_MIN, -1);
    show_add(-5, -7);

    printf("---- 2. 两种手段结果一致 ----\n");
    int vals[4][2] = {
        { INT_MAX, 1 }, { INT_MAX, 0 }, { INT_MIN, -1 }, { 12345, 6789 }
    };
    int all_same = 1;
    for (int i = 0; i < 4; i++) {
        int r1 = 0, r2 = 0;
        bool ok1 = add_overflow_int(vals[i][0], vals[i][1], &r1);
        bool ok2 = add_via_wide(vals[i][0], vals[i][1], &r2);
                                /* 两者都拒绝时，"结果值"根本没被写过，所以只比较"是否允许相加" */
        int same = (ok1 == ok2) && (!ok1 || r1 == r2);
        if (!same) { all_same = 0; }
        printf("  a=%11d b=%11d : 手段一 %s, 手段二 %s, 判定 %s\n",
               vals[i][0], vals[i][1],
               ok1 ? "OK" : "溢出", ok2 ? "OK" : "溢出",
               same ? "一致" : "不一致");
    }
    printf("  两种手段在本机得到完全一致的结果：%s\n", all_same ? "是" : "否");

    printf("---- 3. 乘法溢出检测 ----\n");
    int pairs[4][2] = { { 1000, 1000 }, { 100000, 100000 }, { 46341, 46341 },
                        { -50000, 50000 } };
    for (int i = 0; i < 4; i++) {
        int r = 0;
        if (mul_overflow_int(pairs[i][0], pairs[i][1], &r)) {
            printf("  %6d * %6d = %11d  (OK)\n", pairs[i][0], pairs[i][1], r);
        } else {
            printf("  %6d * %6d = 溢出，已拒绝\n", pairs[i][0], pairs[i][1]);
        }
    }
    printf("  注意 46341*46341：因为 46341 > sqrt(2^31)，正好越过 int 上限。\n");

    printf("---- 4. 嵌入式里的实践建议 ----\n");
    printf("  a) ADC 累加、积分项、位置累加这类会持续增长的量，\n");
    printf("     一律用 int64_t 累加，需要时再限幅回目标类型；\n");
    printf("  b) 判断能不能放回目标类型：if (w > INT32_MAX || w < INT32_MIN)；\n");
    printf("  c) 别写 if (a + b < a) 这种所谓溢出检测：\n");
    printf("     a+b 本身已经 UB，编译器可能把它优化成恒假。\n");
    printf("  d) C23 起有 <stdckdint.h> 的 ckd_add/ckd_sub/ckd_mul，\n");
    printf("     本机支持情况未实测。\n");
    return 0;
}
