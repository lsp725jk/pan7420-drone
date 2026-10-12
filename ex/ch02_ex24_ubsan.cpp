/**
 * ch02_ex24_ubsan.c —— 手工"未定义行为哨兵"：不能自动检测 UB 时怎么办
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <assert.h>

/**
 * ---- 手段一：用变体类型（variant）代替 UB 的结果 ----
 */
typedef struct {
    int ok;                         /* 1 = 成功，0 = 溢出 */
    int value;
} checked_int_t;

static checked_int_t checked_add(int a, int b)
{
    checked_int_t r = { 0, 0 };
    long long w = (long long)a + (long long)b;
    if (w > INT_MAX || w < INT_MIN) {
        return r;                                   /* ok = 0，调用者必须检查 */
    }
    r.ok = 1;
    r.value = (int)w;
    return r;
}

/* ---- 手段二：用断言把"假设"写在代码里 ---- */
static int div_checked(int a, int b)
{
                /* 除数为 0 是 UB（在 x86 上会触发硬件异常直接崩）。 */
    assert(b != 0);                         /* Debug 版本会拦住；Release 版本自动去掉 */
    return a / b;
}

/* ---- 手段三：把危险操作集中在少数几个函数里，并全部带 out 参数报错 ---- */
static int safe_mul_i32(int32_t a, int32_t b, int32_t *out)
{
    long long w = (long long)a * (long long)b;
    if (w > INT32_MAX || w < INT32_MIN) {
        return -1;                                  /* 出错时返回值报错，不写 out */
    }
    *out = (int32_t)w;
    return 0;
}

int main(void)
{
    printf("---- 1. 变体类型：把可能失败写进类型 ----\n");
    int cases[4][2] = { { 100, 200 }, { INT_MAX, 1 }, { INT_MIN, -1 },
                        { INT_MAX, -1 } };
    for (int i = 0; i < 4; i++) {
        checked_int_t r = checked_add(cases[i][0], cases[i][1]);
        if (r.ok) {
            printf("  %11d + %-11d = %11d\n", cases[i][0], cases[i][1], r.value);
        } else {
            printf("  %11d + %-11d = 溢出（调用者必须处理这个分支）\n",
                   cases[i][0], cases[i][1]);
        }
    }

    printf("---- 2. 断言：把假设写成可执行的代码 ----\n");
    printf("  div_checked(100, 7) = %d\n", div_checked(100, 7));
    printf("  div_checked(100, 0) 会触发 assert（Debug 版），这里不调用它。\n");
    printf("  实测 assert 在本机 /Od 默认生效；/DNDEBUG 时会被去掉。\n");

    printf("---- 3. 集中的安全算术 ----\n");
    int32_t pairs[3][2] = { { 100000, 100000 }, { 46341, 46341 }, { 2, 3 } };
    for (int i = 0; i < 3; i++) {
        int32_t out = 0;
        if (safe_mul_i32(pairs[i][0], pairs[i][1], &out) == 0) {
            printf("  %7d * %-7d = %d\n", pairs[i][0], pairs[i][1], out);
        } else {
            printf("  %7d * %-7d = 溢出，已拒绝\n", pairs[i][0], pairs[i][1]);
        }
    }

    printf("---- 4. 在 MSVC 上还能做什么 ----\n");
    printf("  a) 把警告当错误：cl /W4 /WX —— 溢出、截断、未初始化很多都能被抓到；\n");
    printf("  b) 打开运行期检查：cl /RTC1（Debug 常用，检查未初始化变量和栈损坏）；\n");
    printf("  c) 静态分析：cl /analyze（MSVC 自带的分析器，会更慢但能发现更多问题）；\n");
    printf("  d) 代码层面：所有可能失败的计算都用 out 参数 + 返回码表达。\n");
    printf("  【未实测】/analyze 的具体输出、以及 UBSan 在其它编译器上的表现，\n");
    printf("           本教程没有验证过，只作为方向建议。\n");
    return 0;
}
