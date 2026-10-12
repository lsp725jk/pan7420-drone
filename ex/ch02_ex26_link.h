/**
 * ch02_ex26_link.h —— 跨文件共享的正确做法：声明放头文件
 */
#ifndef CH02_EX26_LINK_H
#define CH02_EX26_LINK_H

#include <stdint.h>

/**
 * extern 声明：告诉编译器"这个变量在别的 .c 里定义，别在这里分配空间"。
 */
extern uint32_t g_tick;
extern const char *g_fw_version;

/**
 * 函数声明：默认就是 extern，写不写都一样
 */
uint32_t tick_now(void);
void     tick_advance(uint32_t n);
uint32_t tick_elapsed(uint32_t since);

#endif /* CH02_EX26_LINK_H */
