/**
 * ch02_ex30_hex_bytes.c —— 用十六进制调试字节数据：嵌入式的日常
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
 * 十六进制转储（hexdump）：每一行 8 个字节
 */
static void hexdump(const char *tag, const uint8_t *data, size_t len)
{
    printf("  %s (%zu 字节):\n", tag, len);
    for (size_t i = 0; i < len; i += 8) {
        printf("    %04zu:", i);
        for (size_t j = i; j < i + 8 && j < len; j++) {
            printf(" %02X", data[j]);
        }
        printf("\n");
    }
}

int main(void)
{
    printf("---- 1. 一个假的协议帧 ----\n");
                /* 帧头 0xA5 0x5A + 命令 + 长度 + 数据 + 校验和 */
    uint8_t frame[10] = { 0xA5, 0x5A, 0x03, 0x04, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00 };
    uint8_t sum = 0;
    for (int i = 0; i < 8; i++) {
        sum = (uint8_t)(sum + frame[i]);                        /* 累加和校验 */
    }
    frame[8] = sum;
    hexdump("frame", frame, sizeof(frame));

    printf("---- 2. 逐个字段解析 ----\n");
    uint16_t head = (uint16_t)((frame[0] << 8) | frame[1]);           /* 大端帧头 */
    printf("  帧头   : 0x%04X（期望 0xA55A）\n", head);
    printf("  命令   : 0x%02X\n", frame[2]);
    printf("  长度   : %u\n", (unsigned)frame[3]);
    uint32_t payload = ((uint32_t)frame[4] << 24) | ((uint32_t)frame[5] << 16) |
                       ((uint32_t)frame[6] << 8)  |  (uint32_t)frame[7];
    printf("  数据   : 0x%08X（大端解释）\n", payload);
    printf("  校验和 : 0x%02X（累加和 = %u）\n", frame[8], (unsigned)sum);

    printf("---- 3. 有符号 vs 无符号看同一批字节 ----\n");
    printf("  按 uint8_t 打印: ");
    for (int i = 0; i < 10; i++) {
        printf("%02X ", frame[i]);
    }
    printf("\n  按 int8_t 打印 : ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", (int)(int8_t)frame[i]);
    }
    printf("\n  【结论】0xA5 在 int8_t 里是 -91 —— 这就是字节流必须用无符号的原因。\n");

    printf("---- 4. 常见字节模式的识别 ----\n");
    uint8_t v1 = 0x00, v2 = 0xFF, v3 = 0x80, v4 = 0x7F;
    printf("  0x%02X -> 二进制 ", v1);
    for (int i = 7; i >= 0; i--) { putchar((v1 & (1u << i)) ? '1' : '0'); }
    printf("  （全 0）\n");
    printf("  0x%02X -> 二进制 ", v2);
    for (int i = 7; i >= 0; i--) { putchar((v2 & (1u << i)) ? '1' : '0'); }
    printf("  （全 1）\n");
    printf("  0x%02X -> 二进制 ", v3);
    for (int i = 7; i >= 0; i--) { putchar((v3 & (1u << i)) ? '1' : '0'); }
    printf("  （最高位 1，int8_t 下是 -128）\n");
    printf("  0x%02X -> 二进制 ", v4);
    for (int i = 7; i >= 0; i--) { putchar((v4 & (1u << i)) ? '1' : '0'); }
    printf("  （int8_t 下是 127，有符号的最大值）\n");

    printf("---- 5. 实用建议 ----\n");
    printf("  a) 字节流一律 uint8_t；打印一律 %%02X；\n");
    printf("  b) 解析多字节字段时先转 uint32_t 再移位（见 03.4 的坑）；\n");
    printf("  c) 校验和/CRC 用无符号累加，别用 char；\n");
    printf("  d) 调试时先 hexdump 整帧，再逐字段解析 —— 顺序反了会白折腾。\n");
    return 0;
}
