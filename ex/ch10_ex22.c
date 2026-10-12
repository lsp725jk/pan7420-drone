/**
 * ch10_ex22.c —— 【对照实验】同一段 UB 在 /Od 与 /O2 下的不同结果
 */
#include <stdio.h>
#include <limits.h>

/**
 * ---------- 实验 1：编译器用 UB 反推，把检查删掉 ----------
 */
static int overflow_check_bad(int v)
{
                /**
         * 想检查「v+1 会不会溢出」。但 v+1 本身就是 UB，
         */
    if (v + 1 < v) return 1;                /* 检测到溢出 */
    return 0;
}
static int overflow_check_good(int v)
{
                /* 在溢出发生【之前】检查：v+1 永远不溢出 */
    if (v > INT_MAX - 1) return 1;
    return 0;
}

/* ---------- 实验 2：把「未初始化」变量读出来 ---------- */
static int read_uninit_bad(void)
{
    int x;                                  /* 未初始化 */
    return x;                               /* UB：读未初始化变量 */
}

/* ---------- 实验 3：越界读 ---------- */
static int read_oob(void)
{
    int a[4] = { 10, 20, 30, 40 };
    return a[4];                            /* UB：越界 */
}

/* ---------- 实验 4：有符号溢出 ---------- */
static int signed_overflow(void)
{
    int v = INT_MAX;
    return v + 1;                           /* UB：有符号溢出 */
}

int main(void)
{
    printf("=== 1) 溢出检查：UB 版本 vs 正确版本 ===\n");
    printf("  overflow_check_bad(INT_MAX)  = %d  <- 想返回 1（检测到溢出）\n",
           overflow_check_bad(INT_MAX));
    printf("  overflow_check_bad(0)        = %d\n", overflow_check_bad(0));
    printf("  overflow_check_good(INT_MAX) = %d  <- 正确版本，一定返回 1\n",
           overflow_check_good(INT_MAX));
    printf("  提示：如果 /O2 下 bad(INT_MAX) 打出 0，说明那个 if 被整个删掉了。\n\n");

    printf("=== 2) 未初始化变量 ===\n");
    printf("  read_uninit_bad() = %d  （本机值，换优化等级会变）\n\n",
           read_uninit_bad());

    printf("=== 3) 越界读 ===\n");
    printf("  read_oob() = %d  （本机值，垃圾数据）\n\n", read_oob());

    printf("=== 4) 有符号溢出 ===\n");
    printf("  INT_MAX + 1 = %d\n", signed_overflow());
    printf("  无符号版本：UINT_MAX + 1 = %u （这个是【定义良好】的）\n\n",
           (unsigned)UINT_MAX + 1u);

    printf("=== 怎么用这个实验自查代码 ===\n");
    printf("  1) 用最终优化等级（/O2 或 -O2）编译并运行；\n");
    printf("  2) 再和 /Od 的结果对一遍 —— 有任何差异都说明有 UB；\n");
    printf("  3) 本机【没有】UBSan/TSan：MSVC 会静默忽略 /fsanitize=undefined\n");
    printf("     和 /fsanitize=thread（本项目实测确认）。\n");
    printf("     ASan 是唯一可用的运行期消毒器（/fsanitize=address），\n");
    printf("     它能抓越界和释放后使用，但抓不到有符号溢出。\n");
    printf("  4) 所以对 UB 最靠谱的手段还是：开满警告（/W4）、\n");
    printf("     把警告当错误（/WX）、代码评审时专门找这几类写法。\n");
    return 0;
}
