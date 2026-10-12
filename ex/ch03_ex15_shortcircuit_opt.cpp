/**
 * ch03_ex15_shortcircuit_opt.c —— 短路求值与"该不该依赖它"
 */
#include <stdio.h>

static int g_init_calls = 0;
static int g_read_calls = 0;

/**
 * 带副作用的"初始化"：调用一次就够，但每次调用都会累加计数
 */
static int init_device(void)
{
    g_init_calls++;
    return 1;                               /**
     * 返回"初始化成功"
     */
}

static int already_ready(void)
{
    g_read_calls++;
    return 0;                               /* 返回"还没就绪" */
}

/* 纯判断：没有副作用，适合放在短路表达式里 */
static int is_valid(int x)
{
    return (x >= 0 && x <= 100);
}

int main(void)
{
    printf("---- 1. 短路求值是语言保证 ----\n");
                /* 左边的结果为假，右边**一定不执行** —— 这是标准规定的，不是优化 */
    g_init_calls = 0;
    g_read_calls = 0;
    int r = init_device() || already_ready();
    printf("init_device() || already_ready() = %d\n", r);
    printf("  init_device 被调用 %d 次（返回真，所以右边不执行）\n", g_init_calls);
    printf("  already_ready 被调用 %d 次   <-- 0 次，这就是短路\n", g_read_calls);

    printf("---- 2. 反过来写就会多调一次 ----\n");
    g_init_calls = 0;
    g_read_calls = 0;
    r = already_ready() || init_device();
    printf("already_ready() || init_device() = %d\n", r);
    printf("  already_ready 被调用 %d 次\n", g_read_calls);
    printf("  init_device 被调用 %d 次（左边为假，所以右边必须执行）\n",
           g_init_calls);

    printf("---- 3. 用短路做边界保护（这是它最有价值的用法）----\n");
    int arr[3] = { 10, 20, 30 };
    int idx = 5;
                /* 先判下标合法，再访问 —— 顺序绝对不能反 */
    if (idx >= 0 && idx < 3 && arr[idx] > 0) {
        printf("  arr[%d] = %d\n", idx, arr[idx]);
    } else {
        printf("  idx=%d 越界，短路保护生效，没有访问 arr[idx]\n", idx);
    }
    printf("  如果写成 arr[idx] > 0 && idx < 3，就会先越界读 —— 顺序即正确性。\n");

    printf("---- 4. 依赖短路执行副作用：设计缺陷 ----\n");
    g_init_calls = 0;
    g_read_calls = 0;
                /* 这种写法很常见，但它把"必须发生的初始化"绑在了短路逻辑上： */
    if (init_device() || already_ready()) {
        printf("  设备可用（init_device 调了 %d 次，already_ready 调了 %d 次）\n",
               g_init_calls, g_read_calls);
    }
    printf("  问题：如果哪天 init_device 改成「已经初始化就返回真」，\n");
    printf("        already_ready 就永远不被调用了 —— 而它可能才是真正必要的检查。\n");
    printf("  正确做法：把有副作用的调用**单独写一行**，别塞进条件表达式。\n");
    g_init_calls = 0;
    g_read_calls = 0;
    int inited = init_device();
    int ready = already_ready();
    if (inited || ready) {
        printf("  拆开写之后：init_device %d 次，already_ready %d 次 —— 都可控。\n",
               g_init_calls, g_read_calls);
    }

    printf("---- 5. 纯判断可以放心用短路 ----\n");
    int tests[4] = { -1, 0, 50, 200 };
    for (int i = 0; i < 4; i++) {
        printf("  is_valid(%4d) = %d\n", tests[i], is_valid(tests[i]));
    }
    printf("  is_valid 里 x >= 0 && x <= 100 是纯判断，短路只带来好处。\n");
    return 0;
}
