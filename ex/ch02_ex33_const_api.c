/**
 * ch02_ex33_const_api.c —— const 在接口设计里的作用：谁负责改，谁只能读
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/**
 * 角色一：纯输入 —— const 指针，函数不写它
 */
static int sum_all(const int *data, size_t n)
{
    int total = 0;
    for (size_t i = 0; i < n; i++) {
        total += data[i];
    }
    return total;
}

/* 角色二：纯输出 —— 非 const 指针，函数只写不读 */
static void fill_sequence(int *out, size_t n, int start)
{
    for (size_t i = 0; i < n; i++) {
        out[i] = start + (int)i;
    }
}

/* 角色三：输入输出 —— 非 const 指针，函数读写都要 */
static void scale_in_place(int *inout, size_t n, int factor)
{
    for (size_t i = 0; i < n; i++) {
        inout[i] *= factor;
    }
}

/* 只读查表：static const 会进只读段（在单片机上就是 Flash） */
static const uint16_t g_sine_table[8] = { 0, 707, 1000, 707, 0, 65535, 64535, 63535 };

static uint16_t lookup(size_t idx)
{
    if (idx >= sizeof(g_sine_table) / sizeof(g_sine_table[0])) {
        return 0;                                   /* 边界检查：const 表也一样要查 */
    }
    return g_sine_table[idx];
}

int main(void)
{
    printf("---- 1. 三种参数角色 ----\n");
    int buf[5] = { 1, 2, 3, 4, 5 };
    printf("  输入（只读）: sum_all(buf, 5) = %d\n", sum_all(buf, 5));

    fill_sequence(buf, 5, 100);
    printf("  输出（只写）: fill_sequence(buf,5,100) ->");
    for (int i = 0; i < 5; i++) { printf(" %d", buf[i]); }
    printf("\n");

    scale_in_place(buf, 5, 2);
    printf("  输入输出    : scale_in_place(buf,5,2) ->");
    for (int i = 0; i < 5; i++) { printf(" %d", buf[i]); }
    printf("\n");

    printf("---- 2. 为什么 const 是契约 ----\n");
    printf("  a) 调用者看到 const，就知道「我可以放心把这个缓冲区交给它」；\n");
    printf("  b) 维护者想在里面改数据，编译器立刻报错；\n");
    printf("  c) 编译器可以据此做更好的优化（比如把读取提前）。\n");
    printf("  【反面教材】接口参数不加 const 时，调用者只能靠读实现来判断安全性 ——\n");
    printf("             这是 C 项目里大量 bug 的来源。\n");

    printf("---- 3. 只读查表放哪一段 ----\n");
    printf("  g_sine_table 地址 = %p（本机在只读段；单片机上通常映射到 Flash）\n",
           (const void *)g_sine_table);
    printf("  lookup(2) = %u, lookup(5) = %u, lookup(99) = %u（越界返回 0）\n",
           (unsigned)lookup(2), (unsigned)lookup(5), (unsigned)lookup(99));
    printf("  【为什么重要】PY32F030 这类芯片 RAM 只有几 KB，\n");
    printf("  一张 256 项的 uint16_t 表放 RAM 要 512 字节，放 Flash 则完全不占 RAM。\n");

    printf("---- 4. 三种角色对照表 ----\n");
    printf("  角色      写法             函数能做什么    调用者承诺什么\n");
    printf("  纯输入    const T *p       只读            不改自己的数据\n");
    printf("  纯输出    T *p             只写            不关心进来时是什么\n");
    printf("  输入输出  T *p             读写            接受数据被修改\n");
    printf("  把角色写进签名，比写在注释里可靠得多。\n");
    return 0;
}