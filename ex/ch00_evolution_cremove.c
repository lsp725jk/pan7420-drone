/**
 * ch00_evolution_cremove.c —— 《技术演进总纲》实测脚本 3/4
 */

#include <stdio.h>
#include <string.h>

int main(void)
{
    int n = 5;



    char line[32];
    gets(line);



    int a[n];
    for (int i = 0; i < n; ++i) {
        a[i] = i;
    }

                /* ---- 3. strcpy：同样不检查边界，但标准一直保留 ---- 这个**不会报错**（MSVC 可能给弃用警告，不是错误）。 */
    char dst[8];
    strcpy(dst, "abcd");

    printf("%s %d\n", dst, a[0]);
    return 0;
}
