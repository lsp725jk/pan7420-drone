// ch06_ex15.cpp —— 堆为什么会碎片化：嵌入式上"慎用 malloc"的真正原因
// 编译：check.bat ch06_ex15.cpp c++latest
#include <cstdio>
#include <cstdlib>
#include <cstring>

// 一个典型的嵌入式堆：只有几百字节
static const size_t kHeapSize = 512;
static unsigned char g_heap[kHeapSize];   // 用静态数组模拟"堆区"，方便观察

// 极简的"首次适配"分配器：只为演示碎片化，不追求效率
typedef struct Block {
    size_t size;                 // 载荷大小
    int    used;                 // 1 = 已分配
    struct Block* next;
} Block;

static Block* g_head = nullptr;

void heap_init()
{
    g_head = (Block*)g_heap;
    g_head->size = kHeapSize - sizeof(Block);
    g_head->used = 0;
    g_head->next = nullptr;
}

void* my_alloc(size_t n)
{
    n = (n + 7) & ~(size_t)7;                  // 8 字节对齐
    for (Block* b = g_head; b != nullptr; b = b->next) {
        if (!b->used && b->size >= n) {
                        // 如果剩余空间够放一个新块头，就切一刀
            if (b->size >= n + sizeof(Block) + 8) {
                Block* nb = (Block*)((unsigned char*)b + sizeof(Block) + n);
                nb->size = b->size - n - sizeof(Block);
                nb->used = 0;
                nb->next = b->next;
                b->next = nb;
                b->size = n;
            }
            b->used = 1;
            return (unsigned char*)b + sizeof(Block);
        }
    }
    return nullptr;                             // 没有合适的空洞 → 失败
}

void my_free(void* p)
{
    if (!p) return;
    Block* b = (Block*)((unsigned char*)p - sizeof(Block));
    b->used = 0;
        // 简化：不做相邻块合并（真实分配器会合并，但碎片仍然存在）
}

void report(const char* tag)
{
    int freeCount = 0, freeBytes = 0, biggest = 0;
    for (Block* b = g_head; b != nullptr; b = b->next) {
        if (!b->used) {
            freeCount++;
            freeBytes += (int)b->size;
            if ((int)b->size > biggest) biggest = (int)b->size;
        }
    }
    printf("    %-22s 空闲块 %d 个，总空闲 %d 字节，最大连续块 %d 字节\n",
           tag, freeCount, freeBytes, biggest);
}

int main()
{
    printf("[1] 模拟一块只有 %zu 字节的堆:\n", kHeapSize);
    heap_init();
    report("初始");

    printf("\n[2] 交替申请/释放，制造「空洞」:\n");
        // 申请 8 个 32 字节的小块
    void* p[9];
    for (int i = 0; i < 8; ++i) {
        p[i] = my_alloc(32);
        printf("    申请 32 字节 #%d → %s\n", i, p[i] ? "成功" : "失败");
    }
    report("申请 8 个 32 字节后");

        // 只释放奇数号，留下空洞
    for (int i = 1; i < 8; i += 2) my_free(p[i]);
    report("释放奇数号后");

    printf("\n[3] 现在想申请一大块（100 字节）:\n");
    void* big = my_alloc(100);
    printf("    my_alloc(100) → %s\n", big ? "成功" : "失败");
    report("尝试申请 100 字节后");
    printf("    ← 总空闲字节数可能还够，但没有「连续的」100 字节，所以失败。\n");
    printf("      这就是「外部碎片」（external fragmentation）。\n");

    printf("\n[4] 真实分配器会做块合并，情况好一些，但碎片仍然存在：\n");
    printf("    - 分配/释放的尺寸越杂，空洞越碎\n");
    printf("    - 长时间运行的程序，堆会越来越「花」\n");
    printf("    - 单片机上堆通常只有几百字节到几 KB，一次 100 字节的失败就可能让设备重启\n");

    printf("\n[5] 单片机上遇到需要动态内存怎么办（按推荐度排序）:\n");
    printf("    (1) 全静态：编译期就把所有缓冲区开好（数组 + 固定大小，最可靠）\n");
    printf("    (2) 内存池（memory pool）：启动时一次性切好固定大小的块，只借不还种类\n");
    printf("    (3) 环形缓冲区：串口收发这类「先进先出」的场景用固定数组实现\n");
    printf("    (4) 实在要用堆：只在初始化阶段 malloc 一次，运行期不再 malloc/free\n");
    printf("    (5) 避免在中断里 malloc/free：多数实现不是可重入的\n");
    printf("\n    PY32F030 这类 64KB Flash / 8KB RAM 的芯片，通常的做法是 (1)(2)(3)。\n");
    return 0;
}
