/**
 * ch09_ex23.c —— 【踩坑】qsort 比较函数的两种经典写法错误
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * ---------- 错误写法 1：直接相减 ----------
 */
static int cmp_subtract(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return x - y;                         /* 错：x - y 可能溢出 */
}

/* ---------- 正确写法：只产生 -1 / 0 / 1 ---------- */
static int cmp_safe(const void *a, const void *b)
{
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

/* ---------- 稳定性演示用的记录 ---------- */
struct Rec {
    int key;                    /* 排序键 */
    int seq;                    /* 原始序号 —— 用来观察"相等元素的相对顺序" */
    const char *tag;
};
static int cmp_rec_key_only(const void *a, const void *b)
{
    const struct Rec *pa = (const struct Rec *)a;
    const struct Rec *pb = (const struct Rec *)b;
    return (pa->key > pb->key) - (pa->key < pb->key);           /* 只比 key，不管 seq */
}
static int cmp_rec_stable(const void *a, const void *b)
{
    const struct Rec *pa = (const struct Rec *)a;
    const struct Rec *pb = (const struct Rec *)b;
    if (pa->key != pb->key) return (pa->key > pb->key) - (pa->key < pb->key);
    return (pa->seq > pb->seq) - (pa->seq < pb->seq);           /* 相等时用序号兜底 */
}

int main(void)
{
                /* ---------- 1) 相减溢出的现场 ---------- */
    int v[4] = { 2000000000, -2000000000, 0, 100 };
    int w[4];
    memcpy(w, v, sizeof(v));

    qsort(v, 4, sizeof(v[0]), cmp_subtract);
    printf("1) 用 return x - y 排序的结果：");
    for (int k = 0; k < 4; k++) printf("%d ", v[k]);
    printf("\n   （-2000000000 和 2000000000 相减的结果超出 int 范围 —— UB）\n");

    qsort(w, 4, sizeof(w[0]), cmp_safe);
    printf("   用 return (x>y)-(x<y) 的结果：");
    for (int k = 0; k < 4; k++) printf("%d ", w[k]);
    printf("   <-- 正确\n\n");

                /* ---------- 2) qsort 不稳定 ---------- */
    struct Rec r1[5] = {
        { 2, 1, "A" }, { 1, 2, "B" }, { 2, 3, "C" }, { 1, 4, "D" }, { 2, 5, "E" },
    };
    struct Rec r2[5];
    memcpy(r2, r1, sizeof(r1));

    qsort(r1, 5, sizeof(r1[0]), cmp_rec_key_only);
    printf("2) 只按 key 排序（不稳定）：");
    for (int k = 0; k < 5; k++) printf("%s(k=%d,seq=%d) ", r1[k].tag, r1[k].key, r1[k].seq);
    printf("\n");

    qsort(r2, 5, sizeof(r2[0]), cmp_rec_stable);
    printf("   加上 seq 兜底（稳定）：  ");
    for (int k = 0; k < 5; k++) printf("%s(k=%d,seq=%d) ", r2[k].tag, r2[k].key, r2[k].seq);
    printf("\n");

                /* ---------- 3) 变式：用「比较键 + 原下标」一步到位 ---------- */
    printf("\n3) 想让 qsort 稳定的通用办法：结构体里带一个「出生序号」，\n");
    printf("   比较函数里「键相等就比序号」。这样排序结果完全确定、可复现。\n");
    printf("   嵌入式里做「多传感器按时间戳排序」时必须这么做 ——\n");
    printf("   否则同一毫秒的两个采样，顺序每次运行都可能不同。\n");

                /* ---------- 4) 变式：浮点比较不能相减后转 int ---------- */
    printf("\n4) 浮点排序不能写 return (int)(x - y)：\n");
    printf("   两个接近的浮点相减会得到 0.3 这种小数，转成 int 变 0，\n");
    printf("   于是 a==b 和 a<b 分不清。浮点必须写 if (x < y) return -1; ...\n");

                /* ---------- 5) 变式：字符串数组的两级指针 ---------- */
    char *words[3] = { "b", "a", "c" };
    printf("\n5) 排序 char* 数组时，比较函数收到的是 char**（多一层指针）：\n");
    printf("   words 数组本身在 %p，words[0] 指向的字符串在 %p\n",
           (void *)words, (void *)words[0]);
    printf("   所以比较函数里要先解引用一次：const char *const *pa = a;\n");
    return 0;
}
