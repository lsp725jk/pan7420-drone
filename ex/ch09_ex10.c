/**
 * ch09_ex10.c —— bsearch：在【已排序】的数组里做二分查找
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Entry {
    int         key;
    const char *text;
};

/**
 * 比较函数 1：按整数 key 比。数组必须按 key 升序排好
 */
static int cmp_by_key(const void *key, const void *elem)
{
    int k = *(const int *)key;
    const struct Entry *e = (const struct Entry *)elem;
    return (k > e->key) - (k < e->key);
}
/* 比较函数 2：按字符串 text 比。数组必须按 text 升序排好 */
static int cmp_by_text(const void *key, const void *elem)
{
    const char *k = (const char *)key;
    const struct Entry *e = (const struct Entry *)elem;
    return strcmp(k, e->text);
}

/* 按 key 排好的表 —— 给 cmp_by_key 用 */
static const struct Entry by_key[] = {
    { 10, "idle"    },
    { 20, "arm"     },
    { 30, "takeoff" },
    { 40, "hold"    },
    { 50, "land"    },
};
/* 同一批数据，换成按 text 排 —— 给 cmp_by_text 用 */
static const struct Entry by_text[] = {
    { 20, "arm"     },
    { 40, "hold"    },
    { 10, "idle"    },
    { 50, "land"    },
    { 30, "takeoff" },
};

int main(void)
{
    const size_t n = 5;

                /* 1) 按整数 key 查：命中与不命中 */
    static const int probes[] = { 10, 25, 40, 99 };
    for (int k = 0; k < 4; k++) {
        int q = probes[k];
        const struct Entry *hit =
            (const struct Entry *)bsearch(&q, by_key, n, sizeof(by_key[0]), cmp_by_key);
        printf("按 key 查 %2d : %s\n", q, hit ? hit->text : "没找到");
    }

                /* 2) 按字符串查：必须用"按 text 排好"的那张表 */
    static const char *texts[] = { "hold", "imu" };
    for (int k = 0; k < 2; k++) {
        const struct Entry *hit =
            (const struct Entry *)bsearch(texts[k], by_text, n, sizeof(by_text[0]), cmp_by_text);
        printf("按名字查 %-6s : %s\n", texts[k], hit ? "找到了" : "没找到");
    }

                /* 3) 变式：拿到的指针就是数组内部的元素地址，可以反算下标 */
    int q = 30;
    const struct Entry *hit =
        (const struct Entry *)bsearch(&q, by_key, n, sizeof(by_key[0]), cmp_by_key);
    if (hit) printf("命中 key=30 的元素下标 = %d\n", (int)(hit - by_key));

                /* 4) 坑：用"没有按该规则排序"的表去查 */
    const struct Entry *wrong =
        (const struct Entry *)bsearch("hold", by_key, n, sizeof(by_key[0]), cmp_by_text);
    printf("拿按 key 排的表去按 text 查 \"hold\" : %s\n",
           wrong ? wrong->text : "没找到");
    printf("↑ 表里明明有 hold，却查不到 —— 这就是\"排序规则和比较规则不一致\"的后果\n");
    return 0;
}
