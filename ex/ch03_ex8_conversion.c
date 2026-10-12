/**
 * ch03_ex8_conversion.c —— 类型转换：显式、隐式、窄化
 */
#include <stdio.h>
#include <stdint.h>



#pragma warning(push)
#pragma warning(disable : 4244)
static int returns_int(void)
{
    return 3.99;                         /* 3.99 被截断成 3，不是四舍五入 */
}
#pragma warning(pop)

int main(void)
{
                /* ---- 1. 隐式转换：赋值与传参 ---- */
    printf("---- 1. 隐式转换 ----\n");
    double d = 3.7;
#pragma warning(push)
#pragma warning(disable : 4244)
    int    i = d;                       /* 隐式：double -> int，截断成 3 */
#pragma warning(pop)
    printf("double 3.7 赋给 int     = %d\n", i);
    printf("函数返回 3.99 但声明 int = %d\n", returns_int());
    printf("传参也同理：给 int 形参传 double，进函数前就截断了\n");

                /* 反方向：int -> double 是安全的（可能丢精度，但不会丢量级） */
    int    big = 16777217;              /* 2^24 + 1 */
    float  f   = (float)big;            /* float 只有 24 位有效位，装不下这个奇数 */
    printf("16777217 转成 float 再转回来 = %.0f  <-- 尾数不够，少了 1\n",
           (double)f);
    printf("这说明：int -> float 也可能丢信息（当数值 > 2^24 时）\n");

                /* ---- 2. 显式转换：写法与用处 ---- */
    printf("---- 2. 显式转换 ----\n");
    int  a = 7, b = 2;
    printf("a / b             = %d\n", a / b);
    printf("(double)a / b     = %f   <-- 最常用的显式转换\n", (double)a / b);
    printf("a / (double)b     = %f\n", a / (double)b);

                /* 显式转换在指针上很常见：void* 来回转 */
    int value = 0x12345678;
    void *vp = &value;                          /* 任何对象指针都能隐式转成 void */
    int  *ip = (int *)vp;                       /* 转回来必须显式，否则 C 里会警告 */
    printf("通过 void* 往返: 0x%X\n", (unsigned)*ip);

                /* ---- 3. 窄化：高位直接丢掉 ---- */
    printf("---- 3. 窄化转换 ----\n");
    int32_t  w = 0x12345678;
    int16_t  n16 = (int16_t)w;                  /* 只留低 16 位 */
    int8_t   n8  = (int8_t)w;                   /* 只留低 8 位 */
    printf("0x%08X 截成 16 位 = 0x%04X (%d)\n",
           (unsigned)w, (unsigned)(uint16_t)n16, (int)n16);
    printf("0x%08X 截成  8 位 = 0x%02X (%d)\n",
           (unsigned)w, (unsigned)(uint8_t)n8, (int)n8);

    uint8_t  u8  = 200;
    uint16_t u16 = u8;                          /* 变宽：值不变，安全 */
    uint8_t  back = (uint8_t)u16;               /* 变窄：可能丢 */
    printf("uint8_t 200 -> uint16_t -> uint8_t = %u（没丢，因为装得下）\n",
           (unsigned)back);

    uint16_t u300 = 300;
    uint8_t  low  = (uint8_t)u300;
    printf("300 截成 uint8_t = %u   (300 = 0x12C，只留 0x2C = 44)\n",
           (unsigned)low);

                /* ---- 4. 浮点 <-> 整数：截断、超出范围是 UB ---- */
    printf("---- 4. 浮点与整数互转 ----\n");
    double vals[6] = { 3.9, -3.9, 0.5, -0.5, 2.5, -2.5 };
    for (int k = 0; k < 6; k++) {
        printf("%+.1f -> (int) = %+d\n", vals[k], (int)vals[k]);
    }
    printf("规则：向零截断。3.9->3, -3.9->-3。要四舍五入得用 round()。\n");
                /* 【坑】把超出 int 范围的浮点转成 int 是 UB： double huge = 1e30; int bad = (int)huge;      <-- 未定义行为，别试 */
    printf("把 1e30 转成 int 是 UB（结果不可预测），所以必须自己先判断范围。\n");
    double huge = 1e30;
    if (huge >= -2147483648.0 && huge <= 2147483647.0) {
        printf("先判断范围再转，才安全\n");
    } else {
        printf("1e30 超出 int 范围 -> 拒绝转换（正确做法）\n");
    }

                /* ---- 5. 有符号/无符号之间：值可能完全改变 ---- */
    printf("---- 5. 有符号 <-> 无符号 ----\n");
    int          neg = -1;
    unsigned int un  = (unsigned int)neg;
    printf("(unsigned)(-1)     = %u\n", un);
    printf("(int)4294967295u   = %d\n", (int)4294967295u);
    printf("同样的位模式，两种解释方式 —— 转换本身不改位，只改怎么读。\n");

    int8_t  s8 = -1;
    uint8_t p8 = (uint8_t)s8;
    printf("(uint8_t)(int8_t)-1 = %u   (0xFF)\n", (unsigned)p8);

                /* ---- 6. 什么时候该显式转换 ---- */
    printf("---- 6. 实践建议 ----\n");
    printf("该写：整数除法想得到小数  (double)sum / count\n");
    printf("该写：故意截断低 8 位取字节 (uint8_t)(x & 0xFF)\n");
    printf("该写：库函数的宽松签名  printf(\"%%p\", (void*)&x)\n");
    printf("不该写：用 (int) 把 double 硬塞进 int 来'消掉警告'——\n");
    printf("        那只是把问题藏起来。先想清楚值域，再决定转不转。\n");
    printf("C23 提醒：复合字面量 (int[]){1,2,3} 从 C99 就有（实测 C11/C17/C23 都能编），\n");
    printf("          C23 改的是它的存储期规则，不是写法。\n");
    return 0;
}
