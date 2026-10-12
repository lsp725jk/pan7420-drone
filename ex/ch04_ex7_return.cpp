/**
 * ch04_ex7_return.c —— 返回值：类型、多值、以及不能返回什么
 */
#include <stdio.h>
#include <string.h>

/**
 * ---- 1. 基本返回值，以及提前 return ----
 */
static int classify(int v)
{
    if (v < 0) {
        return -1;                              /* 提前返回，比层层嵌套 if 好读 */
    }
    if (v == 0) {
        return 0;
    }
    return 1;
}

/* 函数里所有路径都必须有返回值，否则 MSVC 会给 C4715 警告 */
static int may_forget(int v)
{
    if (v > 0) {
        return 1;
    }
                /* 这里漏了 return，v<=0 时返回的是不确定值 —— /W4 会警告 C4715 */
    return 0;                           /* 加上这句就对了 */
}

/* ---- 2. 返回结构体：一次带回多个值 ---- */
typedef struct {
    int min;
    int max;
    int sum;
} stats_t;

static stats_t calc_stats(const int *data, int count)
{
    stats_t s = { 0, 0, 0 };                    /* 先初始化，避免未定义值 */
    if (count <= 0) {
        return s;                                       /* 返回整个结构体（按值） */
    }
    s.min = data[0];
    s.max = data[0];
    for (int i = 0; i < count; i++) {
        if (data[i] < s.min) { s.min = data[i]; }
        if (data[i] > s.max) { s.max = data[i]; }
        s.sum += data[i];
    }
    return s;
}

/* ---- 3. 用指针参数带回多个值（C 的经典风格）---- */
static int calc_stats2(const int *data, int count,
                       int *out_min, int *out_max, int *out_sum)
{
    if (data == NULL || out_min == NULL || out_max == NULL || out_sum == NULL) {
        return -1;                                      /* 返回值只用来报错 */
    }
    if (count <= 0) {
        return -2;
    }
    int mn = data[0], mx = data[0], sm = 0;
    for (int i = 0; i < count; i++) {
        if (data[i] < mn) { mn = data[i]; }
        if (data[i] > mx) { mx = data[i]; }
        sm += data[i];
    }
    *out_min = mn;
    *out_max = mx;
    *out_sum = sm;
    return 0;
}

/* ---- 4. 错误示范：返回局部变量的地址 ---- */
/* 这个函数是**故意**写错的，用来演示悬空指针。 MSVC /W4 会报 warning C4172（返回局部变量或临时变量的地址）， 这里压掉警告让输出干净；你的代码里出现 C4172，必须改，不能压。 */
#pragma warning(push)
#pragma warning(disable : 4172)
static int *bad_return_local(void)
{
    int local = 12345;
    return &local;                      /* 危险！函数返回后 local 已经不存在了 */
}
#pragma warning(pop)

/* 上面的正确写法 1：返回 static 变量（注意：不是线程安全的） */
static int *ok_return_static(void)
{
    static int persistent = 12345;              /* 静态存储期，函数返回后依然有效 */
    persistent++;
    return &persistent;
}

/* 上面的正确写法 2：让调用者提供缓冲区（最推荐，谁申请谁释放） */
static int  fill_buffer(int *buf, int cap, int value)
{
    if (buf == NULL || cap <= 0) {
        return -1;
    }
    for (int i = 0; i < cap; i++) {
        buf[i] = value + i;
    }
    return cap;                         /* 返回写入了多少个 */
}

/* ---- 5. 返回字符串的两种正确做法 ---- */
static const char *mode_to_string(int mode)
{
                /* 返回字符串字面量：它存在只读数据段，生命周期是整个程序，安全 */
    switch (mode) {
    case 0:  return "IDLE";
    case 1:  return "ARMED";
    default: return "UNKNOWN";
    }
}

int main(void)
{
    printf("---- 1. 基本返回值 ----\n");
    printf("classify(-5)=%d classify(0)=%d classify(7)=%d\n",
           classify(-5), classify(0), classify(7));
    printf("may_forget(5)=%d may_forget(-5)=%d\n", may_forget(5), may_forget(-5));

    printf("---- 2. 返回结构体 ----\n");
    int data[6] = { 4, -2, 9, 0, 7, 3 };
    stats_t s = calc_stats(data, 6);
    printf("min=%d max=%d sum=%d\n", s.min, s.max, s.sum);
    printf("  结构体不大时直接返回，编译器通常用寄存器传，不比指针慢；\n");
    printf("  结构体很大（几百字节）时，还是传指针 + 输出参数更合适。\n");

    printf("---- 3. 指针参数带回多个值 ----\n");
    int mn = 0, mx = 0, sm = 0;
    int rc = calc_stats2(data, 6, &mn, &mx, &sm);
    printf("rc=%d min=%d max=%d sum=%d\n", rc, mn, mx, sm);
    printf("  调用者一看 rc 就知道成功没有，这是最常见的 C 接口约定：\n");
    printf("  0 表示成功，负数表示各种错误。\n");

    printf("---- 4. 悬空指针演示 ----\n");
    int *bad = bad_return_local();
    printf("  bad_return_local() 返回的地址 = %p\n", (void *)bad);
    printf("  它指向的栈内存已经失效。此刻读 *bad，得到什么完全看运气：\n");
    printf("  可能还是 12345，也可能被别的函数调用覆盖成垃圾。\n");
    printf("  MSVC /W4 会提前报 warning C4172: 返回局部变量或临时变量的地址。\n");
    printf("  【症状】调试版正常、开优化或换调用顺序后开始出乱码/崩溃，\n");
    printf("         这类 bug 最难查，因为它在看起来没问题的代码里。\n");

    printf("  正确做法 1（返回 static）:\n");
    int *p1 = ok_return_static();
    int *p2 = ok_return_static();
    printf("    两次调用返回的地址 %s，值 = %d\n",
           (p1 == p2) ? "相同" : "不同", *p1);
    printf("    注意：static 版本不能被多个线程/中断同时用。\n");

    printf("  正确做法 2（调用者提供缓冲区，最推荐）:\n");
    int buf[4];
    int wrote = fill_buffer(buf, 4, 10);
    printf("    写入了 %d 个:", wrote);
    for (int i = 0; i < wrote; i++) {
        printf(" %d", buf[i]);
    }
    printf("\n");

    printf("  正确做法 3（返回字符串字面量）: mode_to_string(1) = %s\n",
           mode_to_string(1));

    printf("---- 5. 返回值的设计建议 ----\n");
    printf("a) 二选一的状态，用 int 返回 0/1 或 enum，别用神秘数字；\n");
    printf("b) 可能失败的操作，返回值专门用来报错，结果走指针参数；\n");
    printf("c) 一定不要返回局部变量的地址/数组；\n");
    printf("d) 返回类型如果和实际值类型不一致，会有隐式转换，\n");
    printf("   例如声明 int 却 return 3.99，得到的是 3 而不是 4。\n");
    return 0;
}
