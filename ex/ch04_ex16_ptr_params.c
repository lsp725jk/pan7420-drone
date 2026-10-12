/**
 * ch04_ex16_ptr_params.c —— 指针参数的四种用途与三个必须做的检查
 */
#include <stdio.h>
#include <string.h>

/**
 * ---- 用途一：改实参（交换两个变量，值传递做不到）----
 */
static void swap_int(int *a, int *b)
{
    if (a == NULL || b == NULL) {
        return;                                     /* 检查一：判空 */
    }
    int t = *a;
    *a = *b;
    *b = t;
}

/* ---- 用途二：带回多个结果（返回值专门用来报错）---- */
static int min_max_avg(const int *data, int count,
                       int *out_min, int *out_max, double *out_avg)
{
    if (data == NULL || out_min == NULL || out_max == NULL || out_avg == NULL) {
        return -1;                                  /* 参数错 */
    }
    if (count <= 0) {
        return -2;                                  /* 数据为空 */
    }
    int mn = data[0], mx = data[0];
    long long sum = 0;
    for (int i = 0; i < count; i++) {
        if (data[i] < mn) { mn = data[i]; }
        if (data[i] > mx) { mx = data[i]; }
        sum += data[i];
    }
    *out_min = mn;
    *out_max = mx;
    *out_avg = (double)sum / count;
    return 0;
}

/* ---- 用途三：传数组（必须同时传长度）---- */
static size_t copy_upto(char *dst, size_t cap, const char *src)
{
    if (dst == NULL || src == NULL || cap == 0) {
        return 0;                                   /* 检查二：容量为 0 也要挡住 */
    }
    size_t n = strlen(src);
    if (n >= cap) {
        n = cap - 1;                                /* 检查三：留一个字节给 '\0' */
    }
    memcpy(dst, src, n);
    dst[n] = '\0';
    return n;
}

/* ---- 用途四：传大结构体（只读就用 const 指针，避免复制）---- */
typedef struct {
    int   samples[64];
    int   count;
    float scale;
} big_cfg_t;

static float sum_scaled(const big_cfg_t *cfg)      /* const 指针：只读，不复制 */
{
    if (cfg == NULL) {
        return 0.0f;
    }
    long long acc = 0;
    for (int i = 0; i < cfg->count; i++) {
        acc += cfg->samples[i];
    }
    return (float)(acc * cfg->scale);
}

int main(void)
{
    printf("---- 1. 用途一：改实参 ----\n");
    int x = 1, y = 2;
    printf("  交换前 x=%d y=%d\n", x, y);
    swap_int(&x, &y);
    printf("  交换后 x=%d y=%d\n", x, y);
    swap_int(NULL, &y);                         /* 传 NULL 也要安全 */
    printf("  传 NULL 给 swap_int 没有崩（判空生效）\n");

    printf("---- 2. 用途二：带回多个结果 ----\n");
    int data[5] = { 3, -7, 9, 0, 5 };
    int mn = 0, mx = 0;
    double avg = 0.0;
    int rc = min_max_avg(data, 5, &mn, &mx, &avg);
    printf("  rc=%d min=%d max=%d avg=%.2f\n", rc, mn, mx, avg);
    printf("  参数错时：rc=%d\n", min_max_avg(data, 5, NULL, &mx, &avg));
    printf("  数据为空时：rc=%d\n", min_max_avg(data, 0, &mn, &mx, &avg));

    printf("---- 3. 用途三：传数组 + 容量 ----\n");
    char buf[8];
    size_t n = copy_upto(buf, sizeof(buf), "hello, world");
    printf("  copy_upto 返回 %zu，结果 = \"%s\"（被安全截断）\n", n, buf);
    n = copy_upto(buf, sizeof(buf), "hi");
    printf("  短字符串: 返回 %zu，结果 = \"%s\"\n", n, buf);
    printf("  cap=0 时返回 %zu（什么都没写，安全）\n", copy_upto(buf, 0, "x"));

    printf("---- 4. 用途四：传大结构体 ----\n");
    static big_cfg_t cfg;                       /* 放静态区，避免 256 字节的栈占用 */
    cfg.count = 4;
    cfg.scale = 0.5f;
    cfg.samples[0] = 10; cfg.samples[1] = 20;
    cfg.samples[2] = 30; cfg.samples[3] = 40;
    printf("  sizeof(big_cfg_t) = %zu 字节（按值传就要复制这么多）\n",
           sizeof(big_cfg_t));
    printf("  sum_scaled(&cfg) = %.1f\n", (double)sum_scaled(&cfg));
    printf("  传指针只复制 8 字节，而且 const 保证了函数不会改它。\n");

    printf("---- 5. 指针参数的三个必须检查 ----\n");
    printf("  a) 判空：if (p == NULL) return ...;\n");
    printf("  b) 判容量：if (cap == 0 || cap > MAX) return ...;\n");
    printf("  c) 留结尾空间：n = cap - 1（不是 cap）\n");
    printf("  这三条做到了，就能挡掉绝大多数缓冲区溢出和空指针崩溃。\n");
    return 0;
}
