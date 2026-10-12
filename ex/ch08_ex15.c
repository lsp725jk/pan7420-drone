/**
 * ch08_ex15.c —— #include "..." 与 #include <...> 的区别
 */
#include <stdio.h>                  /**
 * 标准库：尖括号
 */
#include "ch08_ex15_local.h"        /**
 * 自己的头文件：引号
 */

/**
 * 变式：头文件名也可以由宏算出来 —— 预处理器会把 #include 的参数先展开
 */
#define HEADER_NAME "ch08_ex15_local.h"
#include HEADER_NAME

int main(void)
{
    printf("LOCAL_HEADER_VERSION = %d\n", LOCAL_HEADER_VERSION);
    printf("local_add(3, 4) = %d\n", local_add(3, 4));

#ifdef _MSC_VER
    printf("本机编译器 MSVC 版本号 _MSC_VER = %d\n", _MSC_VER);
#endif
                /* 坑：把 <ch08_ex15_local.h> 写成尖括号会找不到文件， 因为编译器不会去"源文件所在目录"里找 —— 见 ch08_ex15_bad.c 的实测报错。 */
    return 0;
}
