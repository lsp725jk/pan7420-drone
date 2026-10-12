/**
 * ch10_ex07.c —— 启动文件与链接脚本在干什么：.data / .bss 的搬运 + 向量表
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/**
 * ---------- 模拟 Flash（只读区） ----------
 */
/**
 * 这里放 .data 的"初值镜像"：链接脚本里叫 _sidata
 */
static const uint32_t FLASH_DATA_INIT[3] = { 0x11223344u, 0x55667788u, 0xAABBCCDDu };
/* 这里放向量表的前两项（真机上是栈顶地址 + Reset_Handler 地址） */
static const uint32_t FLASH_VECTOR[4] = { 0x20002000u, 0x08000101u, 0x08000111u, 0x08000121u };

/* ---------- 模拟 RAM ---------- */
static uint32_t RAM_DATA[3];        /* .data 段：3 个字，需要从 Flash 拷初值 */
static uint32_t RAM_BSS[3];         /* .bss  段：3 个字，需要清零 */

/* 链接脚本里会导出这些符号，启动代码靠它们知道"从哪拷到哪" */
static const uint32_t *const _sidata = FLASH_DATA_INIT;
static uint32_t       *const _sdata  = RAM_DATA;
static uint32_t       *const _edata  = RAM_DATA + 3;
static uint32_t       *const _sbss   = RAM_BSS;
static uint32_t       *const _ebss   = RAM_BSS + 3;

/* ---------- 复位处理：启动文件里 Reset_Handler 的 C 版语义 ---------- */
static void Reset_Handler_sim(void)
{
                /* 第 1 步：把 .data 的初值从 Flash 拷到 RAM（真机上是 ldr/str 循环） */
    const uint32_t *src = _sidata;
    for (uint32_t *dst = _sdata; dst < _edata; ) *dst++ = *src++;

                /* 第 2 步：把 .bss 清零（真机上是四字节一组写 0，比逐字节快） */
    for (uint32_t *dst = _sbss; dst < _ebss; ) *dst++ = 0u;

                /* 第 3 步：初始化 C 库要用的东西，然后调 main —— 这里用打印代替 */
    printf("  [Reset_Handler] .data 搬运完成，.bss 清零完成\n");
}

/* ---------- 向量表 + 查表跳转 ---------- */
typedef void (*isr_t)(void);

static void handler_tim(void) { printf("  [向量表] 跳到了 TIM 中断处理函数\n"); }
static void handler_uart(void){ printf("  [向量表] 跳到了 UART 中断处理函数\n"); }

/* 真机上这张表由启动文件写死，并且用 __attribute__((section(".isr_vector"))) 定位到 0 地址 */
static const isr_t VECTOR_TABLE[4] = {
    (isr_t)0,                  /* [0] 初始栈顶（真机上是 MSP 初值，不是函数） */
    (isr_t)0,                  /* [1] Reset_Handler */
    handler_tim,               /* [2] 假设是 TIM17 中断 */
    handler_uart,              /* [3] 假设是 USART1 中断 */
};

static void emulate_irq(int irqn)
{
    printf("硬件报告中断号 %d -> ", irqn);
    VECTOR_TABLE[irqn]();                  /* 真机上是硬件自动做的 */
}

int main(void)
{
                /* 先故意把 RAM 弄脏，模拟"上电后 RAM 里是随机值" */
    memset(RAM_DATA, 0xCD, sizeof(RAM_DATA));
    memset(RAM_BSS,  0xCD, sizeof(RAM_BSS));
    printf("上电瞬间（模拟随机值）：\n");
    printf("  RAM_DATA[0] = 0x%08X , RAM_BSS[0] = 0x%08X\n",
           RAM_DATA[0], RAM_BSS[0]);

    printf("\n执行 Reset_Handler：\n");
    Reset_Handler_sim();

    printf("\n搬运之后：\n");
    for (int k = 0; k < 3; k++)
        printf("  RAM_DATA[%d] = 0x%08X  （来自 Flash 初值表 0x%08X）\n",
               k, RAM_DATA[k], FLASH_DATA_INIT[k]);
    for (int k = 0; k < 3; k++)
        printf("  RAM_BSS [%d] = 0x%08X\n", k, RAM_BSS[k]);

    printf("\n向量表前 4 项（真机上第 0 项是栈顶地址）：\n");
    for (int k = 0; k < 4; k++)
        printf("  [%d] 0x%08X\n", k, FLASH_VECTOR[k]);

    printf("\n模拟两次中断：\n");
    emulate_irq(2);
    emulate_irq(3);

    printf("\n关键数字（对应 PY32F030 的 64KB Flash / 8KB RAM）：\n");
    printf("  .data 段在 Flash 里【也占一份】—— 初值要存在 Flash 里等着被搬运；\n");
    printf("  .bss  段不占 Flash，只占 RAM；\n");
    printf("  所以把大数组初始化成 {0} 比初始化成别的值更省 Flash。\n");
    printf("  全局 const 数组（如查表数据）应该加 const 让它留在 Flash，别被搬到 RAM。\n");
    return 0;
}
