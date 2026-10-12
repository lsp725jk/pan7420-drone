/**
 * ch02_ex32_overflow_add.c —— 溢出检测：先判断再算（不制造 UB）
 */
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * 有符号加法：先判断再算（唯一完全安全的写法）
 */
static bool add_i32_checked(int32_t a, int32_t b, int32_t *out)
{
    if (b > 0 && a > INT32_MAX - b) { return false; }           /* 正溢出 */
    if (b < 0 && a < INT32_MIN - b) { return false; }           /* 负溢出 */
    *out = a + b;
    return true;
}

/* 无符号加法：回绕是定义好的，所以"结果 < 任一加数"就说明回绕了 */
static bool add_u32_carry(uint32_t a, uint32_t b, uint32_t *out)
{
    uint32_t r = a + b;                         /* 有定义：模 2^32 */
    *out = r;
    return (r < a);                             /* 有进位 = 回绕了 */
}

/* 累加器：每步都检查，绝不"先算再看" */
static int accumulate_checked(const int32_t *v, int n, int32_t *out)
{
    int32_t acc = 0;
    for (int i = 0; i < n; i++) {
        if (!add_i32_checked(acc, v[i], &acc)) {
            return -1;                                          /* 报告失败，acc 保持"上一步"的值 */
        }
    }
    *out = acc;
    return 0;
}

int main(void)
{
    printf("---- 1. 有符号溢出检测 ----\n");
    int32_t tests[5][2] = {
        { 100, 200 }, { INT32_MAX, 0 }, { INT32_MAX, 1 },
        { INT32_MIN, -1 }, { INT32_MIN, 1 }
    };
    for (int i = 0; i < 5; i++) {
        int32_t r = 0;
        if (add_i32_checked(tests[i][0], tests[i][1], &r)) {
            printf("  %11d + %11d = %11d\n", tests[i][0], tests[i][1], r);
        } else {
            printf("  %11d + %11d = 会溢出，已拒绝\n", tests[i][0], tests[i][1]);
        }
    }

    printf("---- 2. 无符号加法的进位检测 ----\n");
    uint32_t u[4][2] = {
        { 1u, 2u }, { 0xFFFFFFFFu, 1u }, { 0xFFFFFFFEu, 1u }, { 100u, 200u }
    };
    for (int i = 0; i < 4; i++) {
        uint32_t r = 0;
        bool carry = add_u32_carry(u[i][0], u[i][1], &r);
        printf("  0x%08X + 0x%08X = 0x%08X  %s\n",
               u[i][0], u[i][1], r, carry ? "（有进位/回绕）" : "");
    }
    printf("  无符号的回绕是标准保证的，所以「结果比任一加数小」就是进位的可靠信号。\n");

    printf("---- 3. 累加器：每步检查 ----\n");
    int32_t good[3] = { 1000000, 2000000, 3000000 };
    int32_t bad[3]  = { 1000000, 2000000, INT32_MAX };
    int32_t acc = 0;
    printf("  正常数据: rc=%d acc=%d\n", accumulate_checked(good, 3, &acc), acc);
    printf("  溢出数据: rc=%d  <-- 返回 -1，调用者知道这次累加不可信\n",
           accumulate_checked(bad, 3, &acc));

    printf("---- 4. 为什么不能先算再看 ----\n");
    printf("  错误写法：\n");
    printf("      int r = a + b;                  // 这一步已经 UB\n");
    printf("      if (r < a) { /* 溢出 */ }        // 编译器可以假设它不成立\n");
    printf("  正确写法：\n");
    printf("      if (b > 0 && a > INT32_MAX - b) { /* 溢出 */ }  // 先判断\n");
    printf("  区别在于：正确写法里**没有任何一步**会产生 UB。\n");
    return 0;
}
