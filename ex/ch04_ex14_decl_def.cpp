/**
 * ch04_ex14_decl_def.c —— 声明、默认参数提升、函数原型的三个坑
 */
#include <stdio.h>

/**
 * ---- 1. 参数提升：声明决定一切 ----
 */
/**
 * 声明成 char 参数：调用时实参会先被转成 char（可能截断）
 */
static int takes_char(char c)
{
    return (int)c;
}

/**
 * 声明成 int 参数：不会截断
 */
static int takes_int(int v)
{
    return v;
}

/* ---- 2. 返回值类型也会被声明决定 ---- */
static double half_declared_double(int x);
static int    half_declared_int(int x);

static double half_declared_double(int x) { return x / 2.0; }
static int    half_declared_int(int x)    { return x / 2; }     /* 整数除法 */

/* ---- 3. 参数表 () 与 (void) 的区别 ---- */
/* 下面这种"老式"声明（K&R 风格）在 C 里仍然合法，但没有任何参数检查： int old_style(); 现代代码一律写 (void)。 */

int main(void)
{
    printf("---- 1. 参数提升：声明成 char 会截断 ----\n");
    int big = 300;
                /* 下面这一行**故意**触发 warning C4244（int→char 可能丢数据）， 这正是本例要演示的坑，所以压掉警告让输出干净。 */
#pragma warning(push)
#pragma warning(disable : 4244)
    printf("takes_char(300) = %d   <-- 300 被截成 char（44）\n", takes_char(big));
#pragma warning(pop)
    printf("takes_int (300) = %d   <-- 完整保留\n", takes_int(big));
    printf("  【症状】如果声明和定义的类型不一致（比如声明成 short、定义是 int），\n");
    printf("  编译器可能不报错，但调用约定对不上，参数会读错。\n");
    printf("  这就是为什么**声明必须和定义完全一致**，最好共用头文件。\n");

    printf("---- 2. 返回值类型同样由声明决定 ----\n");
    printf("half_declared_double(5) = %f\n", half_declared_double(5));
    printf("half_declared_int(5)    = %d\n", half_declared_int(5));
    printf("  同一个 x/2.0 和 x/2，返回类型不同结果就不同。\n");
    printf("  【症状】把 double 返回值赋给 int 变量，会静默截断（/W4 报 C4244）。\n");

    printf("---- 3. 参数表 () 与 (void) ----\n");
    printf("  int f(void);  明确表示没有参数，调用时传参数会**编译错误**；\n");
    printf("  int f();      C 里表示参数表未指定，传什么都不检查（很危险）；\n");
    printf("  【建议】一律写 (void)，把参数检查交给编译器。\n");

    printf("---- 4. 未声明的函数 ----\n");
    printf("  C99 起，调用一个没见过声明的函数是**错误**。\n");
    printf("  实测报错：warning C4013: \"foo\"未定义；假设外部返回 int\n");
    printf("           随后往往跟着 error LNK2019（因为真的没有定义）。\n");
    printf("  【症状速查】看到 C4013，第一反应是少了一个 #include。\n");

    printf("---- 5. 一条实用规则 ----\n");
    printf("  函数的声明和定义**只写一遍类型**：把声明放头文件，\n");
    printf("  所有 .c 都 include 它，这样两边不可能写不一致。\n");
    printf("  （在 .c 里再手抄一遍声明，是类型不匹配的主要来源。）\n");
    return 0;
}
