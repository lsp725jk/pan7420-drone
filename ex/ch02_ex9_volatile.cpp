/**
 * ch02_ex9_volatile.c —— volatile：为什么单片机的共享变量必须加它
 */
#include <stdio.h>

/**
 * ---------- 场景：模拟"中断"会来改这个标志 ----------
 */

volatile int g_isr_flag = 0;        /**
 * 中断会改它 -> 必须 volatile
 */
int          g_plain_flag = 0;      /* 对照组：普通全局变量 */

/* 用来控制循环次数的 volatile 变量：外部可以通过调试器/硬件改它 */
volatile int g_step_limit = 5;

/* 模拟中断服务程序：把两个标志都置 1 */
static void fake_isr(void)
{
    g_isr_flag  = 1;
    g_plain_flag = 1;
}

int main(void)
{
                /* ---- 1. volatile 保证每次访问都真的读写内存 ---- */
    printf("---- 1. volatile 变量每次访问都落到内存 ----\n");
    g_isr_flag = 0;
    printf("先设 0，读一次: %d\n", g_isr_flag);
    fake_isr();
    printf("模拟中断置位后读: %d\n", g_isr_flag);

                /* ---- 2. 轮询等待：这是 volatile 最经典的用途 ---- */
    printf("---- 2. 轮询等待标志位 ----\n");
    g_isr_flag = 0;
    int spin = 0;
                /* 真实的嵌入式写法： while (!(REG->SR & FLAG)) { }        // 等硬件把标志置起来 这里用一次模拟中断代替硬件。 */
    fake_isr();
                /* 注意：g_isr_flag 是 volatile，所以编译器不能假设"它一直是 0" */
    while (g_isr_flag == 0) {
        spin++;
        if (spin > 1000000) {
            break;                                              /* 防止真的死循环 */
        }
    }
    printf("等到标志 = %d，空转了 %d 次\n", g_isr_flag, spin);

                /* ---- 3. volatile 指针：寄存器映射的写法 ---- */
    printf("---- 3. volatile 指针的两种常见写法 ----\n");
                /* 写法 A：指针本身可变，指向的对象是 volatile */
    volatile unsigned int fake_reg = 0;
    volatile unsigned int *reg_a = &fake_reg;
    *reg_a = 0x1234u;                           /* 这一次写不会被优化掉 */
    printf("写法 A：*reg_a = 0x%X\n", *reg_a);

                /* 写法 B：指针本身是 const（地址固定），指向的是 volatile —— 嵌入式标准写法 */
    volatile unsigned int *const reg_b = &fake_reg;
    *reg_b = 0x5678u;
    printf("写法 B：*reg_b = 0x%X   (地址固定，内容易变)\n", *reg_b);

                /* ---- 4. volatile 不保证原子性 ---- */
    printf("---- 4. volatile 不等于原子 ----\n");
    volatile int cnt = 0;
    cnt++;                                      /* 实际是：读 -> 加 1 -> 写回，三步 */
    printf("volatile int 自增后 = %d（这三步之间随时可能被打断）\n", cnt);
    printf("要原子：单核上关中断再操作，或用 C11 的 atomic_int。\n");

                /* ---- 5. 什么时候**不该**加 volatile ---- */
    printf("---- 5. 别滥用的三个理由 ----\n");
    printf("a) volatile 会阻止编译器把这些访问合并/提到循环外，代码变慢；\n");
    printf("b) 它不解决并发问题，加了它并不能让代码变成线程安全；\n");
    printf("c) volatile 修饰的结构体整体赋值、传递，语义容易搞错，\n");
    printf("   一般只修饰真正需要每次都读内存的那几个变量。\n");

                /* ---- 6. 编译期就能看到的对比 ---- */
    printf("---- 6. 对比一下普通变量 ----\n");
    int normal = 0;
    normal = normal + 1;                        /* 编译器完全可以在寄存器里完成 */
    printf("普通局部变量 = %d（优化后可能根本不在内存里出现）\n", normal);
    return 0;
}
