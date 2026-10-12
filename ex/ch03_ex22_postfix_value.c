/**
 * ch03_ex22_postfix_value.c —— 后缀自增/自减的返回值：拆解 *(p++) 与 a[i++]
 */
#include <stdio.h>
#include <string.h>

int main(void)
{
    printf("---- 1. 拆解 *(p++) 的每一步 ----\n");
    int arr[4] = { 100, 200, 300, 400 };
    int *p = arr;
    printf("  初始: p 指向 arr[0]，*p = %d\n", *p);

    int v1 = *p++;                      /* 等价于：v1 = *p; p = p + 1; */
    printf("  int v1 = *p++;  -> v1 = %d（旧值），现在 *p = %d\n", v1, *p);

    int v2 = (*p)++;                    /* 等价于：v2 = *p; arr[1] = arr[1] + 1; */
    printf("  int v2 = (*p)++; -> v2 = %d（旧值），现在 *p = %d，arr[1] = %d\n",
           v2, *p, arr[1]);

    p = arr;                            /* 复位 */
    int v3 = ++*p;                      /* 等价于：arr[0] = arr[0] + 1; v3 = arr[0]; */
    printf("  int v3 = ++*p;   -> v3 = %d（新值），arr[0] = %d\n", v3, arr[0]);

    printf("---- 2. 三种写法的对照表 ----\n");
    printf("  写法        读的值        改的对象\n");
    printf("  *p++        当前 *p       指针 p 前进 1\n");
    printf("  (*p)++      当前 *p       指针指向的那个元素 +1\n");
    printf("  ++*p        加 1 后的 *p  指针指向的那个元素 +1\n");
    printf("  记忆：后缀 ++ 的优先级高于 *，所以它先跟 p 结合。\n");

    printf("---- 3. a[i++] 是合法且常用的 ----\n");
    int dst[4] = { 0 };
    int i = 0;
    dst[i++] = 10;                      /* 写 dst[0]，然后 i 变成 1 */
    dst[i++] = 20;                      /* 写 dst[1]，然后 i 变成 2 */
    dst[i++] = 30;
    printf("  三次 dst[i++] = ... 之后：dst = { %d, %d, %d, %d }, i = %d\n",
           dst[0], dst[1], dst[2], dst[3], i);
    printf("  【关键区别】dst[i++] = x 是合法的（i 只在**下标**位置被改一次）；\n");
    printf("             dst[i] = i++ 是 UB（同一个 i 既当下标又被改，见 03.9）。\n");

    printf("---- 4. 字符串遍历：*s++ 的经典写法 ----\n");
    const char *s = "abc";
    const char *q = s;
    int n = 0;
    while (*q) {                        /* 不用 *q++ 也可以，更清楚 */
        n++;
        q++;
    }
    printf("  while (*q) { n++; q++; }  -> 长度 = %d\n", n);
    int n2 = 0;
    q = s;
    while (*q++) {                      /* 用 *q++ ：先取值判断，再前进 */
        n2++;
    }
    printf("  while (*q++) { n2++; }    -> 长度 = %d（等价，但少一行）\n", n2);
    printf("  【提醒】第二种写法在循环结束时 q 已经越过 '\\0' 了 —— \n");
    printf("          如果后面还要用 q，这个差别会咬人。\n");

    printf("---- 5. 一个真实的坑：把 *p++ 写成了 (*p)++ ----\n");
    int data[3] = { 5, 6, 7 };
    int sum = 0;
    int *r = data;
    for (int k = 0; k < 3; k++) {
        sum += *r++;                            /* 取当前值，然后前进 —— 正确 */
    }
    printf("  sum += *r++ 三次 -> sum = %d（正确累加了 5+6+7）\n", sum);
    printf("  如果误写成 sum += (*r)++，指针永远不动，反复读同一个元素：\n");
    int data2[3] = { 5, 6, 7 };
    int sum2 = 0;
    int *r2 = data2;
    for (int k = 0; k < 3; k++) {
        sum2 += (*r2)++;                        /* 错：改的是元素，指针不动 */
    }
    printf("    sum2 = %d, data2 = { %d, %d, %d }（r2 没前进！）\n",
           sum2, data2[0], data2[1], data2[2]);
    printf("  【症状】数据被悄悄改坏，而且循环看起来跑完了。\n");
    return 0;
}
