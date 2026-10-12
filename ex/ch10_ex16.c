/**
 * ch10_ex16.c —— 对齐、紧凑布局与编译器扩展（MSVC 一侧）
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/**
 * ---------- 1) 默认布局：编译器会自动插入填充 ----------
 */
struct Default {
    uint8_t  a;              /* 偏移 0 */
    uint32_t b;              /* 为了 4 字节对齐，偏移变成 4（中间补 3 字节） */
    uint8_t  c;              /* 偏移 8 */
};                   /* 末尾还要补到 4 的倍数 -> 总大小 12 */

/* ---------- 2) 紧凑布局：用 #pragma pack(1) 关掉所有填充 ---------- */
#pragma pack(push, 1)
struct Packed {
    uint8_t  a;
    uint32_t b;              /* 现在偏移就是 1，但访问它是"非对齐访问" */
    uint8_t  c;
};
#pragma pack(pop)

/* ---------- 3) 对齐：__declspec(align(n)) ---------- */
__declspec(align(16)) static uint8_t dma_buf[8];   /* 起始地址强制 16 字节对齐 */

/* ---------- 4) 变式：把对齐要求放在成员上 ---------- */
struct WithAlignedMember {
    uint8_t  head;
    __declspec(align(8)) uint32_t payload;
    uint8_t  tail;
};

int main(void)
{
    printf("=== 默认布局 ===\n");
    printf("  sizeof(struct Default) = %zu\n", sizeof(struct Default));
    printf("  offsetof(a)=%zu  offsetof(b)=%zu  offsetof(c)=%zu\n",
           offsetof(struct Default, a), offsetof(struct Default, b),
           offsetof(struct Default, c));
    printf("  -> b 的偏移从 1 变成 4，中间 3 字节是填充（内容不确定）\n\n");

    printf("=== 紧凑布局 #pragma pack(1) ===\n");
    printf("  sizeof(struct Packed) = %zu\n", sizeof(struct Packed));
    printf("  offsetof(a)=%zu  offsetof(b)=%zu  offsetof(c)=%zu\n",
           offsetof(struct Packed, a), offsetof(struct Packed, b),
           offsetof(struct Packed, c));
    printf("  -> 没有填充，正好 6 字节，可以安全地写进 Flash / 发出去\n");
    printf("  -> 代价：b 是非对齐成员。x86/ARM 对普通内存的非对齐访问\n");
    printf("     （除多字节原子访问和某些外设）通常没问题，但会慢一点；\n");
    printf("     而 Cortex-M0+ 【不支持】非对齐访问外设寄存器（会 HardFault）。\n\n");

    printf("=== 对齐 __declspec(align(16)) ===\n");
    printf("  dma_buf 的地址 = %p ，地址 %% 16 = %zu\n",
           (void *)dma_buf, (size_t)((uintptr_t)dma_buf % 16));
    printf("  -> 对 DMA 描述符、以及要求对齐的外设缓冲，这是必需的\n\n");

    printf("=== 成员级对齐 ===\n");
    printf("  sizeof(struct WithAlignedMember) = %zu\n", sizeof(struct WithAlignedMember));
    printf("  offsetof(payload) = %zu （被强制到 8 的倍数）\n",
           offsetof(struct WithAlignedMember, payload));
    printf("\n=== 实用建议 ===\n");
    printf("  · 只有「要跨设备/跨芯片传输」的结构体才需要 pack(1)；\n");
    printf("    普通内存里的结构体保持默认对齐，访问更快；\n");
    printf("  · pack(1) 和取成员地址要小心：编译器会警告「指针可能未对齐」；\n");
    printf("  · 每条编译器扩展都写清楚是给哪个编译器用的，\n");
    printf("    跨 MSVC/GCC/ARMCC 的工程要用宏包一层：\n");
    printf("      #if defined(_MSC_VER)  -> __declspec(align(n))\n");
    printf("      #elif defined(__GNUC__) -> __attribute__((aligned(n)))\n");
    return 0;
}
