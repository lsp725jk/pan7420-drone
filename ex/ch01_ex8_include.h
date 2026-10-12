/**
 * ch01_ex8_include.h —— 头文件该长什么样（配对示例，不能单独编译）
 */
#ifndef CH01_EX8_INCLUDE_H       /**
 * include guard（包含卫士）第一步
 */
#define CH01_EX8_INCLUDE_H       /**
 * 定义一个标记
 */

/**
 * 常量。宏没有类型，enum 有类型 —— 工程上更推荐 enum
 */
#define LED_ON   1
#define LED_OFF  0

/* 函数原型（prototype）：只写参数类型和返回类型，不写函数体 */
int  led_state_to_text_len(void);
const char *led_state_to_text(int state);

#endif /* CH01_EX8_INCLUDE_H */
