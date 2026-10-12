/**
 * ch02_ex7_static.c —— static 的三种含义（同一个关键字，三个不同作用）
 */
#include <stdio.h>

/**
 * ---------- 三种"计数器"的对比 ----------
 */

/**
 * ① 普通局部变量：每次调用都从 0 开始
 */
static int counter_auto(void)
{
    int n = 0;
    n++;
    return n;
}

/* ② 静态局部变量：只初始化一次，值跨调用保留 */
static int counter_static(void)
{
    static int n = 0;                   /* 这一行整个程序只执行一次 */
    n++;
    return n;
}

/* ③ 文件作用域静态变量：本文件内部的"全局"变量 */
static int file_scope_n = 0;

static int counter_file(void)
{
    file_scope_n++;
    return file_scope_n;
}

/* 静态函数：内部链接。别的 .c 文件就算也定义一个同名函数也不冲突。 */
static void report(const char *tag, int value)
{
    printf("%-18s -> %d\n", tag, value);
}

/* ---------- 静态局部变量的"陷阱示范"：初始化不会重复执行 ---------- */
static void init_happens_once(void)
{
    static int init_count = 0;
    init_count++;
    printf("  进入函数第 %d 次，static 局部变量的值 = %d\n",
           init_count, init_count);
}

int main(void)
{
    printf("---- 1. 普通局部 vs 静态局部 ----\n");
    for (int i = 0; i < 3; i++) {
        printf("第 %d 次: counter_auto() = %d, counter_static() = %d\n",
               i + 1, counter_auto(), counter_static());
    }
    printf("结论：静态局部变量记住了上次的值，普通局部变量每次都是新的。\n");

                /* 【坑】上面那一行的求值顺序！ C 没有规定函数实参的求值顺序，MSVC 是从右往左。 这里两个函数互不影响，所以没关系；但如果有副作用依赖顺序，就是 UB。 */

    printf("---- 2. 文件作用域静态变量 ----\n");
    for (int i = 0; i < 3; i++) {
        report("counter_file()", counter_file());
    }

    printf("---- 3. 初始化只做一次 ----\n");
    for (int i = 0; i < 3; i++) {
        init_happens_once();
    }

    printf("---- 4. static 局部变量不占栈 ----\n");
    printf("它的地址在数据段（每次调用都一样），不是栈地址：\n");
    static int in_data_segment = 42;
    int on_stack = 42;
    printf("static 变量地址 = %p\n", (void *)&in_data_segment);
    printf("普通局部地址   = %p  (两次调用之间会变)\n", (void *)&on_stack);

    printf("---- 5. 嵌入式视角：为什么 static 很常用 ----\n");
    printf("a) 中断服务程序(ISR)和主循环共享的变量，用 static 包在文件内部，\n");
    printf("   避免随便被别的文件改；\n");
    printf("b) 状态机里'上次的状态'用静态局部变量存，比全局变量干净；\n");
    printf("c) 大缓冲区用 static，避免放在栈上把几 KB 的栈撑爆 ——\n");
    printf("   这是单片机项目里最实际的理由。\n");
    return 0;
}
