/**
 * ch02_ex2_stdint.c —— 固定宽度类型（stdint.h）：单片机为什么要用它
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>       /**
 * 提供 PRIu32 这类"跨平台的格式串宏"
 */

/**
 * 用固定宽度类型写一个简单的位域结构：32 位状态字。 这种"一个 32 位整数按位拆开当开关用"的写法，在飞控、寄存器配置里到处都是。
 */
typedef union {
    uint32_t raw;                           /* 整个 32 位一起访问 */
    struct {
        uint32_t armed     : 1;                     /* 位段（bit-field）：占 1 位 */
        uint32_t failsafe  : 1;
        uint32_t mode      : 3;                     /* 占 3 位，可存 0..7 */
        uint32_t reserved  : 27;
    } bits;                                 /* 按位分别访问 */
} status_word_t;

int main(void)
{
                /* ---- 1. 固定宽度类型确实就是那么宽 ---- */
    printf("---- 固定宽度类型的 sizeof ----\n");
    printf("int8_t   %zu  uint8_t   %zu\n", sizeof(int8_t), sizeof(uint8_t));
    printf("int16_t  %zu  uint16_t  %zu\n", sizeof(int16_t), sizeof(uint16_t));
    printf("int32_t  %zu  uint32_t  %zu\n", sizeof(int32_t), sizeof(uint32_t));
    printf("int64_t  %zu  uint64_t  %zu\n", sizeof(int64_t), sizeof(uint64_t));
    printf("intptr_t %zu  (能装下 void* 的整型)\n", sizeof(intptr_t));

        /* ---- 2. 用 PRId32 / PRIu32 打印，而不是靠猜 ---- 直接写 printf("%ld", (long)x) 在 Windows 上会出错，因为 long 只有 4 字节； 写 PRId32 让标准库自己决定该用什么长度修饰符。 */
    int32_t  a = INT32_MIN;
    uint32_t b = UINT32_MAX;
    uint64_t c = UINT64_MAX;
    printf("---- 跨平台的打印方式 ----\n");
    printf("INT32_MIN  = %" PRId32 "\n", a);
    printf("UINT32_MAX = %" PRIu32 "\n", b);
    printf("UINT64_MAX = %" PRIu64 "\n", c);
    printf("十六进制   = 0x%08" PRIX32 "\n", b);

                /* ---- 3. 定长类型 + 位运算 = 嵌入式里最常见的组合 ---- */
    status_word_t st;
    st.raw = 0;                             /* 先把整字清零，别忘这一步 */
    st.bits.armed = 1;
    st.bits.mode  = 5;
    printf("---- 位段与整字互看 ----\n");
    printf("st.raw = 0x%08" PRIX32 "  (二进制 0b1011 = armed|mode)\n", st.raw);
    printf("armed=%" PRIu32 "  failsafe=%" PRIu32 "  mode=%" PRIu32 "\n",
           (uint32_t)st.bits.armed, (uint32_t)st.bits.failsafe,
           (uint32_t)st.bits.mode);

                /* 同一个 32 位寄存器，用掩码手工读一个字段： 先右移到最低位，再与上掩码。这是不依赖位段的标准做法。 */
    uint32_t mode_field = (st.raw >> 2) & 0x7u;
    printf("用移位+掩码读出 mode = %" PRIu32 "  (和位段读出来的一致)\n",
           mode_field);

                /* ---- 4. 定长类型在"回绕"上的确定性 ---- uint32_t 溢出一定回绕（取模 2^32），这是标准保证的， 所以在做计时器差值、协议校验时非常可靠。 */
    uint32_t tick = 0xFFFFFFFEu;
    printf("---- 无符号回绕是可预测的 ----\n");
    printf("0x%08" PRIX32 " + 1 = 0x%08" PRIX32 "\n", tick, tick + 1u);
    printf("0x%08" PRIX32 " + 2 = 0x%08" PRIX32 "\n", tick, tick + 2u);
    printf("(uint32_t)0 - 1 = %" PRIu32 "\n", (uint32_t)0 - 1u);
    return 0;
}
