/**
 * ch08_ex09.c —— # 字符串化（stringize）与 ## 记号粘贴（token paste）
 */
#include <stdio.h>

/**
 * # 把参数变成字符串字面量。两级写法才能让参数里的宏先展开
 */
#define STR_(x)  #x
#define STR(x)   STR_(x)

/**
 * ## 把左右两个记号粘成一个新记号
 */
#define MAKE_NAME(prefix, n)  prefix##n
#define DECLARE_VAR(n)        int var_##n = n

/**
 * 例：用 ## 批量生成"取寄存器地址"的函数名
 */
#define DEF_READER(reg)       static unsigned int read_##reg(void) { return reg; }

/* 被"读取"的假寄存器（用普通变量模拟，真实工程里是 #define GPIOA ((GPIO_TypeDef*)0x50000000)） */
static unsigned int GPIOA = 0x40000000u;
static unsigned int GPIOB = 0x40000400u;
DEF_READER(GPIOA)
DEF_READER(GPIOB)

int main(void)
{
                /* 1) 字符串化：把"源码文本"变成字符串 */
    printf("STR(1 + 2)      = %s\n", STR(1 + 2));
    printf("STR_(1 + 2)     = %s\n", STR_(1 + 2));
    printf("STR(__LINE__)   = %s  （宏先展开，得到的是行号）\n", STR(__LINE__));
    printf("STR_(__LINE__)  = %s  （少一层，得到的是名字本身）\n", STR_(__LINE__));

                /* 2) 记号粘贴：在源码层面拼出新名字 */
    DECLARE_VAR(10);
    DECLARE_VAR(20);
    printf("var_10 = %d , var_20 = %d\n", var_10, var_20);

                /* 3) ## 的经典用处：给每个外设生成一套读函数（定义在文件开头） */
    printf("read_GPIOA() = 0x%08X , read_GPIOB() = 0x%08X\n",
           read_GPIOA(), read_GPIOB());

                /* 4) 坑：## 会阻止参数展开。想"宏套宏"必须先加一层中转 */
#define ID(x)      x##_suffix
#define ID2_(x)    x##_suffix
#define ID2(x)     ID2_(x)
#define REAL       real
    printf("ID(REAL)  = %s  （REAL 没被展开）\n", STR(ID(REAL)));
    printf("ID2(REAL) = %s  （加了一层中转后展开了）\n", STR(ID2(REAL)));
    return 0;
}
