/**
 * ch09_ex26.c —— 【变式】stdint.h 的定宽类型：手写协议打包/解包
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FRAME_LEN  12

/**
 * ---------- 小端打包：低字节在前（ARM/x86 都是小端） ----------
 */
static void put_u16(uint8_t *p, uint16_t v) { p[0] = (uint8_t)(v & 0xFF); p[1] = (uint8_t)(v >> 8); }
static void put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v & 0xFF); p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF); p[3] = (uint8_t)((v >> 24) & 0xFF);
}
static uint16_t get_u16(const uint8_t *p) { return (uint16_t)(p[0] | ((uint16_t)p[1] << 8)); }
static uint32_t get_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* ---------- 帧格式（手工约定，和结构体布局无关） ---------- 偏移 0    : 帧头 0xA5 偏移 1    : 版本 偏移 2..3 : 序号（u16，小端） */
#define OFF_HEAD   0
#define OFF_VER    1
#define OFF_SEQ    2
#define OFF_GX     4
#define OFF_GY     6
#define OFF_TS     8

static void frame_pack(uint8_t *buf, uint8_t ver, uint16_t seq,
                       int16_t gx, int16_t gy, uint32_t ts)
{
    memset(buf, 0, FRAME_LEN);
    buf[OFF_HEAD] = 0xA5;
    buf[OFF_VER]  = ver;
    put_u16(buf + OFF_SEQ, seq);
    put_u16(buf + OFF_GX,  (uint16_t)gx);
    put_u16(buf + OFF_GY,  (uint16_t)gy);
    put_u32(buf + OFF_TS,  ts);
}

int main(void)
{
    uint8_t frame[FRAME_LEN];

    printf("=== 打包 ===\n");
    frame_pack(frame, 2, 1000, -1234, 5678, 0x11223344u);
    printf("字节流：");
    for (int k = 0; k < FRAME_LEN; k++) printf("%02X ", frame[k]);
    printf("\n");

    printf("\n=== 解包 ===\n");
    if (frame[OFF_HEAD] != 0xA5) { printf("帧头错\n"); return 1; }
    printf("版本      = %u\n", frame[OFF_VER]);
    printf("序号      = %u\n", get_u16(frame + OFF_SEQ));
    printf("陀螺 X    = %d\n", (int16_t)get_u16(frame + OFF_GX));
    printf("陀螺 Y    = %d\n", (int16_t)get_u16(frame + OFF_GY));
    printf("时间戳    = 0x%08X\n", get_u32(frame + OFF_TS));

                /* ---------- 关键对比：同样的数据，用结构体直接「强转」会怎样 ---------- */
    printf("\n=== 对比：如果用结构体直接映射 ===\n");
    struct FrameNaive {
        uint8_t  head;
        uint8_t  ver;
        uint16_t seq;
        int16_t  gx;
        int16_t  gy;
        uint32_t ts;
    };
    printf("sizeof(struct FrameNaive) = %zu 字节，而协议帧是 %d 字节\n",
           sizeof(struct FrameNaive), FRAME_LEN);
    printf("  为什么不一样？看各成员的偏移：\n");
    printf("    head=0 ver=1 seq=%zu gx=%zu gy=%zu ts=%zu\n",
           (size_t)((char *)&((struct FrameNaive *)0)->seq - (char *)0),
           (size_t)((char *)&((struct FrameNaive *)0)->gx  - (char *)0),
           (size_t)((char *)&((struct FrameNaive *)0)->gy  - (char *)0),
           (size_t)((char *)&((struct FrameNaive *)0)->ts  - (char *)0));
    printf("  编译器为了让 uint16_t/uint32_t 对齐，插入了填充字节。\n");
    printf("  所以「把收到的字节流强转成结构体」是错的 —— 偏移全对不上。\n");

                /* ---------- 变式：大端（网络字节序）打包 ------ */
    printf("\n=== 变式：如果对端要的是大端 ===\n");
    uint8_t be[4];
    uint32_t v = 0x11223344u;
    be[0] = (uint8_t)(v >> 24); be[1] = (uint8_t)(v >> 16);
    be[2] = (uint8_t)(v >> 8);  be[3] = (uint8_t)v;
    printf("大端字节流：%02X %02X %02X %02X\n", be[0], be[1], be[2], be[3]);
    printf("  对端是大端设备（某些射频模块、部分网络协议）时必须这样打。\n");
    printf("  统一原则：字节序在协议里写死，代码里手工拼，绝不依赖 CPU。\n");

                /* ---------- 定宽类型的价值 ---------- */
    printf("\n=== 为什么必须用 uint16_t 而不是 unsigned short ===\n");
    printf("sizeof(unsigned short) = %zu ，sizeof(uint16_t) = %zu （本机相同）\n",
           sizeof(unsigned short), sizeof(uint16_t));
    printf("  但标准只保证 short 「至少 16 位」，某些平台上是 32 位。\n");
    printf("  协议字段一旦写错宽度，对端就解不出来 —— 所以只用定宽类型。\n");
    return 0;
}
