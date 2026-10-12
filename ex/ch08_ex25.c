/**
 * ch08_ex25.c —— 【进阶变式】X-Macro 生成"命令表 + 函数指针"
 */
#include <stdio.h>
#include <string.h>

/**
 * ================= 唯一的"数据源"：命令表 =================
 */
#define CMD_LIST(X)             \
    X("arm",    cmd_arm)        \
    X("disarm", cmd_disarm)     \
    X("calib",  cmd_calib)      \
    X("reboot", cmd_reboot)

/* ---------- 展开 1：声明所有命令函数 ---------- */
#define X_DECL(name, fn)  static int fn(void);
CMD_LIST(X_DECL)
#undef X_DECL

/* ---------- 实际实现（顺序随意，声明已经由上面的宏生成） ---------- */
static int cmd_arm(void)    { printf("  -> 执行: 解锁电机\n");   return 0; }
static int cmd_disarm(void) { printf("  -> 执行: 锁定电机\n");   return 0; }
static int cmd_calib(void)  { printf("  -> 执行: 陀螺校准\n");   return 1; }
static int cmd_reboot(void) { printf("  -> 执行: 软件复位\n");   return 2; }

/* ---------- 展开 2：生成分派表 ---------- */
#define X_TABLE(name, fn)  { name, fn },
static const struct {
    const char *name;
    int (*fn)(void);
} g_cmd_table[] = {
    CMD_LIST(X_TABLE)
};
#undef X_TABLE

#define CMD_COUNT ((int)(sizeof(g_cmd_table) / sizeof(g_cmd_table[0])))

/* ---------- 展开 3：生成"帮助"文本 ---------- */
static void cmd_help(void)
{
    printf("可用命令：");
#define X_HELP(name, fn)  printf("%s ", name);
    CMD_LIST(X_HELP)
#undef X_HELP
    printf("\n");
}

/* ---------- 用表来分派 ---------- */
static int cmd_dispatch(const char *line)
{
    for (int k = 0; k < CMD_COUNT; k++) {
        if (strcmp(line, g_cmd_table[k].name) == 0)
            return g_cmd_table[k].fn();                              /* 通过函数指针调用 */
    }
    printf("  -> 未知命令: %s\n", line);
    return -1;
}

int main(void)
{
    printf("命令表（由 X-Macro 一张表生成 声明 + 实现 + 表 + 帮助）：\n");
    for (int k = 0; k < CMD_COUNT; k++)
        printf("  [%d] %-8s -> 函数 %p\n", k, g_cmd_table[k].name,
               (void *)(uintptr_t)g_cmd_table[k].fn);
    printf("\n");

    cmd_help();
    printf("\n");

                /* 模拟串口收到几行命令 */
    static const char *inputs[] = { "arm", "calib", "fly", "reboot", "disarm" };
    for (int k = 0; k < 5; k++) {
        printf("收到 \"%s\"\n", inputs[k]);
        int r = cmd_dispatch(inputs[k]);
        printf("     返回码 = %d\n", r);
    }

    printf("\n好处：加一条命令只需要在 CMD_LIST 里加一行 ——\n");
    printf("      函数声明、分派表、帮助文本三处全部自动跟上。\n");
    return 0;
}
