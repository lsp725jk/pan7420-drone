/**
 * ch08_ex22.c —— 【变式】"什么都不做"的宏怎么写：(void)0 / do{}while(0) / 空定义
 */
#include <stdio.h>

#define DEBUG 0                    /**
 * 把它改成 1，看看第 3 段的变化
 */

/**
 * 写法 A（错）：定义成空
 */
#define LOG_A(msg)                 /**
 * 展开成"什么都没有"
 */

/**
 * 写法 B（对）：展开成一条"什么都不做的语句"
 */
#define LOG_B(msg)  ((void)0)

/* 写法 C（对）：do-while(0)，和上一节的 SWAP_B 一个套路 */
#define LOG_C(msg)  do { if (DEBUG) printf("  [LOG_C] %s\n", (msg)); } while (0)

int main(void)
{
    int x = 5;

                /* 1) 写法 A 在 if 后面直接翻车 */
        /* 下面这一行会编译报错：'else' 没有匹配的 'if'（因为 LOG_A 展开成空，分号成了独立语句） if (x > 0) LOG_A("正数"); else printf("非正数\n"); 实测报错见章末"实测报错"（把上面那行的注释去掉就能复现） */
    printf("1) LOG_A 展开成空 -> 放进 if/else 会报「没有匹配 if 的非法 else」\n\n");

                /* 2) 写法 B：安全，且不影响 if/else */
    if (x > 0)
        LOG_B("正数");                             /* 展开成 ((void)0); —— 一条合法语句 */
    else
        printf("不会走到这里\n");
    printf("2) LOG_B ((void)0) 在 if/else 里完全安全\n\n");

                /* 3) 写法 C：可以带上编译期开关 */
    LOG_C("这一行只有 DEBUG=1 时才会打印");
    printf("3) LOG_C 现在 DEBUG=%d ，所以上面什么都没打印\n", DEBUG);
    printf("   把源码里的 #define DEBUG 改成 1 再编译，就会看到 [LOG_C] 那一行\n\n");

                /* 4) 变式：用 ##__VA_ARGS__ 让"带参数"和"不带参数"共用一个宏名 */
#define LOGF(fmt, ...)  do { if (DEBUG) printf("  [LOGF] " fmt "\n", ##__VA_ARGS__); } while (0)
    LOGF("没有可变参数");
    LOGF("有可变参数 a=%d b=%s", 7, "xyz");
    printf("4) LOGF 用 ##__VA_ARGS__ 同时支持两种调用（DEBUG=%d 时都不打印）\n\n", DEBUG);

                /* 5) 变式：真正"零开销"的写法是让宏展开成【完全没有代码】 */
    printf("5) 关掉调试时，(void)0 不生成任何指令；\n");
    printf("   而 do{ if(0) ... }while(0) 里的 if 也会被编译器优化掉。\n");
    printf("   两者在 Flash 占用上等价，((void)0) 更简洁。\n");
    return 0;
}
