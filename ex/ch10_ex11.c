/**
 * ch10_ex11.c —— 栈有多小：递归为什么在单片机上很危险
 */
#include <stdio.h>
#include <stdint.h>

#define DEPTH 64

static char *g_addr[DEPTH];

/**
 * 每次调用都记录自己的局部变量地址，返回后累加
 */
static long recurse(int depth, long acc)
{
    char local[32];                               /* 故意加 32 字节的局部数组 */
    local[0] = (char)depth;
    if (depth < DEPTH) {
        g_addr[depth] = local;
        acc = recurse(depth + 1, acc + local[0]);
    }
    return acc;
}

/* 迭代版：同样的事，但栈占用是常数 */
static long iterate(int n)
{
    char local[32];
    long acc = 0;
    for (int k = 0; k < n; k++) { local[0] = (char)k; acc += local[0]; }
    return acc;
}

int main(void)
{
    printf("PC 上先测一个「每层递归要吃多少栈」的参考值：\n");
    long r = recurse(0, 0);
    printf("  递归 %d 层的结果 = %ld\n", DEPTH, r);

                /* 相邻两层局部变量地址之差，约等于一层的栈帧大小 */
    long diffs = 0;
    int  n = 0;
    for (int k = 1; k < DEPTH; k++) {
        long d = (long)(g_addr[k - 1] - g_addr[k]);
        if (d > 0) { diffs += d; n++; }
    }
    printf("  实测每层栈帧约 %ld 字节（PC 的 x64 调用约定）\n\n", n ? diffs / n : 0);

    printf("迭代版结果 = %ld（栈占用是常数）\n\n", iterate(DEPTH));

    printf("换到 PY32F030（Cortex-M0+，8KB RAM）上估算：\n");
    printf("  · M0+ 上一个栈帧（返回地址 + 若干寄存器 + 局部变量）通常 16~48 字节；\n");
    printf("  · 假设 32 字节/层，栈只给 1KB，那么最多约 %d 层；\n", 1024 / 32);
    printf("  · 再算上中断压栈 32 字节、printf 内部可能吃 100~200 字节，\n");
    printf("    实际安全深度往往只有几十层。\n\n");

    printf("所以：\n");
    printf("  · 单片机上【不要用递归】，尤其不要用「深度取决于输入」的递归\n");
    printf("    （比如递归解析协议包、递归求阶乘）；\n");
    printf("  · 栈溢出不会报错，它表现为「莫名其妙的 HardFault / 变量值被改 / 跑飞」，\n");
    printf("    因为栈是向下生长的，溢出会踩到栈下面别的数据；\n");
    printf("  · 排查手段：在栈顶和栈底放魔术字（0xDEADBEEF）定期检查，\n");
    printf("    或者直接看 map 文件里 __initial_sp 和栈区间的地址。\n");
    return 0;
}
