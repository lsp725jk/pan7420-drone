/**
 * ch02_ex16_signed_cmp.c —— 有符号/无符号比较的三种安全写法
 */
#include <stdio.h>
#include <stddef.h>
#include <string.h>

/**
 * 安全的"小于"比较：把符号性差异摊开来说清楚
 */
static int lt_safe(long a, unsigned long b)
{
    if (a < 0) {
        return 1;                               /**
         * 负数一定小于任何无符号数
         */
    }
    return (unsigned long)a < b;
}

int main(void)
{
                /* ---- 1. 直接比较会出错 ---- */
    printf("---- 1. 直接比较的问题 ----\n");
    int          a = -1;
    unsigned int b = 1u;
#pragma warning(push)
#pragma warning(disable : 4018)
    printf("int(-1) < unsigned(1) 直接比 = %d   <-- 期望 1，实际 0\n", a < b);
#pragma warning(pop)
    printf("用 lt_safe(-1, 1) 比        = %d\n", lt_safe(-1, 1));
    printf("用 lt_safe(5, 10) 比        = %d\n", lt_safe(5, 10));
    printf("用 lt_safe(-100, 5) 比      = %d\n", lt_safe(-100, 5));

                /* ---- 2. 长度判断：无符号减法下溢 ---- */
    printf("---- 2. 长度判断 ----\n");
    const char *s = "ab";
    size_t len = strlen(s);
    printf("strlen(\"ab\") = %zu\n", len);
    printf("错误的写法 if (len - 3 > 0)：\n");
    printf("  len - 3 = %zu   <-- 下溢成一个巨大的数\n", len - 3);
    printf("  所以 (len - 3 > 0) = %d，本该是假却成了真\n", (len - 3 > 0) ? 1 : 0);
    printf("正确的写法 if (len > 3) = %d\n", (len > 3) ? 1 : 0);
    printf("记住：**永远不要用无符号减法去判断\"够不够\"**。\n");

                /* ---- 3. 用强转把两边统一到同一类型 ---- */
    printf("---- 3. 统一类型的三种做法 ----\n");
    int    x = 7;
    size_t y = 10;
    printf("做法 A：都转成有符号  (long)x < (long)y      = %d\n",
           (long)x < (long)y);
    printf("做法 B：都转成无符号  (size_t)x < y          = %d\n",
           (size_t)x < y);
    printf("做法 C：先判符号（最安全，适用于可能为负的输入） = %d\n",
           lt_safe((long)x, (unsigned long)y));
    printf("选择原则：\n");
    printf("  · 确定不会有负数（长度、下标、计数）→ 全部用无符号，做法 B；\n");
    printf("  · 可能有负数（传感器差值、误差）→ 全部用有符号 + 做法 C。\n");

                /* ---- 4. 混合运算里最隐蔽的一种 ---- */
    printf("---- 4. 混在算术式里的陷阱 ----\n");
    unsigned int n = 0;
    int          k = -1;
    printf("unsigned(0) + int(-1) = %u   <-- 不是 -1，是 4294967295\n", n + k);
    printf("(unsigned(0) + int(-1)) > 0 = %d   <-- 本该是假\n",
           (n + k) > 0u ? 1 : 0);
    printf("【症状】判断\"减法结果是否为正\"失败，进而走进错误分支。\n");
    printf("       这类 bug 在解析协议长度字段时最容易出现。\n");
    return 0;
}
