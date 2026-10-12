/**
 * ch08_ex12.c —— 条件编译的变式与坑：#if 里的"未定义标识符"与命令行 -D
 */
#include <stdio.h>

/**
 * 变式 1：把"命令行开关"变成默认值 —— 这是嵌入式工程最常见的写法
 */
#ifndef USE_MAG
#define USE_MAG 0          /**
 * 没人在命令行 -D 就默认关掉
 */
#endif

/**
 * 坑 1：#if 里出现"从没定义过的标识符"不会报错，它被当成 0
 */
/**
 * #if NEVER_DEFINED      等价于 #if 0
 */

/* 坑 2：所以 #if XXX == 0 这种写法"永远成立"，哪怕 XXX 拼错了 */
#define BMP280_I2C_ADDR 0x76
/* 假设这里手滑写成了 BM280_I2C_ADDR（少一个 P）： */
#if BM280_I2C_ADDR == 0
#define TYPO_DETECTED 1
#endif

int main(void)
{
#if USE_MAG
    printf("USE_MAG = %d ：磁力计功能已编译进来\n", USE_MAG);
#else
    printf("USE_MAG = %d ：磁力计功能被条件编译裁掉了\n", USE_MAG);
#endif

#ifdef TYPO_DETECTED
    printf("坑 2 现场：#if BM280_I2C_ADDR == 0 成立 —— 其实这个宏根本不存在，\n");
    printf("          漏写一个字母不会被发现，条件静默为真。\n");
#endif

    printf("正确写法：#if defined(BMP280_I2C_ADDR) 才会真的检查\"有没有定义\"\n");
#if defined(BMP280_I2C_ADDR)
    printf("BMP280_I2C_ADDR = 0x%02X\n", BMP280_I2C_ADDR);
#endif

                /* 坑 3：#if 里不能出现 sizeof / 强制类型转换 / 浮点数 / 逗号， 因为它是在"编译之前"由预处理器算的，那时候还没有类型 */
    printf("对比：#if 只能算整数常量表达式；sizeof 是编译期算的，预处理器看不到\n");
    return 0;
}
