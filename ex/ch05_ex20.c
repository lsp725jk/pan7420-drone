/**
 * ch05_ex20.c —— 用 C 语言（C17）编译：C 与 C++ 在指针上的关键差异
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Point { int x, y; };

int main(void)
{
                /**
         * 差异 1：C 允许把字符串字面量赋给 char*（字面量类型是 char[]，不带 const）
         */
    char* s = "hello";
    printf("[C] char* s = \"hello\"; 编译通过，s[0]='%c'\n", s[0]);
                /* s[0] = 'H'; */                           /* ← 打开这行：编译仍然通过，运行崩 */

                /* 差异 2：malloc 的 void* 在 C 里自动转换，不用强制转换 */
    int* a = malloc(5 * sizeof(int));              /* C 里不写 (int*)，C++ 里必须写 */
    for (int i = 0; i < 5; ++i) a[i] = i * i;
    printf("[C] malloc 不用转换: ");
    for (int i = 0; i < 5; ++i) printf("%d ", a[i]);
    printf("\n");
    free(a);

                /* 差异 3：结构体在 C 里必须带 struct 关键字（除非 typedef） */
    struct Point p = { 3, 4 };
    printf("[C] struct Point p = {%d, %d}\n", p.x, p.y);

                /* 差异 4：C99 起标准里有变长数组（VLA），C++ 没有；MSVC 的 C 模式也不支持 */
                /* int n = 5; int vla[n];  ← MSVC 下会报错，见正文说明 */

    printf("[C] __STDC_VERSION__ = %ld\n", (long)__STDC_VERSION__);
    return 0;
}
