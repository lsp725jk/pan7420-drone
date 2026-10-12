// ch06_ex2.cpp —— calloc：申请 + 清零，以及和 malloc 的对比
// 编译：check.bat ch06_ex2.cpp c++latest
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main()
{
        // calloc 的两个参数是"个数"和"每个的大小"（顺序和 malloc 的写法正好相反）
    int* c = (int*)calloc(5, sizeof(int));
    printf("calloc(5, sizeof(int)) 之后:\n  ");
    for (int i = 0; i < 5; ++i) printf("%d ", c[i]);
    printf("\n  ← 全是 0（calloc 会清零）\n");

    int* m = (int*)malloc(5 * sizeof(int));
    printf("\nmalloc(5*sizeof(int)) 之后（同一块堆的前后关系可能不同）:\n  ");
    for (int i = 0; i < 5; ++i) printf("%d ", m[i]);
    printf("\n  ← 不保证是 0（这次恰好是 0，是运气/是刚被别人清零过）\n");

        // 想用 malloc 达到 calloc 的效果：加一步 memset
    int* mz = (int*)malloc(5 * sizeof(int));
    memset(mz, 0, 5 * sizeof(int));
    printf("\nmalloc + memset(...,0,...) 之后:\n  ");
    for (int i = 0; i < 5; ++i) printf("%d ", mz[i]);
    printf("\n  ← 等价于 calloc，但慢一点点（要额外走一遍清零）\n");

        // 为什么嵌入式里偏爱 calloc：结构体字段不用逐个初始化
    struct Cfg { int id; int speed; char name[8]; };
    struct Cfg* cfg = (struct Cfg*)calloc(1, sizeof(struct Cfg));
    printf("\ncalloc 出来的结构体：id=%d speed=%d name=\"%s\"\n", cfg->id, cfg->speed, cfg->name);
    printf("  ← 不用写 cfg->id = 0; cfg->speed = 0; ... 全部自动清零\n");

        // calloc 的两个参数是相乘关系，写错顺序不影响结果，但溢出检查要注意
    printf("\ncalloc(1, sizeof(struct Cfg)) = %zu 字节\n", sizeof(struct Cfg));
    printf("calloc 在内部会检查 个数*大小 是否溢出，比手写 malloc(n*size) 安全一点\n");

    free(c); free(m); free(mz); free(cfg);
    return 0;
}
