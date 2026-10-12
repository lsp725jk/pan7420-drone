/**
 * ch03_ex11_shift_edge.c —— 移位的三种边界情况
 */
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

/**
 * 循环左移：最容易踩"移位量为 0"的那个坑
 */
static uint32_t rotl32_bad(uint32_t x, int n)
{
                /**
         * n == 0 时 x >> 32 是 UB；本机实测可能碰巧对，但不保证
         */
    return (x << n) | (x >> (32 - n));
}

static uint32_t rotl32_ok(uint32_t x, int n)
{
    if (n == 0) {
        return x;                                   /* 必须先拦掉这个边界 */
    }
    n &= 31;                                /* 顺便把范围收到 0..31 */
    return (x << n) | (x >> (32 - n));
}

/* 安全左移：移位量越界时返回 0（而不是 UB） */
static uint32_t shl_safe(uint32_t x, int n)
{
    if (n < 0 || n >= 32) {
        return 0;                                   /* 由调用者决定怎么处理越界 */
    }
    return x << n;
}

int main(void)
{
                /* ---- 1. 循环移位的 n == 0 边界 ---- */
    printf("---- 1. 循环移位的边界 ----\n");
    uint32_t v = 0x80000001u;
    printf("v = 0x%08X\n", v);
    printf("rotl32_bad(v, 0) = 0x%08X   <-- 里面有 x>>32，是 UB（本机碰巧对）\n",
           rotl32_bad(v, 0));
    printf("rotl32_ok (v, 0) = 0x%08X   <-- 显式返回 x，安全\n",
           rotl32_ok(v, 0));
    printf("rotl32_ok (v, 1) = 0x%08X\n", rotl32_ok(v, 1));
    printf("rotl32_ok (v, 4) = 0x%08X\n", rotl32_ok(v, 4));
    printf("rotl32_ok (v,31) = 0x%08X\n", rotl32_ok(v, 31));

                /* ---- 2. 右移和除法在负数上不一样 ---- */
    printf("---- 2. 负数：>> 是向下取整，/ 是向零取整 ----\n");
    int vals[4] = { -7, -8, -1, 7 };
    for (int i = 0; i < 4; i++) {
        int x = vals[i];
        printf("  %3d: x/2 = %3d   x>>1 = %3d   %s\n",
               x, x / 2, x >> 1,
               (x / 2 == (x >> 1)) ? "一致" : "**不一致**");
    }
    printf("  【结论】别用 >> 代替有符号除法。\n");
    printf("  【注】对有符号负数右移是实现定义（本机是算术右移）。\n");

                /* ---- 3. 移位量来自变量：必须有范围检查 ---- */
    printf("---- 3. 变量移位量 ----\n");
    int amounts[5] = { 0, 1, 31, 32, -1 };
    for (int i = 0; i < 5; i++) {
        int n = amounts[i];
                                /* 危险版本：shl_safe 里做了检查，所以这里安全地看到结果 */
        printf("  1u << %2d = 0x%08X", n, shl_safe(1u, n));
        if (n < 0 || n >= 32) {
            printf("   <-- 越界！真实代码里这里必须是 UB 或显式处理");
        }
        printf("\n");
    }
    printf("  这就是那个安全包装的意义：把 UB 变成有定义的返回值。\n");

                /* ---- 4. 掩码构造的推荐写法 ---- */
    printf("---- 4. 掩码构造 ----\n");
    printf("  低 8 位     : 0x%08X   (0xFFu)\n", 0xFFu);
    printf("  第 15 位    : 0x%08X   (1u << 15)\n", 1u << 15);
    printf("  高 8 位     : 0x%08X   (0xFFu << 24)\n", 0xFFu << 24);
    printf("  连续 8 位   : 0x%08X   (((1u<<8)-1) << 8)\n", ((1u << 8) - 1u) << 8);
    printf("  全 1        : 0x%08X   (~0u)\n", ~0u);
    printf("  【技巧】((1u<<n)-1) 是最省事的 n 个 1 的写法，注意 n 别到 32。\n");
    printf("  INT_MAX     : 0x%08X\n", (unsigned)INT_MAX);
    return 0;
}
