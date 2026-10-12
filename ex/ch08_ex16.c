/**
 * ch08_ex16.c —— __FILE__ / __LINE__ / __func__ 与"可自动定位"的日志宏
 */
#include <stdio.h>

/**
 * __FILE__  ：当前源文件名（字符串字面量）
 */

/**
 * 变式 1：需要至少一个可变参数
 */
#define LOGF(fmt, ...) \
    printf("%-14s:%-4d [%-8s] " fmt "\n", __FILE__, __LINE__, __func__, __VA_ARGS__)

/**
 * 变式 2：只有一句消息、没有可变参数时，单独做一个宏更清楚
 */
#define LOGM(msg) \
    printf("%-14s:%-4d [%-8s] %s\n", __FILE__, __LINE__, __func__, (msg))



#define LOGV(fmt, ...) \
    printf("%-14s:%-4d [%-8s] " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

static int motor_test(int idx, int pwm)
{
    LOGF("电机 %d 设定 PWM = %d", idx, pwm);           /* 有参数 */
    LOGV("这一行没有可变参数");                         /* 靠 ## 把逗号吃掉 */
    if (pwm > 1000) {
        LOGM("PWM 超出安全范围");                               /* 只有消息 */
    }
    return pwm;
}

int main(void)
{
    printf("本文件 __FILE__ = %s\n", __FILE__);
    printf("这一行 __LINE__ = %d\n", __LINE__);

    LOGM("程序开始");
    printf("   ↑ 上面这行在源码里是第 %d 行\n", __LINE__ - 1);

    motor_test(1, 800);
    motor_test(2, 1500);

    LOGF("两个数：%d 和 %s", 42, "字符串也可以");
    LOGM("程序结束");
    return 0;
}
