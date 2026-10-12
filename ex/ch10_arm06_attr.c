/**
 * ch10_arm06_attr.c —— GCC 的 __attribute__ 在 Cortex-M0+ 上的实际效果
 */
#include <stdint.h>

/**
 * 1) aligned：强制对齐。DMA 缓冲区、以及某些外设的 FIFO 必须对齐
 */
__attribute__((aligned(16))) uint8_t dma_buffer[8];

/**
 * 2) packed：紧凑布局。协议包、Flash 配置块必须一个填充字节都没有
 */
struct __attribute__((packed)) RadioFrame {
    uint8_t  head;               /* 偏移 0 */
    uint16_t chans[4];           /* 偏移 1（不是 2！） */
    uint32_t crc;                /* 偏移 9（不是 12！） */
    uint8_t  tail;               /* 偏移 13 */
};                       /* 总大小 14 */

/* 3) section：把变量/函数放进自定义段，链接脚本再把它摆到指定地址 */
__attribute__((section(".my_config"))) const uint32_t g_config_block[2] = { 0xA5A5A5A5u, 0x5A5A5A5Au };

/* 4) used：即使没人引用也不许被链接器丢掉（查表数据、ISR 表常用） */
__attribute__((used)) static const char g_version[] = "fw-1.0";



__attribute__((weak)) void HAL_TIM_PeriodElapsedCallback(void *htim)
{
    (void)htim;                  /* 默认什么都不做 */
}

/* 6) always_inline / noinline：控制内联。中断里的小工具函数常用 always_inline */
__attribute__((always_inline)) static inline uint32_t read_reg(volatile uint32_t *p)
{
    return *p;
}

/* 7) Cortex-M 上 ISR 就是普通 C 函数（硬件自动保存现场）， 所以【不需要】也没有 __attribute__((interrupt))；名字必须和启动文件 向量表里的名字一致，否则永远不会被调用。这里演示一个典型的 ISR。 */
static volatile uint32_t g_tick;

void TIM17_IRQHandler(void)
{
    g_tick++;
}

uint32_t use_all(volatile uint32_t *p)
{
    return read_reg(p) + (uint32_t)sizeof(struct RadioFrame)
         + g_config_block[0] + (uint32_t)g_version[0] + g_tick;
}
