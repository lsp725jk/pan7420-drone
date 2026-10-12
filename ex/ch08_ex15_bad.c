/**
 * ch08_ex15_bad.c —— 故意犯错：用尖括号包含"自己写的、就在同目录"的头文件
 */
#include <stdio.h>
#include <ch08_ex15_local.h>     /**
 * 错：编译器只在系统包含路径里找
 */

int main(void) { return LOCAL_HEADER_VERSION; }
