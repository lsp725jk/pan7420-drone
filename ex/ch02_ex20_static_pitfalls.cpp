/**
 * ch02_ex20_static_pitfalls.c —— static 的三个真实坑
 */
#include <stdio.h>
#include <string.h>

/**
 * ---- 坑一：静态对象只能用常量初始化 ----
 */
static int make_id(int start)
{
        /* 下面这行如果取消注释会报： error C2099: 初始化符号不是常量 因为静态存储期的对象在**程序启动前**就要定好初值， 那时函数还没运行，拿不到 start 的值。 */
                /* static int id = start; */
    static int id = 100;                    /* 常量初始化：合法 */
    id += start;                            /* 想用运行时值，就放在函数体里做 */
    return id;
}

/* ---- 坑二：大缓冲区的位置 ---- */
#define BIG_N 50000

static void big_local_report(void)
{
    int local_buf[BIG_N];                   /* 在**栈**上要 200000 字节！ */
    static int static_buf[BIG_N];           /* 在**数据段**，不占栈 */
                /* 只写首尾，避免真的去碰 20 万个元素 */
    local_buf[0] = 1;  local_buf[BIG_N - 1] = 2;
    static_buf[0] = 1; static_buf[BIG_N - 1] = 2;
    printf("  local_buf  首元素地址  = %p  （栈区，单片机上会爆栈）\n",
           (void *)&local_buf[0]);
    printf("  static_buf 首元素地址  = %p  （数据段，安全）\n",
           (void *)&static_buf[0]);
    printf("  两者都占 %d 字节，区别只在**放在哪一段内存**。\n",
           (int)(BIG_N * sizeof(int)));
    printf("  【本机实测】桌面进程栈有 1MB，所以 local_buf 能活；\n");
    printf("  换成 PY32F030（栈通常 1~4KB）就会直接爆栈。\n");
}

/* ---- 坑三：static 局部变量 = 不可重入 ---- */
static int running_sum(int x)
{
    static int total = 0;                   /* 记住上次的结果 */
    total += x;
    return total;
}

/* 同一个功能，改成"状态由调用者持有"就是可重入的 */
static int running_sum_reentrant(int x, int *state)
{
    *state += x;
    return *state;
}

int main(void)
{
    printf("---- 1. 静态初始化必须用常量 ----\n");
    printf("make_id(1)  = %d\n", make_id(1));
    printf("make_id(2)  = %d   <-- 静态变量记住了上次的值\n", make_id(2));
    printf("make_id(3)  = %d\n", make_id(3));
    printf("如果想让初值随参数变，就得在函数体里赋值，不能写在声明处。\n");

    printf("---- 2. 大缓冲区放哪一段 ----\n");
    big_local_report();

    printf("---- 3. static 局部变量与可重入 ----\n");
    printf("running_sum(10) = %d\n", running_sum(10));
    printf("running_sum(20) = %d   <-- 30，因为它记得上次的 10\n", running_sum(20));
    printf("running_sum(5)  = %d\n", running_sum(5));

    int state = 0;
    printf("可重入版本：\n");
    printf("  running_sum_reentrant(10, &state) = %d\n",
           running_sum_reentrant(10, &state));
    printf("  running_sum_reentrant(20, &state) = %d\n",
           running_sum_reentrant(20, &state));
    printf("  把状态交给调用者，函数本身就没有隐藏状态了。\n");

    printf("---- 4. 为什么嵌入式特别在意不可重入 ----\n");
    printf("  假设 running_sum 被主循环和串口中断同时调用：\n");
    printf("  中断插在 total += x 的读-改-写中间，那次加法就丢了。\n");
    printf("  症状：统计值偶尔偏小，且只在中断频繁时出现 —— 极难复现。\n");
    printf("  对策：① 改成传状态指针（可重入）；\n");
    printf("        ② 加 volatile + 关中断（只解决共享，不解决重入）；\n");
    printf("        ③ 干脆禁止在 ISR 里调用这类函数。\n");
    return 0;
}
