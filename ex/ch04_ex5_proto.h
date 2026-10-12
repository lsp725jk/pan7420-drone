/**
 * ch04_ex5_proto.h —— 头文件里该放什么
 */
#ifndef CH04_EX5_PROTO_H
#define CH04_EX5_PROTO_H

/**
 * 常量：用宏或者 enum。宏没有类型，enum 有类型，工程上更推荐 enum
 */
#define MAX_SAMPLES 8

/**
 * 函数声明（也叫函数原型 function prototype）： 参数名可以省略，但写上更利于阅读文档
 */
int  sample_add(int a, int b);
int  sample_max(const int *data, int count);
void sample_reset(void);

/* 声明一个"别处定义的全局变量" */
extern int g_sample_calls;

#endif /* CH04_EX5_PROTO_H */
