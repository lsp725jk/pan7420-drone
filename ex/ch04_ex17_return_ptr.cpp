/**
 * ch04_ex17_return_ptr.c —— 返回指针/结构体的四种正确做法与两种错误做法
 */
#include <stdio.h>
#include <string.h>

typedef struct {
    int   id;
    float value;
} sample_t;

/**
 * ---- 错误做法一：返回局部变量的地址（栈内存已失效）----
 */
#pragma warning(push)
#pragma warning(disable : 4172)
static int *bad_local_ptr(void)
{
    int local = 7;
    return &local;
}
#pragma warning(pop)

/* ---- 错误做法二：返回内部 static 缓冲区（会被下次调用覆盖）---- */
static const char *bad_shared_buffer(int n)
{
    static char buf[32];
    snprintf(buf, sizeof(buf), "value=%d", n);
    return buf;
}

/* ---- 正确做法一：返回 static（单次使用没问题，多线程/多次调用危险）---- */
static const char *ok_static_table(int mode)
{
    static const char *const names[3] = { "IDLE", "RUN", "FAULT" };
    if (mode < 0 || mode > 2) {
        return "?";
    }
    return names[mode];                     /* 返回的是字符串字面量/静态表，安全 */
}

/* ---- 正确做法二：返回结构体（小结构体，编译器可用寄存器传）---- */
static sample_t make_sample(int id, float v)
{
    sample_t s;
    s.id = id;
    s.value = v;
    return s;
}

/* ---- 正确做法三：调用者提供缓冲区（最推荐）---- */
static int format_into(char *buf, size_t cap, int n)
{
    if (buf == NULL || cap == 0) {
        return -1;
    }
    int need = snprintf(buf, cap, "value=%d", n);
    return (need < 0) ? -1 : need;          /* 返回"实际需要多少字符" */
}

/* ---- 正确做法四：返回值报错 + 输出参数带结果 ---- */
static int get_sample(int id, sample_t *out)
{
    if (out == NULL) {
        return -1;
    }
    if (id < 0 || id > 9) {
        return -2;
    }
    out->id = id;
    out->value = (float)id * 1.5f;
    return 0;
}

int main(void)
{
    printf("---- 1. 错误做法一：返回局部变量地址 ----\n");
    int *bad = bad_local_ptr();
    printf("  bad_local_ptr() = %p（栈内存，已失效）\n", (void *)bad);
    printf("  读 *bad 的结果不可预测；MSVC /W4 会报 warning C4172。\n");

    printf("---- 2. 错误做法二：返回内部 static 缓冲区 ----\n");
    const char *p1 = bad_shared_buffer(1);
    printf("  p1 = \"%s\"（第一次调用返回的内容）\n", p1);
    const char *p2 = bad_shared_buffer(2);
    printf("  p2 = \"%s\"（第二次调用）\n", p2);
    printf("  而 p1 现在也变成了 \"%s\"  <-- 两次调用共用同一块内存！\n", p1);
    printf("  注意 p1 和 p2 的地址 %s（它们指向同一个 static 缓冲区）。\n",
           (p1 == p2) ? "相同" : "不同");
    printf("  这就是 C 标准库里 strtok / localtime 那类函数的历史包袱。\n");

    printf("---- 3. 正确做法一：返回静态表里的字符串 ----\n");
    for (int m = 0; m <= 3; m++) {
        printf("  mode %d -> %s\n", m, ok_static_table(m));
    }
    printf("  安全：返回的是字符串字面量，生命期是整个程序。\n");

    printf("---- 4. 正确做法二：返回结构体 ----\n");
    sample_t s = make_sample(3, 4.5f);
    printf("  make_sample(3, 4.5) -> id=%d value=%.1f\n", s.id, (double)s.value);
    printf("  sizeof(sample_t) = %zu 字节（小结构体，返回开销可以忽略）\n",
           sizeof(sample_t));

    printf("---- 5. 正确做法三：调用者提供缓冲区（最推荐）----\n");
    char buf[16];
    int need = format_into(buf, sizeof(buf), 12345);
    printf("  format_into -> \"%s\"（返回需要 %d 个字符）\n", buf, need);
    char tiny[6];
    need = format_into(tiny, sizeof(tiny), 12345);
    printf("  缓冲区只有 6 字节 -> \"%s\"（安全截断，需要 %d 个字符）\n",
           tiny, need);
    printf("  看到 need >= cap 就知道被截断了 —— 这是 snprintf 的经典用法。\n");

    printf("---- 6. 正确做法四：返回值报错 + 输出参数 ----\n");
    sample_t got;
    printf("  get_sample(5, &got) = %d", get_sample(5, &got));
    printf("  -> id=%d value=%.1f\n", got.id, (double)got.value);
    printf("  get_sample(-1, &got) = %d（id 非法）\n", get_sample(-1, &got));
    printf("  get_sample(5, NULL) = %d（输出参数为空）\n", get_sample(5, NULL));
    printf("  这种形状最可控：调用者必须检查返回值，结果一定写进自己的变量。\n");
    return 0;
}
