// ch06_ex3.cpp —— realloc：改大小，注意三件事（临时指针、老内容、失败处理）
// 编译：check.bat ch06_ex3.cpp c++latest
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main()
{
    int* p = (int*)malloc(4 * sizeof(int));
    for (int i = 0; i < 4; ++i) p[i] = (i + 1) * 100;
    printf("[1] 初始 4 个元素: ");
    for (int i = 0; i < 4; ++i) printf("%d ", p[i]);
    printf("  p=%p\n", (void*)p);

        // 扩容：老内容保留，新增部分未初始化
    int* q = (int*)realloc(p, 8 * sizeof(int));
    printf("\n[2] realloc 到 8 个元素:\n");
    printf("    老内容前 4 个: ");
    for (int i = 0; i < 4; ++i) printf("%d ", q[i]);
    printf("\n    新增的 4 个（未初始化，垃圾值）: ");
    for (int i = 4; i < 8; ++i) printf("%d ", q[i]);
    printf("\n    p=%p q=%p  相同? %s\n", (void*)p, (void*)q, (p == q) ? "是（原地扩展）" : "否（搬到了新地址）");

        // 正确写法：把 realloc 的结果先放进临时变量，失败了原来的内存还在
    int* tmp = (int*)realloc(q, 16 * sizeof(int));
    if (tmp != nullptr) {
        q = tmp;
        printf("\n[3] 正确姿势: tmp=realloc(...); if(tmp) q=tmp;\n");
        printf("    扩展到 16 个元素成功，q=%p\n", (void*)q);
    }

        // 错误写法（千万不要这么写）：
        // p = (int*)realloc(p, ...);
        // 如果 realloc 失败返回 NULL，p 就变成 NULL，原来那块内存谁也找不到了 → 内存泄漏

        // 缩容：也能用来缩小
    int* r = (int*)realloc(q, 2 * sizeof(int));
    printf("\n[4] 缩容到 2 个元素: %d %d   （前两个保留）\n", r[0], r[1]);

        // realloc(ptr, 0)：**不等于** free(ptr)。C17 起这是「实现定义」行为：可能返回 NULL
// （此时老块是否被释放也不保证），也可能返回不可访问的非空指针。
// 该用法已被 C DR 400 标为废弃，C23 起是未定义行为。要用就直接 free。
    printf("\n[5] realloc(p, 0) 是「实现定义」行为，不等于 free(p)；本机实测返回 NULL，\n");
    printf("    但标准允许此时老块未被释放 —— 这个写法不可移植，别用\n");

        // 真实用途：动态数组翻倍增长（均摊 O(1) 追加）
    printf("\n[6] 动态数组翻倍增长演示:\n");
    int cap = 2, len = 0;
    int* dyn = (int*)malloc(cap * sizeof(int));
    for (int i = 0; i < 9; ++i) {
        if (len == cap) {
            cap *= 2;
            int* nt = (int*)realloc(dyn, cap * sizeof(int));
            if (!nt) { printf("    扩容失败\n"); break; }
            dyn = nt;
            printf("    容量不够，扩容到 %d，新地址 %p\n", cap, (void*)dyn);
        }
        dyn[len++] = i * i;
    }
    printf("    最终 len=%d cap=%d 内容: ", len, cap);
    for (int i = 0; i < len; ++i) printf("%d ", dyn[i]);
    printf("\n");

    free(r);
    free(dyn);
    return 0;
}
