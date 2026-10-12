/**
 * ch03_ex7_incdec.c —— 自增自减：i++ 与 ++i 的区别和坑
 */
#include <stdio.h>

int main(void)
{
                /**
         * ---- 1. 前缀/后缀在"取值"时的区别 ----
         */
    printf("---- 1. 取值区别 ----\n");
    int i = 5;
    int a = i++;                        /* a 拿旧值 5，i 变 6 */
    printf("int a = i++;  -> a = %d, i = %d\n", a, i);
    int b = ++i;                        /* i 先变 7，b 拿 7 */
    printf("int b = ++i;  -> b = %d, i = %d\n", b, i);
    int c = i--;                        /* c 拿 7，i 变 6 */
    printf("int c = i--;  -> c = %d, i = %d\n", c, i);
    int d = --i;                        /* i 先变 5，d 拿 5 */
    printf("int d = --i;  -> d = %d, i = %d\n", d, i);

                /* ---- 2. 单独一条语句时没有区别（但风格要统一）---- */
    printf("---- 2. 单独用时一样 ----\n");
    int x = 0;
    x++;
    printf("x++ 之后 x = %d\n", x);
    ++x;
    printf("++x 之后 x = %d\n", x);
    printf("for 循环里 for(i=0;i<n;i++) 和 ++i 等价，选一种坚持用。\n");

                /* ---- 3. 常见用法：遍历、计数、指针移动 ---- */
    printf("---- 3. 常见用法 ----\n");
    int arr[5] = { 10, 20, 30, 40, 50 };
    int n = 0;
                /* 用后缀走过数组：*(p++) 是先取值再移动指针 */
    int *p = arr;
    while (n < 5) {
        printf("%d ", *p++);
        n++;
    }
    printf("  <- *p++ 更常写成 for 循环\n");

                /* 更清楚的写法：用下标，别玩指针技巧 */
    for (int k = 0; k < 5; k++) {
        printf("%d ", arr[k]);
    }
    printf("  <- 下标写法最不容易看错\n");

                /* 后缀的经典紧凑写法：字符串复制（注意结尾的 '\0' 也要拷） */
    const char *src = "hi";
    char dst[8];
    int m = 0;
    while ((dst[m] = src[m]) != '\0') {           /* 也可以用 (dst[m++] = src[m]) 这种写法， */
        m++;                                              /* 但初学阶段分开写更安全 */
    }
    printf("复制结果: %s\n", dst);

                /* ---- 4. 未定义行为：同一变量在一个表达式里改两次 ---- */
    printf("---- 4. 未定义行为（UB）----\n");
    int u = 0;
    int r1 = u++ + u++;
    printf("int u=0; r1 = u++ + u++;  -> r1 = %d, u = %d\n", r1, u);
    printf("  实测 MSVC 19.51 在 /Od 和 /O2 下都得到 r1 = 0, u = 2，\n");
    printf("  但标准说这是 UB，换个编译器可能得到 1 或 2。别写这种代码。\n");

    int v = 0;
    int r2 = v++ + ++v;                     /* 更糟：一个后缀一个前缀 */
    printf("int v=0; r2 = v++ + ++v;  -> r2 = %d, v = %d (同样是 UB)\n", r2, v);

                /* 下面的写法是明确合法的：分开两条语句，顺序清楚 */
    int w = 0;
    w++;
    w++;
    printf("分开写：w++ 两次 -> w = %d   <-- 这才是该写的代码\n", w);

                /* ---- 5. 和 printf 实参顺序有关的坑 ---- */
    printf("---- 5. 不要在一个 printf 里多次自增同一个变量 ----\n");
    int t = 1;
        /* printf("%d %d\n", t++, t++);   <-- 实参求值顺序未指定，结果随编译器变。 MSVC 是从右往左求值，所以实际会打印 "2 1"。这里故意不写， 因为这种"能跑但依赖实现"的代码不该出现在任何项目里。 */
    printf("拆开写：");
    printf("%d ", t++);
    printf("%d\n", t++);
    printf("  这样每个自增的顺序都是确定的。\n");

                /* ---- 6. 自增自减在嵌入式里的实际风险 ---- */
    printf("---- 6. 嵌入式提醒 ----\n");
    printf("a) shared_flag++ 在中断和主循环里同时执行会丢计数：\n");
    printf("   它实际是'读-改-写'三条指令，中断可能插在中间。\n");
    printf("   必须 volatile + 关中断，或用原子指令。\n");
    printf("b) 对 volatile 变量做 i++ 不等于'原子的加一'，两者是两件事。\n");
    printf("c) 指针自增遍历寄存器块很常见，但一定确认没越界：\n");
    printf("   越界读在单片机上可能读到别的外设寄存器，行为诡异且难查。\n");
    return 0;
}
