// ch07_ex14.cpp —— 状态机：enum 状态 + 结构体传参 + switch（综合小例子）
// 编译：check.bat ch07_ex14.cpp c++latest
#include <cstdio>
#include <cstring>

// 枚举做状态（末尾加 _COUNT 便于检查合法性）
typedef enum {
    ST_IDLE = 0,
    ST_ARMED,
    ST_RUNNING,
    ST_FAULT,
    ST_COUNT
} State;

// 转成可读字符串：用一张表，和枚举顺序一一对应
static const char* kStateNames[ST_COUNT] = {
    "IDLE", "ARMED", "RUNNING", "FAULT"
};

typedef struct {
    State state;
    int   ticks;
    int   error;              // 0 = 无错误
    char  last_event[16];
} Context;

// 状态迁移函数：传指针，直接改调用者的 Context
void transition(Context* ctx, State next, const char* event)
{
    if ((int)next < 0 || next >= ST_COUNT) {          // 用 _COUNT 做边界检查
        printf("    [拒绝] 非法状态值 %d\n", (int)next);
        return;
    }
    printf("    %-8s → %-8s  (事件: %s)\n",
           kStateNames[ctx->state], kStateNames[next], event);
    ctx->state = next;
    ctx->ticks = 0;
    snprintf(ctx->last_event, sizeof(ctx->last_event), "%s", event);
}

int main()
{
    Context ctx;
    memset(&ctx, 0, sizeof(ctx));      // 全清零，state 变成 ST_IDLE
    printf("[1] 初始状态: %s  ticks=%d error=%d\n",
           kStateNames[ctx.state], ctx.ticks, ctx.error);

    printf("\n[2] 走一遍状态迁移:\n");
    transition(&ctx, ST_ARMED,   "arm_cmd");
    transition(&ctx, ST_RUNNING, "throttle_up");
    ctx.ticks += 100;
    printf("    跑了 100 个 tick，ticks=%d\n", ctx.ticks);
    transition(&ctx, ST_FAULT,   "sensor_lost");
    ctx.error = 42;

    printf("\n[3] 根据状态做事（switch）:\n");
    switch (ctx.state) {
    case ST_IDLE:     printf("    待机：可以做低功耗\n"); break;
    case ST_ARMED:    printf("    已解锁：等油门\n"); break;
    case ST_RUNNING:  printf("    运行中：正常控制\n"); break;
    case ST_FAULT:
        printf("    故障：last_event=\"%s\" error=%d\n", ctx.last_event, ctx.error);
        transition(&ctx, ST_IDLE, "reset");
        break;
    default:          printf("    未知状态（理论上到不了，用 _COUNT 保证不会漏）\n"); break;
    }

    printf("\n[4] 枚举做状态的好处:\n");
    printf("    - 状态名可读：ST_FAULT 比 3 好懂\n");
    printf("    - ST_COUNT 让边界检查、字符串表长度自动跟着走\n");
    printf("    - switch 覆盖不完时编译器能警告（加上 ST_COUNT 更明显）\n");
    printf("    sizeof(Context) = %zu 字节（State %zu + 2 个 int %zu + char[16] + 填充）\n",
           sizeof(Context), sizeof(State), 2 * sizeof(int));
    printf("    sizeof(State) = %zu（枚举默认按 int 存；想省内存用 enum : unsigned char）\n",
           sizeof(State));
    return 0;
}
