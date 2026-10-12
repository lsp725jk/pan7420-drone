/**
 * ch04_ex21_ptr_by_value.c —— 指针也是"按值传递"：交换指针和"传指针的指针"
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 只想改"指向的内容"：传一级指针就够了
 */
static void set_value(int *p, int v)
{
    if (p == NULL) {
        return;
    }
    *p = v;                         /* 改的是 p 指向的那块内存 */
}

/* 想让函数改"指针本身"：必须传二级指针 */
static int alloc_and_fill(int **out, int n, int value)
{
    if (out == NULL || n <= 0) {
        return -1;
    }
    int *p = (int *)malloc((size_t)n * sizeof(int));
    if (p == NULL) {
        return -2;
    }
    for (int i = 0; i < n; i++) {
        p[i] = value + i;
    }
    *out = p;                       /* 通过二级指针把新地址交回调用者 */
    return 0;
}

/* 常见坑：只传一级指针，函数里改了指针，外面完全不知道 */
static void alloc_wrong(int *p, int n)
{
    p = (int *)malloc((size_t)n * sizeof(int));           /* 只改了副本！ */
    if (p != NULL) {
        p[0] = 999;
    }
                /* 这块内存泄漏了，而且调用者的指针还是 NULL */
}

/* 更推荐的替代：直接返回指针（简单场景首选） */
static int *alloc_simple(int n, int value)
{
    if (n <= 0) {
        return NULL;
    }
    int *p = (int *)malloc((size_t)n * sizeof(int));
    if (p != NULL) {
        for (int i = 0; i < n; i++) {
            p[i] = value + i;
        }
    }
    return p;
}

int main(void)
{
    printf("---- 1. 改指向的内容：一级指针够用 ----\n");
    int a = 1;
    printf("  调用前 a = %d\n", a);
    set_value(&a, 42);
    printf("  调用后 a = %d（内容被改了）\n", a);

    printf("---- 2. 改指针本身：一级指针无效 ----\n");
    int *q = NULL;
    alloc_wrong(q, 3);
    printf("  alloc_wrong(q, 3) 之后 q 仍然是 %s\n", (q == NULL) ? "NULL" : "非空");
    printf("  【原因】q 是按值传进去的，函数里 p = malloc(...) 只改了副本。\n");
    printf("  【代价】那块 malloc 出来的内存再也没人引用 —— 内存泄漏。\n");

    printf("---- 3. 改指针本身：必须传二级指针 ----\n");
    int *r = NULL;
    int rc = alloc_and_fill(&r, 4, 10);
    printf("  alloc_and_fill(&r, 4, 10) = %d\n", rc);
    if (r != NULL) {
        printf("  r 现在指向 %p，内容是:", (void *)r);
        for (int i = 0; i < 4; i++) {
            printf(" %d", r[i]);
        }
        printf("\n");
        free(r);
        r = NULL;
    }

    printf("---- 4. 更简单：直接返回指针 ----\n");
    int *s = alloc_simple(4, 100);
    if (s != NULL) {
        printf("  alloc_simple(4, 100) 内容:");
        for (int i = 0; i < 4; i++) {
            printf(" %d", s[i]);
        }
        printf("\n");
        free(s);
        s = NULL;
    }
    printf("  【选择建议】\n");
    printf("    a) 只有一个输出指针 -> 直接 return 指针（最清楚）；\n");
    printf("    b) 需要「返回值报错 + 输出指针」 -> 用二级指针或 out 参数；\n");
    printf("    c) 需要同时改多个指针 -> 二级指针数组或者结构体。\n");

    printf("---- 5. 一句话总结 ----\n");
    printf("  C 里**一切参数都是按值传递**，指针也不例外。\n");
    printf("  想让函数改外面的东西，就得把「那个东西的地址」传进去：\n");
    printf("    · 改 int      -> 传 int *\n");
    printf("    · 改 int *    -> 传 int **\n");
    printf("    · 改 int **   -> 传 int ***\n");
    printf("  【实践提醒】超过二级指针就该重构了 —— 通常意味着该抽结构体。\n");
    return 0;
}
