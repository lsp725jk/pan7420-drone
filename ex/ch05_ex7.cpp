// ch05_ex7.cpp —— 指针运算：p++ / p+n / p1-p2 和容易写错的优先级
// 编译：check.bat ch05_ex7.cpp c++latest
#include <cstdio>

int main()
{
        // ---- 1) 遍历：p++ 移动的是"一个元素"，不是 1 字节 ----
    int a[5] = { 10, 20, 30, 40, 50 };
    int* p = a;                  // 等价于 &a[0]

    printf("[1] 用 ++p 遍历，同时打印地址，看步长:\n");
    for (int i = 0; i < 5; ++i) {
        printf("    p=%p  *p=%2d  (距 a 起点 %td 字节)\n",
               (void*)p, *p, (char*)p - (char*)a);
        ++p;                     // 前进 1 个 int = 4 字节
    }

        // 循环结束后 p 指向"末尾的下一个位置"（合法的指针值，但不能解引用）
    printf("    循环结束后 p - a = %td  （元素个数，不是字节）\n", p - a);
    printf("    此时 p 是 one past the end，可以比较、可以减，但不能 *p\n");

        // ---- 2) 回退和随机访问 ----
    --p;                         // 回到最后一个元素
    printf("\n[2] --p 之后 *p = %d\n", *p);
    printf("    p[-1] = %d      *(p-1) = %d   （负下标合法，只要还在数组里）\n",
           p[-1], *(p - 1));

        // ---- 3) 两个指针相减 ----
    int* q = &a[4];
    int* r = &a[1];
    printf("\n[3] 相减:\n");
    printf("    &a[4] - &a[1] = %td   （元素个数）\n", q - r);
    printf("    字节差         = %td\n", (char*)q - (char*)r);
    printf("    两者关系: 元素个数 × sizeof(int) = %td × %zu = %td\n",
           q - r, sizeof(int), (q - r) * (ptrdiff_t)sizeof(int));

        // ---- 4) p += n 等价于 p = p + n ----
    printf("\n[4] p += n 的等价性:\n");
    int* s1 = a;
    int* s2 = a;
    s1 += 3;
    s2 = s2 + 3;
    printf("    s1 = a; s1 += 3;  → *s1 = %d\n", *s1);
    printf("    s2 = a; s2 = s2+3 → *s2 = %d   两者相等? %s\n",
           *s2, (s1 == s2) ? "是" : "否");

        // ---- 5) *p++ 和 (*p)++ 完全不同（最容易写错的地方）----
    printf("\n[5] 优先级：*p++ 不等于 (*p)++\n");
    int b[3] = { 1, 2, 3 };
    int* t = b;
    int v1 = *t++;               // 先取 *t，再让 t 前进：v1 = b[0], t 指向 b[1]
    printf("    int* t = b; int v1 = *t++;\n");
    printf("      v1 = %d   （取的是原来的 *t）\n", v1);
    printf("      现在 *t = %d  （t 已经前进了）\n", *t);

    int c[3] = { 1, 2, 3 };
    int* u = c;
    int v2 = (*u)++;             // 先把 *u 加 1：v2 = 原值 1，c[0] 变成 2
    printf("    int* u = c; int v2 = (*u)++;\n");
    printf("      v2 = %d   （取的是加之前的值）\n", v2);
    printf("      现在 c[0] = %d, *u = %d  （u 没动，是内容变了）\n", c[0], *u);

    printf("\n    记忆：*p++  →  *(p++)   移动指针\n");
    printf("          (*p)++ →  内容加一（指针不动）\n");
    printf("          ++*p   →  ++(*p)   内容加一并返回新值\n");
    printf("          *++p   →  *(++p)   先移动指针再取值\n");

        // ---- 6) 比较：指向同一数组的指针可以用 < > ----
    printf("\n[6] 比较（只对同一数组内的指针有意义）:\n");
    printf("    &a[1] < &a[4] ? %s\n", (&a[1] < &a[4]) ? "真" : "假");
    printf("    &a[4] > &a[1] ? %s\n", (&a[4] > &a[1]) ? "真" : "假");
    printf("    注意：不同数组/不同变量的指针互相比较是 UB\n");
    return 0;
}
