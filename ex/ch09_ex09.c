/**
 * ch09_ex09.c —— qsort：C 标准库唯一的通用排序
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 变式 1：整数升序。注意不能写 return *a - *b —— 相减会溢出
 */
static int cmp_int_asc(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);                  /* 只返回 -1 / 0 / 1，永不出错 */
}
/* 变式 2：整数降序 —— 就是把比较结果取反 */
static int cmp_int_desc(const void *a, const void *b)
{
    return -cmp_int_asc(a, b);
}

/* 变式 3：结构体按某个成员排序 */
struct ImuCal {
    int   id;
    float bias;
    const char *name;
};
static int cmp_by_bias(const void *a, const void *b)
{
    const struct ImuCal *pa = (const struct ImuCal *)a;
    const struct ImuCal *pb = (const struct ImuCal *)b;
    if (pa->bias < pb->bias) return -1;
    if (pa->bias > pb->bias) return  1;
    return 0;
}

/* 变式 4：字符串排序（数组里放的是指针，所以参数是 char * const * ） */
static int cmp_str(const void *a, const void *b)
{
    const char *const *pa = (const char *const *)a;
    const char *const *pb = (const char *const *)b;
    return strcmp(*pa, *pb);
}

int main(void)
{
    int v[8] = { 42, -7, 0, 999, 3, -7, 15, 100 };
    qsort(v, 8, sizeof(v[0]), cmp_int_asc);
    printf("整数升序：");
    for (int k = 0; k < 8; k++) printf("%d ", v[k]);
    printf("\n");

    qsort(v, 8, sizeof(v[0]), cmp_int_desc);
    printf("整数降序：");
    for (int k = 0; k < 8; k++) printf("%d ", v[k]);
    printf("\n");

    struct ImuCal cal[4] = {
        { 3, 1.25f, "gyro_z"  },
        { 1, -0.50f, "gyro_x" },
        { 4, 0.03f, "gyro_p"  },
        { 2, -2.10f, "accel_x" },
    };
    qsort(cal, 4, sizeof(cal[0]), cmp_by_bias);
    printf("按零偏排序：");
    for (int k = 0; k < 4; k++) printf(" %s=%.2f", cal[k].name, cal[k].bias);
    printf("\n");

    char *words[5] = { "pitch", "roll", "yaw", "imu", "alt" };
    qsort(words, 5, sizeof(words[0]), cmp_str);
    printf("字符串排序：");
    for (int k = 0; k < 5; k++) printf("%s ", words[k]);
    printf("\n");

    printf("提示：qsort 不稳定（相等元素顺序不保证），要稳定排序得自己带序号比较\n");
    return 0;
}
