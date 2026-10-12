/**
 * ch08_ex19.c —— X-Macro：用一张"表"同时生成枚举、名字表、解析函数
 */
#include <stdio.h>
#include <string.h>

/**
 * 唯一的"数据源"：飞行模式表（名字, 编号, 中文说明）
 */
#define FLIGHT_MODE_LIST(X)          \
    X(MODE_IDLE,      0, "待机")     \
    X(MODE_STABILIZE, 1, "自稳")     \
    X(MODE_ALT_HOLD,  2, "定高")     \
    X(MODE_HEADLESS,  3, "无头")

/**
 * 展开 1：生成枚举。最后补一个 COUNT 当数组长度
 */
#define X_ENUM(name, value, text) name = value,
enum FlightMode {
    FLIGHT_MODE_LIST(X_ENUM)
    MODE_COUNT
};
#undef X_ENUM                /* 必须 #undef，否则同一个 X 会被重定义 */

/* 展开 2：生成"编号 -> 名字"的数组 */
#define X_TEXT(name, value, text) text,
static const char *const g_mode_text[MODE_COUNT] = {
    FLIGHT_MODE_LIST(X_TEXT)
};
#undef X_TEXT

/* 展开 3：生成"名字 -> 编号"的查表循环体 */
#define X_CASE(name, value, text) if (strcmp(s, text) == 0) return name;
static enum FlightMode mode_parse(const char *s)
{
    FLIGHT_MODE_LIST(X_CASE)
    return MODE_COUNT;               /* 没匹配上 */
}
#undef X_CASE

/* 展开 4：生成调试打印 */
#define X_PRINT(name, value, text) \
    printf("  %-16s = %d  (%s)\n", #name, (int)name, text);
static void mode_dump(void)
{
    FLIGHT_MODE_LIST(X_PRINT)
}
#undef X_PRINT

int main(void)
{
    printf("枚举与中文名（由同一张表生成）：\n");
    mode_dump();

    printf("按编号取名：g_mode_text[2] = %s\n", g_mode_text[2]);

    printf("按名字解析：\"定高\" -> %d , \"不存在\" -> %d\n",
           (int)mode_parse("定高"), (int)mode_parse("不存在"));

    printf("MODE_COUNT = %d （自动等于条目数，加一行数据就自动变大）\n", MODE_COUNT);
    return 0;
}
