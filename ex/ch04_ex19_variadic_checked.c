/**
 * ch04_ex19_variadic_checked.c —— 可变参数的安全写法：让"格式串检查"帮上忙
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

/**
 * ---- 1. 最安全：个数 + 类型固定的逐个取 ----
 */
/**
 * 全部按 int 取，调用者只要保证"传的都是 int 且个数对"就行
 */
static long long sum_i32(int count, ...)
{
    va_list ap;
    long long total = 0;
    va_start(ap, count);
    for (int i = 0; i < count; i++) {
        total += va_arg(ap, int);
    }
    va_end(ap);
    return total;
}

/* ---- 2. 稍灵活：用枚举/标签说明下一个参数的类型 ---- */
/* 这是一种"自己实现的类型标签"风格，比裸格式串安全（至少类型是枚举出来的） */
typedef enum { ARG_I32, ARG_I64, ARG_DBL } arg_type_t;

static double sum_tagged(int count, ...)
{
    va_list ap;
    double total = 0.0;
    va_start(ap, count);
    for (int i = 0; i < count; i++) {
        arg_type_t t = (arg_type_t)va_arg(ap, int);                   /* 先取标签 */
        switch (t) {
        case ARG_I32: total += va_arg(ap, int);    break;
        case ARG_I64: total += (double)va_arg(ap, long long); break;
        case ARG_DBL: total += va_arg(ap, double); break;
        default:      break;
        }
    }
    va_end(ap);
    return total;
}

/* ---- 3. 最灵活：格式串 + vprintf/vsnprintf 转发 ---- */

static void log_tagged(const char *tag, _Printf_format_string_ const char *fmt, ...)
{
    va_list ap;
    printf("[%s] ", tag);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

/* 把格式化结果写进调用者的缓冲区，并返回"实际需要多少字符" */
static int log_to_buf(char *buf, size_t cap, const char *fmt, ...)
{
    va_list ap;
    int need;
    if (buf == NULL || cap == 0) {
        return -1;
    }
    va_start(ap, fmt);
    need = vsnprintf(buf, cap, fmt, ap);
    va_end(ap);
    return need;
}

int main(void)
{
    printf("---- 1. 个数 + 类型固定：最安全 ----\n");
    printf("  sum_i32(4, 1,2,3,4)      = %lld\n", sum_i32(4, 1, 2, 3, 4));
    printf("  sum_i32(3, 100,-50,25)   = %lld\n", sum_i32(3, 100, -50, 25));
    printf("  调用者只要数对数就行 —— 类型是写死的，不可能读错。\n");

    printf("---- 2. 标签 + 值：类型由标签决定 ----\n");
    double d = sum_tagged(4,
                          ARG_I32, 10,                                                              /* 10 按 int 取 */
                          ARG_DBL, 2.5,                                                             /* 2.5 按 double 取 */
                          ARG_I64, 1000LL,                                                          /* 1000LL 按 long long 取 */
                          ARG_I32, -3);
    printf("  sum_tagged(...) = %.2f\n", d);
    printf("  比裸格式串安全：标签是枚举，拼错编译器会报错。\n");

    printf("---- 3. 格式串 + vprintf 转发（最灵活）----\n");
    log_tagged("IMU", "gyro x=%d y=%d z=%d", 12, -3, 47);
    log_tagged("BARO", "altitude=%.2f m", 123.456);
    log_tagged("SYS", "boot ok");
    printf("  本机实测：_Printf_format_string_ 注解让 cl 对字面量格式串做检查；\n");
    printf("  如果你写 log_tagged(\"X\", \"%%d\", 3.5)，/W4 会报 C4477。\n");

    printf("---- 4. 写到调用者的缓冲区：最可控 ----\n");
    char buf[32];
    int need = log_to_buf(buf, sizeof(buf), "roll=%+.2f pitch=%+.2f", 1.234, -0.987);
    printf("  结果 = \"%s\"，需要 %d 个字符\n", buf, need);
    char tiny[8];
    need = log_to_buf(tiny, sizeof(tiny), "roll=%+.2f pitch=%+.2f", 1.234, -0.987);
    printf("  缓冲区只有 8 字节 -> \"%s\"，需要 %d 个字符（超过 cap 就被截断）\n",
           tiny, need);
    printf("  【用法】if (need >= (int)sizeof(tiny)) 就知道被截断了。\n");

    printf("---- 5. 可变参数的三条安全规则 ----\n");
    printf("  a) 优先用「个数 + 类型固定」，不要为了灵活牺牲检查；\n");
    printf("  b) 必须用格式串时，加编译器注解（MSVC: _Printf_format_string_）；\n");
    printf("  c) 永远不要把用户输入直接当格式串（printf(buf) 是漏洞）。\n");
    return 0;
}
