/**
 * ch03_ex19_bitfield_ops.c —— 位段的读写与"跨字节字段"的坑
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
 * 有符号位段：读出来可能是负数
 */
typedef struct {
    int32_t s3 : 3;                     /**
     * 3 位有符号：范围 -4..3
     */
    int32_t u3 : 3;                     /**
     * 3 位无符号：范围 0..7（因为 int32_t 是有符号的， 这一个字段其实仍然是有符号的 —— 见下面的实测）
     */
    int32_t rest : 26;
} signed_bits_t;

/* 明确用无符号底层类型 */
typedef struct {
    uint32_t mode  : 3;
    uint32_t flag  : 1;
    uint32_t level : 8;
    uint32_t pad   : 20;
} unsigned_bits_t;

int main(void)
{
    printf("---- 1. 位段写入超范围会静默截断 ----\n");
    unsigned_bits_t u;
    u.mode = 0;
    u.flag = 0;
    u.level = 0;
    u.pad = 0;
    u.mode = 5;
    printf("  mode（3 位）写 5  -> 读出来 %u\n", (unsigned)u.mode);
    u.mode = 9;                         /* 9 = 0b1001，3 位装不下 */
    printf("  mode（3 位）写 9  -> 读出来 %u（9 & 0x7 = 1）\n", (unsigned)u.mode);
    u.mode = 15;                        /* 0b1111 */
    printf("  mode（3 位）写 15 -> 读出来 %u（15 & 0x7 = 7）\n", (unsigned)u.mode);
    printf("  【症状】赋值不报错、读回来变了 —— 数值悄悄被截断。\n");

    u.level = 300;                      /* 8 位装不下 300 */
    printf("  level（8 位）写 300 -> 读出来 %u（300 & 0xFF = 44）\n",
           (unsigned)u.level);

    printf("---- 2. 位段赋值会读-改-写整个存储单元 ----\n");
    u.mode = 1;
    u.flag = 1;
    u.level = 0x5A;
    printf("  设完三个字段后 raw 视图：");
    uint32_t raw = 0;
                /* 用 memcpy 把位段结构体当成字节看（避免类型双关的未定义行为） */
    memcpy(&raw, &u, sizeof(raw));
    printf("0x%08X\n", raw);
    printf("  拆出来：mode=%u flag=%u level=0x%02X\n",
           (unsigned)u.mode, (unsigned)u.flag, (unsigned)u.level);
    printf("  【注意】每次给字段赋值都会生成一次「读-改-写」，\n");
    printf("  所以在中断共享的场景里要一次改完再一次写回（见 03.10）。\n");

    printf("---- 3. 有符号位段的符号扩展 ----\n");
    signed_bits_t s;
    s.rest = 0;
    s.s3 = 3;   printf("  s3（3 位，有符号）写 3  -> %d\n", (int)s.s3);
    s.s3 = 4;   printf("  s3（3 位，有符号）写 4  -> %d（4 = 0b100，最高位是符号位）\n",
                       (int)s.s3);
    s.s3 = -1;  printf("  s3（3 位，有符号）写 -1 -> %d\n", (int)s.s3);
    s.s3 = -4;  printf("  s3（3 位，有符号）写 -4 -> %d\n", (int)s.s3);
    s.s3 = 7;   printf("  s3（3 位，有符号）写 7  -> %d（被当成 -1）\n", (int)s.s3);
    printf("  【结论】位段字段一定要用 **无符号底层类型**（uint32_t），\n");
    printf("          否则读出来可能是你不期望的负数。\n");

    printf("---- 4. 跨字节字段：移位掩码 vs 位段 ----\n");
                /* 协议里"12 位字段跨两个字节"的情况，用移位掩码最稳 */
    uint8_t buf[2] = { 0xAB, 0x0C };                /* 小端：低字节在前 */
    uint16_t raw16 = (uint16_t)(buf[0] | (uint16_t)(buf[1] << 8));
    uint16_t fieldA = (uint16_t)(raw16 & 0x0FFFu);                /* 低 12 位 */
    uint16_t fieldB = (uint16_t)((raw16 >> 12) & 0x0Fu);          /* 高 4 位 */
    printf("  字节 AB 0C 按小端读成 0x%04X\n", raw16);
    printf("    低 12 位  = 0x%03X（%u）\n", fieldA, (unsigned)fieldA);
    printf("    高 4 位   = 0x%X（%u）\n", fieldB, (unsigned)fieldB);
    printf("  反向写回：\n");
    uint16_t new_raw = (uint16_t)((fieldA & 0x0FFFu) | ((fieldB & 0x0Fu) << 12));
    printf("    拼回去 = 0x%04X，字节序列 = %02X %02X\n",
           new_raw, (unsigned)(new_raw & 0xFF), (unsigned)((new_raw >> 8) & 0xFF));
    printf("  【建议】协议解析一律用移位掩码；位段只用在「本编译器 + 本芯片」内部。\n");

    printf("---- 5. 位段的宽度上限 ----\n");
    printf("  位段宽度不能超过底层类型宽度，写 uint32_t a : 40 会报错。\n");
    printf("  不同编译器对「能否跨存储单元」的处理也不同 —— 又一个可移植性理由。\n");
    return 0;
}
