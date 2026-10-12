/**
 * ch08_ex21.c —— 【踩坑】宏参数是"函数调用"时也会被求值多次
 */
#include <stdio.h>

/**
 * 模拟"读一次传感器"：每次调用返回值都不同（像真实的噪声）
 */
static int g_read_count = 0;
static int sensor_read(void)
{
    g_read_count++;
    return 100 + g_read_count;               /**
     * 101, 102, 103 ...
     */
}

#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define SQ(x)       ((x) * (x))

int main(void)
{
                /* 1) 最直观的现场：MIN(sensor_read(), 105) 读了几次？ */
    g_read_count = 0;
    int m = MIN(sensor_read(), 105);
    printf("MIN(sensor_read(), 105) = %d ，传感器被读了 %d 次\n", m, g_read_count);
    printf("  -> 期望只读 1 次，实际读了 %d 次（每次值都不同）\n\n", g_read_count);

                /* 2) 更糟：两个参数都是函数调用 */
    g_read_count = 0;
    int m2 = MIN(sensor_read(), sensor_read());
    printf("MIN(sensor_read(), sensor_read()) = %d ，传感器被读了 %d 次\n",
           m2, g_read_count);
    printf("  -> 读了 4 次！因为 a 和 b 各出现两次\n\n");

                /* 3) 对照：换成真函数，只求值一次 */
    g_read_count = 0;
    int a = sensor_read();
    int b = sensor_read();
    printf("先存变量再调用：%d ，传感器被读了 %d 次（这是预期的）\n\n",
           (a < b ? a : b), g_read_count);

                /* 4) 变式：加一层"先求值一次"的保护宏（最常用的修法） */
#define MIN_SAFE(a, b)  do { } while (0)   /* 宏做不到"只求值一次"，只能靠函数 */
    printf("结论：宏无法做到\"参数只求值一次\"。\n");
    printf("      想要一次求值 + 零开销，唯一正确的写法是 static inline 函数：\n");
    printf("        static inline int min_i(int a, int b) { return a < b ? a : b; }\n");
    printf("        min_i(sensor_read(), 105)  -> 一定只调用一次\n");

                /* 5) 顺便：这种 bug 的症状 */
    printf("\n症状：传感器读数\"跳跃\"、I2C 总线忙、SPI 时序被打乱 ——\n");
    printf("      看起来像硬件问题，其实是一行宏。\n");
    return 0;
}
