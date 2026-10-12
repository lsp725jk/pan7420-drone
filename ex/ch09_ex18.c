/**
 * ch09_ex18.c —— 四个"定义环境"的头文件：
 */
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct Frame {
    uint8_t  head;
    uint32_t ts;
    uint8_t  tail;
};

int main(void)
{
                /* ---------- limits.h ---------- */
    printf("== limits.h ==\n");
    printf("CHAR_BIT   = %d   一个字节多少位\n", CHAR_BIT);
    printf("CHAR_MIN/MAX = %d / %d\n", CHAR_MIN, CHAR_MAX);
    printf("SCHAR_MAX  = %d , UCHAR_MAX = %d\n", SCHAR_MAX, UCHAR_MAX);
    printf("SHRT_MAX   = %d , USHRT_MAX = %d\n", SHRT_MAX, USHRT_MAX);
    printf("INT_MAX    = %d , INT_MIN = %d\n", INT_MAX, INT_MIN);
    printf("UINT_MAX   = %u\n", UINT_MAX);
    printf("LONG_MAX   = %ld （Windows/ARM 32 位下都是 32 位）\n", LONG_MAX);
    printf("LLONG_MAX  = %lld\n", LLONG_MAX);

                /* ---------- float.h ---------- */
    printf("\n== float.h ==\n");
    printf("FLT_RADIX    = %d   浮点的进制（2 = 二进制）\n", FLT_RADIX);
    printf("FLT_MANT_DIG = %d   有效二进制位数\n", FLT_MANT_DIG);
    printf("FLT_DIG      = %d   能保证的十进制有效位数\n", FLT_DIG);
    printf("DBL_DIG      = %d\n", DBL_DIG);
    printf("FLT_EPSILON  = %.10e  1.0 与「下一个 float」之差\n", (double)FLT_EPSILON);
    printf("DBL_EPSILON  = %.10e\n", DBL_EPSILON);
    printf("FLT_MAX      = %.6e , FLT_MIN = %.6e\n", (double)FLT_MAX, (double)FLT_MIN);
    printf("DBL_MAX      = %.6e\n", DBL_MAX);
    printf("判断 float 相等时用的容差，就从 FLT_EPSILON 推出来\n");

                /* ---------- stdint.h：嵌入式里【只用这一套】 ---------- */
    printf("\n== stdint.h ==\n");
    printf("sizeof: int8=%zu uint8=%zu int16=%zu uint16=%zu int32=%zu uint32=%zu int64=%zu\n",
           sizeof(int8_t), sizeof(uint8_t), sizeof(int16_t), sizeof(uint16_t),
           sizeof(int32_t), sizeof(uint32_t), sizeof(int64_t));
    printf("INT8_MAX  = %d , INT8_MIN = %d , UINT8_MAX = %u\n",
           INT8_MAX, INT8_MIN, UINT8_MAX);
    printf("INT16_MAX = %d , UINT16_MAX = %u\n", INT16_MAX, UINT16_MAX);
    printf("INT32_MAX = %d , UINT32_MAX = %u\n", INT32_MAX, UINT32_MAX);
    printf("INT64_MAX = %lld\n", (long long)INT64_MAX);
    printf("INT32_C(5) = %d ，UINT32_C(5) = %u （保证字面量类型正确）\n",
           INT32_C(5), UINT32_C(5));
    printf("intptr_t 大小 = %zu （能装下一个指针的整数类型）\n", sizeof(intptr_t));

                /* ---------- stdbool.h ---------- */
    printf("\n== stdbool.h ==\n");
    bool ok = true;
    bool no = false;
    printf("true = %d , false = %d , sizeof(bool) = %zu\n", ok, no, sizeof(bool));
    printf("注意：C 里 bool 底层就是 _Bool，占 1 字节；\n");
    printf("      任何非 0 值赋给 bool 都会变成 1：bool b = 42; -> %d\n", (bool)42);

                /* ---------- stddef.h ---------- */
    printf("\n== stddef.h ==\n");
    printf("NULL = %p , sizeof(size_t) = %zu , sizeof(ptrdiff_t) = %zu\n",
           (void *)NULL, sizeof(size_t), sizeof(ptrdiff_t));
    printf("offsetof(struct Frame, ts)   = %zu\n", offsetof(struct Frame, ts));
    printf("offsetof(struct Frame, tail) = %zu  （不是 5，因为有对齐填充）\n",
           offsetof(struct Frame, tail));
    printf("sizeof(struct Frame)         = %zu\n", sizeof(struct Frame));
    return 0;
}
