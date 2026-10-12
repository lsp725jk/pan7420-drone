/**
 * ch10_ex14_ptr.c —— 【故意让编译期断言失败】看它拦住了什么
 */
#include <stdint.h>

_Static_assert(sizeof(void *) == 4,
               "本工程假设指针是 4 字节：PY32F030 是 32 位机，PC 是 64 位，请换平台编译");

int main(void) { return 0; }
