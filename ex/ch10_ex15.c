/**
 * ch10_ex15.c —— 未定义行为（UB, undefined behavior）的实际后果
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>

static int g_zero = 0;

int main(void)
{
    printf("=== 1) 有符号整数溢出（UB）===\n");
    int a = INT_MAX;
    int b = a + 1;                         /* UB：有符号溢出 */
    printf("  INT_MAX + 1 = %d   （本机得到 INT_MIN；标准说这是 UB）\n", b);
    unsigned int ua = UINT_MAX;
    printf("  UINT_MAX + 1 = %u  （无符号溢出是【定义良好】的，回绕到 0）\n", ua + 1u);
    printf("  -> 需要回绕语义时，就用无符号类型；有符号溢出必须避免\n\n");

    printf("=== 2) 移位超出位宽（UB）===\n");
    unsigned int x = 1u;
    printf("  1u << 31 = 0x%08X  （合法）\n", x << 31);
    printf("  1u << 32 —— UB，本机不演示具体值（不同 CPU 上 x86 是取模，ARM 是 0）\n");
    int neg = -1;
    printf("  -1 >> 1  = %d  （负数右移是【实现定义】的：算术移位还是逻辑移位由编译器定）\n\n",
           neg >> 1);

    printf("=== 3) 未初始化变量（UB）===\n");

    printf("  未初始化的局部变量：值是垃圾。/O2 下编译器甚至会假定「你不会读它」\n");
    printf("  -> 嵌入式里 .bss 段的全局变量由启动代码清零，但【局部变量不会】\n\n");

    printf("=== 4) 同一个变量上多次读写且没有序列点（UB）===\n");
    int i = 5;
    int r = i++ + i++;                     /* UB：i 在两个序列点之间被改了两次 */
    printf("  i++ + i++ 的结果 = %d ，i = %d （本机值，不可依赖）\n", r, i);
    i = 5;
    i = i++ + 1;                           /* UB */
    printf("  i = i++ + 1 之后 i = %d （本机值，不可依赖）\n\n", i);

    printf("=== 5) 数组越界写（UB）——本机只读不写，避免破坏栈 ===\n");
    int arr[4] = { 1, 2, 3, 4 };
    printf("  arr[4] 越界读到的值 = %d （垃圾，而且可能直接触发访问违例）\n", arr[4]);
    printf("  嵌入式里的越界写更危险：它会踩坏相邻变量，症状是「另一个变量自己变了」\n\n");

    printf("=== 6) 空指针解引用（UB）—— 只说明，不演示 ===\n");
    printf("  int *p = NULL; *p = 1;  在单片机上通常就是 HardFault\n\n");

    printf("=== 7) 最坑的一种：编译器用 UB 反推「这不可能发生」 ===\n");
    printf("  下面这段逻辑在 /O2 下会被整个删掉：\n");
    printf("      int v = INT_MAX;\n");
    printf("      if (v + 1 < v) { 处理溢出 }   // 编译器认为 v+1 不可能小于 v\n");
    printf("  因为溢出是 UB，编译器有权假定它不发生，于是这个 if 永远为假。\n");
    printf("  正确写法：先判断 if (v > INT_MAX - 1)，【在溢出发生之前】检查。\n\n");

    printf("=== 实用规则 ===\n");
    printf("  1) 有符号溢出、越界、未初始化、空指针、多次修改同一变量 —— 全部要避开；\n");
    printf("  2) 打开并【看完】编译器的警告（/W4 / -Wall -Wextra），它们专抓这类问题；\n");
    printf("  3) Debug 版好、Release 版坏，第一件事就是怀疑 UB；\n");
    printf("  4) 嵌入式里 UB 的代价往往是 HardFault + 看门狗复位，\n");
    printf("     表现为「飞机每隔几秒重启一次」—— 极难定位。\n");
    return 0;
}
