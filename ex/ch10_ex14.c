/**
 * ch10_ex14.c —— 编译期断言与内存布局校验
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>

/**
 * ---------- 1) 基本用法：确认类型宽度 ----------
 */
_Static_assert(sizeof(int32_t) == 4, "int32_t 必须是 4 字节");
_Static_assert(sizeof(uint8_t) == 1, "uint8_t 必须是 1 字节");
_Static_assert(CHAR_BIT == 8,        "一个字节必须是 8 位");



/* ---------- 2) 真实场景：校验一个"要写进 Flash 的配置结构体" ---------- */
#pragma pack(push, 1)                    /* 关掉填充，让布局完全可控 */
struct FlashConfig {
    uint16_t magic;                              /* 0x00 */
    uint8_t  version;                            /* 0x02 */
    uint8_t  flags;                              /* 0x03 */
    int16_t  gyro_bias[3];                       /* 0x04 ~ 0x09 */
    uint32_t crc32;                              /* 0x0A ~ 0x0D */
    uint16_t reserved;                           /* 0x0E ~ 0x0F，凑满 16 字节 */
};
#pragma pack(pop)

_Static_assert(sizeof(struct FlashConfig) == 16,
               "FlashConfig 必须是 16 字节（Flash 是按 16 字节页擦的）");
_Static_assert(offsetof(struct FlashConfig, gyro_bias) == 4, "gyro_bias 必须在偏移 4");
_Static_assert(offsetof(struct FlashConfig, crc32) == 10,    "crc32 必须在偏移 10");

/* ---------- 3) 变式：把"数组长度和枚举个数必须一致"也钉住 ---------- */
enum Fault { FAULT_NONE = 0, FAULT_IMU, FAULT_BARO, FAULT_RC, FAULT_COUNT };

static const char *const g_fault_names[FAULT_COUNT] = {
    "无", "IMU", "气压计", "遥控"
};
_Static_assert(sizeof(g_fault_names) / sizeof(g_fault_names[0]) == FAULT_COUNT,
               "故障名字表和枚举个数必须一致 —— 加了枚举忘了加名字，这里就会拦住");

int main(void)
{
    printf("编译期断言全部通过，说明这些「假设」在当前平台上成立：\n");
    printf("  sizeof(int32_t)          = %zu\n", sizeof(int32_t));
    printf("  sizeof(void*)            = %zu （本机 64 位，与单片机的 4 不同）\n",
           sizeof(void *));
    printf("  sizeof(struct FlashConfig) = %zu\n", sizeof(struct FlashConfig));
    printf("  offsetof(gyro_bias)      = %zu\n", offsetof(struct FlashConfig, gyro_bias));
    printf("  offsetof(crc32)          = %zu\n", offsetof(struct FlashConfig, crc32));
    printf("  FAULT_COUNT              = %d\n", FAULT_COUNT);

    printf("\n对比：如果【不】用 #pragma pack(1)，同样是「1+4+1」的结构体会变成：\n");
    struct Unpacked {
        uint8_t  a;
        uint32_t b;
        uint8_t  c;
    };
    printf("  sizeof(struct Unpacked)  = %zu （成员只有 6 字节，多了填充）\n",
           sizeof(struct Unpacked));
    printf("  offsetof(b) = %zu , offsetof(c) = %zu\n",
           offsetof(struct Unpacked, b), offsetof(struct Unpacked, c));
    printf("  一旦这个结构体要写进 Flash / 通过射频发给遥控器，\n");
    printf("  填充字节的【内容】是不确定的，会让 CRC 每次都不一样 —— 极难查。\n");

    printf("\n用法总结：\n");
    printf("  · 头文件里写几行 _Static_assert，等于给编译器下「体检单」；\n");
    printf("  · 尤其要断言：寄存器结构体的大小与偏移、协议包的长度、查表数组的长度；\n");
    printf("  · 它是【零开销】的：不生成任何代码，只在编译期检查。\n");
    return 0;
}
