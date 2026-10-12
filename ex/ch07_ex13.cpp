// ch07_ex13.cpp —— 位域 vs 显式移位掩码：什么时候该用哪个
// 编译：check.bat ch07_ex13.cpp c++latest
#include <cstdio>
#include <cstring>
#include <cstdint>

// 做法 A：位域结构体（可读性好，但布局实现定义）
struct CtlBits {
    uint32_t enable   : 1;
    uint32_t mode     : 3;
    uint32_t prescale : 4;
    uint32_t count    : 16;
};

// 做法 B：裸 uint32_t + 显式掩码（布局完全可控，跨平台安全）
enum {
    CTL_ENABLE_SHIFT   = 0,  CTL_ENABLE_MASK   = 0x1u,
    CTL_MODE_SHIFT     = 1,  CTL_MODE_MASK     = 0x7u,
    CTL_PRESCALE_SHIFT = 4,  CTL_PRESCALE_MASK = 0xFu,
    CTL_COUNT_SHIFT    = 8,  CTL_COUNT_MASK    = 0xFFFFu
};

// 通用读写辅助函数（嵌入式里几乎每个工程都有这两个）
static inline void reg_write(uint32_t* reg, int shift, uint32_t mask, uint32_t val)
{
    *reg = (*reg & ~(mask << shift)) | ((val & mask) << shift);
}
static inline uint32_t reg_read(const uint32_t* reg, int shift, uint32_t mask)
{
    return (*reg >> shift) & mask;
}

int main()
{
    printf("[1] 同一份配置，两种做法的结果应该一致:\n");
    const uint32_t EN = 1, MODE = 5, PRE = 9, CNT = 0x1234;

        // 做法 A
    struct CtlBits a;
    memset(&a, 0, sizeof(a));
    a.enable = EN; a.mode = MODE; a.prescale = PRE; a.count = CNT;
    uint32_t rawA;
    memcpy(&rawA, &a, sizeof(rawA));
    printf("    位域写法      : raw = 0x%08X\n", rawA);

        // 做法 B
    uint32_t rawB = 0;
    reg_write(&rawB, CTL_ENABLE_SHIFT,   CTL_ENABLE_MASK,   EN);
    reg_write(&rawB, CTL_MODE_SHIFT,     CTL_MODE_MASK,     MODE);
    reg_write(&rawB, CTL_PRESCALE_SHIFT, CTL_PRESCALE_MASK, PRE);
    reg_write(&rawB, CTL_COUNT_SHIFT,    CTL_COUNT_MASK,    CNT);
    printf("    掩码写法      : raw = 0x%08X\n", rawB);
    printf("    两者相同? %s\n", (rawA == rawB) ? "相同" : "不同");

    printf("\n[2] 两种做法的读回:\n");
    printf("    位域读回  : enable=%u mode=%u prescale=%u count=0x%X\n",
           a.enable, a.mode, a.prescale, a.count);
    printf("    掩码读回  : enable=%u mode=%u prescale=%u count=0x%X\n",
           reg_read(&rawB, CTL_ENABLE_SHIFT, CTL_ENABLE_MASK),
           reg_read(&rawB, CTL_MODE_SHIFT, CTL_MODE_MASK),
           reg_read(&rawB, CTL_PRESCALE_SHIFT, CTL_PRESCALE_MASK),
           reg_read(&rawB, CTL_COUNT_SHIFT, CTL_COUNT_MASK));

    printf("\n[3] 单字段修改（把 mode 从 5 改成 2，其他位不动）:\n");
    uint32_t before = rawB;
    reg_write(&rawB, CTL_MODE_SHIFT, CTL_MODE_MASK, 2);
    printf("    改前 0x%08X → 改后 0x%08X\n", before, rawB);
    printf("    只变了 mode 那 3 位: 0x%X\n", (before ^ rawB));
    printf("    位域写法直接 a.mode = 2; 也可以，但没法像这样「只动一位」地精确控制\n");

    printf("\n[4] 怎么选（结论）:\n");
    printf("    ┌────────────────────┬──────────────┬──────────────┐\n");
    printf("    │ 场景               │ 位域结构体   │ 显式掩码     │\n");
    printf("    ├────────────────────┼──────────────┼──────────────┤\n");
    printf("    │ 可读性             │ 好           │ 一般         │\n");
    printf("    │ 布局可控           │ 差（实现定义）│ 完全可控     │\n");
    printf("    │ 跨平台/跨编译器     │ 危险         │ 安全         │\n");
    printf("    │ 取地址/做指针       │ 不行         │ 可以         │\n");
    printf("    │ 和硬件寄存器对应    │ 很舒服       │ 稍繁琐       │\n");
    printf("    └────────────────────┴──────────────┴──────────────┘\n");
    printf("    一句话：只在本工程内部用 → 位域；要收发数据/跨平台 → 掩码\n");
    return 0;
}
