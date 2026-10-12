/**
 * ch01_ex1_structure.c —— 一个 C 程序的最小完整骨架
 */

/**
 * ① 预处理指令（preprocessing directive）：
 */
#include <stdio.h>

/* ② 函数声明（function declaration，也叫原型 prototype）： 它只告诉编译器"有这么个函数、参数是 int,int、返回 int"，没有函数体。 在 C 里，函数必须先声明后使用，否则编译器只能靠猜（C99 起直接报错/警告）。 */
int add(int a, int b);

/* ③ main 函数：程序唯一的入口（entry point）。 int main(void) 里的 void 表示"不带参数"。 注意：C 里写 int main() 是"参数表未指定"，不是"没有参数"， 想表达没有参数要写 (void)。这是 C 和 C++ 的一个真实差异。 */
int main(void)
{
        /* ④ 语句（statement）以分号结尾；一对 {} 把多条语句组成"块"（block）。 局部变量的声明放在块的开头是最保险的老习惯（C89 要求如此， C99 起允许随用随声明）。 */
    int a = 3;                  /* 定义（definition）：分配了存储空间，还给了初值 */
    int b = 4;
    int sum;

                /* ⑤ 函数调用：add(a, b) 把 a、b 的**值**复制给形参（值传递）。 注意实参和形参是两个不同的对象，后面第 04 章会细讲。 */
    sum = add(a, b);

        /* ⑥ printf（print formatted，格式化输出）： "" 里是格式串（format string），%d 是转换说明（conversion specifier）， 含义是"把后面的 int 实参按十进制打印在这里"。 */
    printf("hello, C\n");
    printf("%d + %d = %d\n", a, b, sum);

                /* ⑦ return 把值交还给操作系统。 main 是特例：不写 return 也等价于 return 0（C99 起明确规定）。 */
    return 0;
}

/* ⑧ 函数定义（function definition）：声明 + 函数体。 定义只能有一份，声明可以有很多份——这一点是"声明放在 .h、 定义放在 .c"这种工程习惯的根源。 */
int add(int a, int b)
{
    return a + b;
}
