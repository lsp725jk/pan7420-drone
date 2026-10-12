/**
 * ch10_ex10.c —— 内存受限环境：8KB RAM 意味着什么
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FAKE_RAM_SIZE 8192

static uint8_t  g_ram[FAKE_RAM_SIZE];        /**
 * 模拟那 8KB
 */
static size_t   g_used = 0;

/* 一个极简的"栈式分配器"：只能整体回退，不能单独释放 —— 因此不会有碎片 */
static void *arena_alloc(size_t n, size_t align)
{
    size_t off = (g_used + (align - 1)) & ~(align - 1);
    if (off + n > FAKE_RAM_SIZE) return NULL;                 /* 分配失败，返回 NULL */
    g_used = off + n;
    return &g_ram[off];
}

/* 对比：一个"会释放"的分配器就必然面对碎片 */
#define NBLK 6
static size_t g_blk_off[NBLK];
static size_t g_blk_len[NBLK];
static int    g_blk_used[NBLK];

static int first_fit(size_t n)
{
    size_t off = 0;
    for (int k = 0; k < NBLK; k++) {
        off += g_blk_len[k];
        if (!g_blk_used[k] && g_blk_len[k] >= n) return k;
    }
    return -1;
}

int main(void)
{
    printf("模拟 RAM 大小 = %d 字节\n\n", FAKE_RAM_SIZE);

                /* ---------- 1) 静态分配：编译期就定下来，最安全 ---------- */
    static float imu_fifo[200];                      /* 800 字节，直接进 .bss */
    static float rot_mat[3][3];                      /* 36 字节 */
    printf("静态分配：\n");
    printf("  imu_fifo[200]  = %zu 字节（占 RAM 的 %.1f%%）\n",
           sizeof(imu_fifo), 100.0 * sizeof(imu_fifo) / FAKE_RAM_SIZE);
    printf("  rot_mat[3][3]  = %zu 字节\n", sizeof(rot_mat));
    printf("  优点：不会失败、不会碎片、链接时就知道够不够（编不过就是不够）。\n\n");

                /* ---------- 2) 栈式分配（arena）：启动时一次性划好，运行期只用不还 ---------- */
    memset(g_ram, 0, sizeof(g_ram));
    g_used = 0;
    printf("栈式分配（arena）：\n");
    void *p1 = arena_alloc(512, 4);
    void *p2 = arena_alloc(300, 8);
    void *p3 = arena_alloc(1024, 4);
    printf("  分配 512 -> %s\n", p1 ? "成功" : "失败");
    printf("  分配 300 -> %s\n", p2 ? "成功" : "失败");
    printf("  分配 1024 -> %s\n", p3 ? "成功" : "失败");
    printf("  共用了 %zu 字节\n", g_used);

                /* ---------- 3) 分配失败：嵌入式里必须处理，不能假设它一定成功 ---------- */
    printf("\n分配失败的处理：\n");
    void *big = arena_alloc(9000, 4);
    printf("  请求 9000 字节 -> %s （因为池子只有 8192）\n", big ? "成功" : "NULL");
    if (big == NULL) {
        printf("  正确做法：不要把结果当有效指针用；\n");
        printf("    要么降级（用更小的缓冲区）、要么报错、要么复位。\n");
    }

                /* ---------- 4) 碎片：为什么会"总量够但申请不到" ---------- */
    printf("\n碎片演示（6 块 1024 字节的堆，交替占用与释放）：\n");
    for (int k = 0; k < NBLK; k++) { g_blk_len[k] = 1024; g_blk_used[k] = 0; }
                /* 先占满奇数块，再释放它们 */
    for (int k = 0; k < NBLK; k += 2) g_blk_used[k] = 1;
    for (int k = 0; k < NBLK; k += 2) g_blk_used[k] = 0;
    int free_total = 0;
    for (int k = 0; k < NBLK; k++) if (!g_blk_used[k]) free_total += (int)g_blk_len[k];
    printf("  现在总空闲 = %d 字节（6 块全部空闲）\n", free_total);
                /* 占掉偶数块，把空间切成碎片 */
    for (int k = 1; k < NBLK; k += 2) g_blk_used[k] = 1;
    free_total = 0;
    for (int k = 0; k < NBLK; k++) if (!g_blk_used[k]) free_total += (int)g_blk_len[k];
    printf("  占掉 3 块后：总空闲还是 %d 字节，但都是 1024 的小块\n", free_total);
    int idx = first_fit(2048);
    printf("  现在申请一块 2048 连续字节 -> %s\n", (idx >= 0) ? "成功" : "失败（碎片）");
    printf("  -> 总量够、但拿不到 —— 这就是碎片，也是嵌入式不用 malloc 的主因。\n\n");

    printf("给飞控的四条经验：\n");
    printf("  1) 所有缓冲区都静态分配，链接脚本里把栈定死（1KB 或 2KB）；\n");
    printf("  2) 编译不过或链接报「region RAM overflowed」是好事 —— 比运行时崩溃好；\n");
    printf("  3) 每次看到 sizeof 大的数组，都问一句「它真的要放 RAM 吗」，\n");
    printf("     查表数据加 const 就能留在 Flash；\n");
    printf("  4) 栈深要留够：中断嵌套 + printf + 浮点库都会吃栈（见 ch10_ex11.c）。\n");
    return 0;
}
