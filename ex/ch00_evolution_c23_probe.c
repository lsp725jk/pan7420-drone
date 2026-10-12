/**
 * ch00_evolution_c23_probe.c —— 《技术演进总纲》实测脚本 2/4
 */

#include <stdio.h>

/* ---- 1. C23 的 constexpr：本机 MSVC 14.51 **不支持** ---- 预期：error C2054: 在"constexpr"之后应输入"(" C23 里它的作用是替代 #define / enum 当编译期常量，且有类型、有作用域。 */
constexpr int K = 42;



static_assert(sizeof(int) == 4, "本机 int 必须是 4 字节");

/* ---- 3. C11 的 _Generic：本机 C17 / clatest 都支持 ---- */
#define TYPENAME(x) _Generic((x), int: "int", double: "double", char *: "char*", default: "other")

int main(void)
{
    int  i = 1;
    double d = 2.0;

                /* ---- 4. C23 的 nullptr：本机 MSVC 14.51 **不支持** ---- 预期：error C2065: "nullptr": 未声明的标识符 */
    char *p = nullptr;

    printf("__STDC_VERSION__ = %ld\n", (long)__STDC_VERSION__);
    printf("K = %d\n", K);
    printf("%s %s %s\n", TYPENAME(i), TYPENAME(d), TYPENAME(p));
    return 0;
}
