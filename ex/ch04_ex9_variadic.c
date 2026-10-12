/**
 * ch04_ex9_variadic.c —— 可变参数：printf 是怎么做到参数个数不固定的
 */
#include <stdio.h>
#include <stdarg.h>

/**
 * ---- 1. 最简单的可变参数：求任意个 int 的和（第一个参数告诉个数）----
 */
static int sum_ints(int count, ...)
{
    va_list ap;
    int total = 0;

    va_start(ap, count);                    /* count 是最后一个有名参数 */
    for (int i = 0; i < count; i++) {
        total += va_arg(ap, int);                   /* 每次都按 int 取 */
    }
    va_end(ap);                             /* 收尾 */
    return total;
}

/* ---- 2. 参数个数不定时，用"哨兵值"结束（要约定一个不会出现的数据）---- */
static int sum_until_zero(int first, ...)
{
    va_list ap;
    int total = first;
    int v;

    va_start(ap, first);
    while ((v = va_arg(ap, int)) != 0) {            /* 遇到 0 就停 */
        total += v;
    }
    va_end(ap);
    return total;
}

/* ---- 3. 自己写一个 printf 风格的函数：先解析格式串再取值 ---- */
static void my_log(const char *tag, const char *fmt, ...)
{
    va_list ap;
    printf("[%s] ", tag);                   /* 先打印标签 */
    va_start(ap, fmt);
    vprintf(fmt, ap);                       /* 有 v 前缀的版本可以直接吃 va_list */
    va_end(ap);
    printf("\n");
}

/* 更进一步：把结果格式化到自己的缓冲区里（嵌入式日志常用） */
static int format_into(char *buf, size_t cap, const char *fmt, ...)
{
    va_list ap;
    int n;
    va_start(ap, fmt);
    n = vsnprintf(buf, cap, fmt, ap);           /* vsnprintf 是安全版本，带容量上限 */
    va_end(ap);
    return n;                                   /* 返回实际需要的长度 */
}

/* ---- 4. 可变参数与默认参数提升：为什么 float 会变成 double ---- */
static void show_promotions(int count, ...)
{
    va_list ap;
    va_start(ap, count);
    for (int i = 0; i < count; i++) {
                                /* 关键规则：可变参数位置上，float 一定被提升为 double， char/short 一定被提升为 int。 所以取的时候必须写 double，不能写 float；必须写 int，不能写 char。 */
        double d = va_arg(ap, double);
        printf("    第 %d 个: %.3f\n", i + 1, d);
    }
    va_end(ap);
}

int main(void)
{
    printf("---- 1. 按个数取参数 ----\n");
    printf("sum_ints(3, 10,20,30)    = %d\n", sum_ints(3, 10, 20, 30));
    printf("sum_ints(5, 1,2,3,4,5)   = %d\n", sum_ints(5, 1, 2, 3, 4, 5));
    printf("sum_ints(1, 100)         = %d\n", sum_ints(1, 100));

    printf("---- 2. 用哨兵值结束 ----\n");
    printf("sum_until_zero(1,2,3,0)        = %d\n", sum_until_zero(1, 2, 3, 0));
    printf("sum_until_zero(10,20,30,40,0)  = %d\n", sum_until_zero(10, 20, 30, 40, 0));
    printf("【坑】如果忘了写最后的 0，函数会一直往后读栈上的垃圾，\n");
    printf("      轻则算出离谱结果，重则崩溃。这是可变参数最危险的地方。\n");

    printf("---- 3. 自己实现 printf 风格 ----\n");
    my_log("IMU", "gyro x=%d y=%d z=%d", 12, -3, 47);
    my_log("BARO", "altitude=%.2f m, temp=%.1f C", 123.456, 25.5);
    my_log("SYS", "boot ok");

    char line[64];
    int need = format_into(line, sizeof(line), "roll=%+.3f pitch=%+.3f", 1.23456, -0.98765);
    printf("  格式化到缓冲区: \"%s\"（实际需要 %d 个字符）\n", line, need);
    printf("  vsnprintf 的容量参数非常重要：它是唯一能防缓冲区溢出的手段。\n");

    printf("---- 4. 默认参数提升 ----\n");
    printf("  传 float 进去，取的时候必须用 double：\n");
    show_promotions(3, 1.5f, 2.25f, 3.125f);
    printf("  传 char/short 进去，取的时候必须用 int：\n");
    {
        va_list ap;
                                /* 演示：char 在可变参数里被提升为 int */
        int c1 = 'A';
        printf("    'A' 提升后按 int 取 = %d\n", c1);
        (void)ap;
    }

    printf("---- 5. 可变参数的常见错误清单 ----\n");
    printf("a) 忘记 va_end（在多数实现里无害，但标准要求必须配对）；\n");
    printf("b) 用错类型：va_arg(ap, float) 是错的，必须 double；\n");
    printf("c) 格式串和实参不匹配：printf(\"%%d\", 3.5) 编译能过，运行乱码；\n");
    printf("   本机 /W4 对字面量格式串会检查（warning C4477），这是能救命的功能；\n");
    printf("d) 忘记哨兵值或个数不对，会读到栈上的垃圾；\n");
    printf("e) 把用户可控的字符串直接当格式串：printf(user_input) ——\n");
    printf("   这是典型的格式串漏洞（format string vulnerability），绝对不能这么写。\n");
    printf("   正确写法：printf(\"%%s\", user_input);\n");
    return 0;
}
