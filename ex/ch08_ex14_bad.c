/**
 * ch08_ex14_bad.c —— 故意犯错：包含没有卫士的头文件两次（这个文件编译不过）
 */
#include <stdio.h>
#include "ch08_ex14_noguard.h"
#include "ch08_ex14_noguard.h"    /**
 * 第二次：struct 被重新定义
 */

int main(void) { return 0; }
