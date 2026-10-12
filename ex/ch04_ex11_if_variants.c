/**
 * ch04_ex11_if_variants.c —— if 的四种组织方式（查表、提前返回、卫语句、goto 单一出口）
 */
#include <stdio.h>

/**
 * ---- 方式一：查表（最省事的 if 替代品）----
 */
static const char *const g_state_names[] = {
    "IDLE", "ARMED", "FLYING", "FAILSAFE"
};
#define STATE_COUNT ((int)(sizeof(g_state_names) / sizeof(g_state_names[0])))

static const char *state_name_lookup(int s)
{
    if (s < 0 || s >= STATE_COUNT) {
        return "OUT_OF_RANGE";                          /* 边界必须自己管，查表不会帮你 */
    }
    return g_state_names[s];
}

/* ---- 方式二：卫语句（guard clause）提前返回 ---- */
/* 反面写法：层层嵌套的 if */
static int clamp_nested(int v, int lo, int hi)
{
    if (v < lo) {
        return lo;
    } else {
        if (v > hi) {
            return hi;
        } else {
            if (lo > hi) {
                return lo;                                              /* 参数非法 */
            } else {
                return v;
            }
        }
    }
}

/* 正面写法：先把"不合格的情况"排掉，剩下就是正常流程 */
static int clamp_flat(int v, int lo, int hi)
{
    if (lo > hi) {
        return lo;                                      /* 参数非法：先挡掉 */
    }
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;                                   /* 到这里一定合法，不用再嵌一层 */
}

/* ---- 方式三：用"状态机 + 查表"代替一堆 if ---- */
typedef enum { ST_IDLE = 0, ST_ARMED, ST_FLYING, ST_FAILSAFE } state_t;

static state_t next_state_switch(int event, state_t cur)
{
    switch (cur) {
    case ST_IDLE:     return (event == 1) ? ST_ARMED : ST_IDLE;
    case ST_ARMED:    return (event == 2) ? ST_FLYING : ST_ARMED;
    case ST_FLYING:   return (event == 3) ? ST_FAILSAFE : ST_FLYING;
    case ST_FAILSAFE: return ST_FAILSAFE;
    default:          return ST_IDLE;
    }
}

/* ---- 方式四：goto 单一出口（多资源释放的标准写法）---- */
static int acquire_and_use(int fail_step, int *out)
{
    char *res_a = NULL;
    char *res_b = NULL;
    int rc = -1;

    if (fail_step == 1) {
        goto cleanup;
    }
    res_a = "resource-A";
    if (fail_step == 2) {
        goto cleanup;
    }
    res_b = "resource-B";

    *out = 42;
    rc = 0;                                     /* 只有走到这里才算成功 */

cleanup:                                /* 唯一的出口 */
    if (res_a) {         /* 真实代码里这里是 free(res_a) 或关闭外设 */ }
    if (res_b) {         /* 同理 */ }
    return rc;
}

int main(void)
{
    printf("---- 1. 查表代替 if ----\n");
    for (int s = -1; s <= 4; s++) {
        printf("  state %2d -> %s\n", s, state_name_lookup(s));
    }
    printf("  表的大小用 sizeof 算，加一个状态只要改数组，不用改逻辑。\n");

    printf("---- 2. 卫语句拍平嵌套 ----\n");
    printf("  clamp_nested(150, 0, 100) = %d\n", clamp_nested(150, 0, 100));
    printf("  clamp_flat  (150, 0, 100) = %d\n", clamp_flat(150, 0, 100));
    printf("  clamp_nested(-5, 0, 100)  = %d\n", clamp_nested(-5, 0, 100));
    printf("  clamp_flat  (-5, 0, 100)  = %d\n", clamp_flat(-5, 0, 100));
    printf("  clamp_flat  (50, 100, 0)  = %d  (参数非法，返回 lo)\n",
           clamp_flat(50, 100, 0));
    printf("  两者结果一致，但卫语句版本的缩进永远只有一层。\n");

    printf("---- 3. 状态机：把 if 森林变成一张表 ----\n");
    state_t cur = ST_IDLE;
    int events[4] = { 0, 1, 2, 3 };
    for (int i = 0; i < 4; i++) {
        state_t nxt = next_state_switch(events[i], cur);
        printf("  event=%d: %s -> %s\n", events[i],
               g_state_names[cur], g_state_names[nxt]);
        cur = nxt;
    }
    printf("  真实飞控的状态迁移表就是这个形状，只是条件更多。\n");

    printf("---- 4. goto 单一出口 ----\n");
    int out = 0;
    printf("  正常: rc=%d, out=%d\n", acquire_and_use(0, &out), out);
    printf("  第 1 步失败: rc=%d\n", acquire_and_use(1, &out));
    printf("  第 2 步失败: rc=%d\n", acquire_and_use(2, &out));
    printf("  好处：不管从哪一步失败，清理代码只有一份，不会漏释放。\n");
    return 0;
}
