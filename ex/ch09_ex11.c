/**
 * ch09_ex11.c —— rand / srand / RAND_MAX：伪随机数与"取模偏差"
 */
#include <stdio.h>
#include <stdlib.h>

/**
 * 变式：自己写的轻量随机数发生器（嵌入式常用），同种子必然同序列
 */
static unsigned int g_seed = 1u;
static unsigned int xorshift32(void)
{
    g_seed ^= g_seed << 13;
    g_seed ^= g_seed >> 17;
    g_seed ^= g_seed << 5;
    return g_seed;
}

int main(void)
{
    printf("RAND_MAX = %d\n", RAND_MAX);

                /* 1) 固定种子 -> 每次运行序列完全相同（可复现，调试很有用） */
    srand(1);
    printf("srand(1) 前 5 个 : ");
    for (int k = 0; k < 5; k++) printf("%d ", rand());
    printf("\n");

    srand(1);
    printf("再来一次 srand(1): ");
    for (int k = 0; k < 5; k++) printf("%d ", rand());
    printf("   <-- 一样，所以 rand 是「伪」随机\n");

                /* 2) 不调 srand 相当于 srand(1) */
    printf("不播种的第一次 rand() = %d\n", rand());

                /* 3) 生成 [0, 100) 的随机数：正确姿势 */
    srand(42);
    printf("rand()%%100 前 10 个 : ");
    for (int k = 0; k < 10; k++) printf("%d ", rand() % 100);
    printf("\n");

        /* 4) 坑：rand() % N 的"取模偏差" —— 低位不是均匀分布的。 用 RAND_MAX = 32767 举例：32768 / 10 = 3276.8， 所以 0~7 每个数会出现 3277 次，而 8~9 只有 3276 次。 */
    printf("RAND_MAX+1 = %d , 对 10 取模：\n", RAND_MAX + 1);
    {
        int buckets[10] = { 0 };
        srand(7);
        for (int k = 0; k < 200000; k++) buckets[rand() % 10]++;
        printf("  各桶计数：");
        for (int k = 0; k < 10; k++) printf("%d ", buckets[k]);
        printf("\n  （偏差很小，但高要求场合要用 rand()/(RAND_MAX+1.0)*N 的写法）\n");
    }

                /* 5) 变式：自己写的 xorshift32，同种子同序列，且不依赖库 */
    g_seed = 1u;
    printf("xorshift32 前 5 个 : ");
    for (int k = 0; k < 5; k++) printf("%u ", xorshift32());
    printf("\n");
    g_seed = 1u;
    printf("重新播种后再来一次 : ");
    for (int k = 0; k < 5; k++) printf("%u ", xorshift32());
    printf("   <-- 可复现\n");
    return 0;
}
