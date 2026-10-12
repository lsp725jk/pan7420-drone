/**
 * ch02_ex6_const.c —— const 的五种用法
 */
#include <stdio.h>

/**
 * 只读传参：明确告诉调用者"这个数组我不会动"
 */
static int sum_array(const int *data, int count)
{
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += data[i];
    }
                /* data[0] = 999;    <-- 取消注释会编译失败：assignment of read-only location */
    return total;
}

/* 返回指针时也可以用 const 表达"我只是借给你看，别改" */
static const char *mode_name(int mode)
{
    static const char *names[] = { "IDLE", "ARMED", "FAILSAFE" };
    if (mode < 0 || mode > 2) {
        return "UNKNOWN";
    }
    return names[mode];
}

int main(void)
{
                /* ---- 1. 修饰变量：一旦初始化就不能再改 ---- */
    const int max_retry = 3;
    printf("---- 1. const 变量 ----\n");
    printf("max_retry = %d\n", max_retry);
                /* max_retry = 4;   <-- 编译错误：error C2166: 左值指定 const 对象 */

                /* ---- 2. 指向 const 的指针 vs const 指针 ---- */
    int a = 10;
    int b = 20;
    const int *p_to_const = &a;                 /* 值不能改，指针能改 */
    int * const const_p   = &a;                 /* 指针不能改，值能改 */
    const int * const both = &a;                /* 都不能改 */

    printf("---- 2. 指针与 const ----\n");
    p_to_const = &b;                            /* OK：换了指向 */
    printf("p_to_const 现在指向 b = %d\n", *p_to_const);
                /* p_to_const = 99;                <-- 错误：不能改值 */

    *const_p = 11;                              /* OK：改了 a 的值 */
    printf("a 现在是 %d\n", a);
                /* const_p = &b;                    <-- 错误：不能换指向 */

    printf("both 指向 a = %d\n", *both);
                /* both = 1;  both = &b;           <-- 两个都错 */

                /* ---- 3. 传参：const 是"接口约定" ---- */
    int data[5] = { 1, 2, 3, 4, 5 };
    printf("---- 3. 只读传参 ----\n");
    printf("sum_array(data,5) = %d\n", sum_array(data, 5));

                /* 【坑】把 const 指针传给"非 const 参数"的函数， 等于放弃了 const 保护，编译器会警告（C4090/Discarded const qualifier）。 */

                /* ---- 4. const 与字符串字面量：字面量本来就是只读的 ---- */
    printf("---- 4. 字符串字面量 ----\n");
    const char *ro = "hello";                   /* 正确：字面量不能被改写 */
    printf("ro = %s\n", ro);
                /* ro[0] = 'H';  <-- 运行期会崩（字面量在只读段），千万别这么写。 想要可改的字符串要自己准备数组：char buf[] = "hello"; */

    char buf[] = "hello";                       /* 这是数组，内容可改 */
    buf[0] = 'H';
    printf("buf = %s   (数组版本的副本可以改)\n", buf);

                /* ---- 5. const 在嵌入式里的实际用法 ---- */
    printf("---- 5. 嵌入式里的用法 ----\n");
                /* 查表常数一律加 const：编译器可以把它放进只读数据段（Flash）， 不占宝贵的 RAM —— 这对 RAM 只有几 KB 的 PY32F030 很关键。 */
    static const int mode_table[3] = { 0, 1, 2 };
    printf("mode_name(1) = %s   mode_table[1] = %d\n",
           mode_name(1), mode_table[1]);
                /* 函数参数写成 const uint8_t *buf，也是在告诉调用者： "我读完你的缓冲区就还给你，不会偷偷改"。 */
    printf("---- 提示：C23 起可以用 constexpr 替代更多 const 场景 ----\n");
    return 0;
}
