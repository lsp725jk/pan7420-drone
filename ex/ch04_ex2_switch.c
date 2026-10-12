/**
 * ch04_ex2_switch.c —— switch：多分支、贯穿、default
 */
#include <stdio.h>

/**
 * ---- 1. 最标准的用法：每个 case 都以 break 结束 ----
 */
static const char *state_name_ok(int state)
{
    switch (state) {
    case 0:
        return "IDLE";
    case 1:
        return "ARMED";
    case 2:
        return "FLYING";
    default:
        return "UNKNOWN";
    }
                /* 用 return 时不需要 break，因为函数已经退出了。 但注意：这样写编译器可能提示"并非所有路径都有返回值"， 所以 default 一定要有。 */
}

/* ---- 2. 忘记 break 会怎样 ---- */
static void state_name_buggy(int state)
{
    printf("  switch(%d) 忘记 break 的结果：", state);
    switch (state) {
    case 0:
        printf("IDLE ");
                                /* 这里少了一个 break，会继续往下掉 */
    case 1:
        printf("ARMED ");
    case 2:
        printf("FLYING ");
        break;
    default:
        printf("UNKNOWN ");
        break;
    }
    printf("\n");
}

/* ---- 3. 故意贯穿：多个 case 共用一段代码 ---- */
static int is_vowel(char c)
{
    switch (c) {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
        return 1;                                   /* 五个 case 共用这一个 return */
    default:
        return 0;
    }
}

int main(void)
{
                /* ---- 1. 基本用法 ---- */
    printf("---- 1. 基本 switch ----\n");
    for (int s = 0; s <= 3; s++) {
        printf("  state %d -> %s\n", s, state_name_ok(s));
    }

                /* ---- 2. 忘记 break 的症状 ---- */
    printf("---- 2. 贯穿的坑 ----\n");
    for (int s = 0; s <= 3; s++) {
        state_name_buggy(s);
    }
    printf("【症状】case 0 会连着打印 IDLE ARMED FLYING。\n");
    printf("        MSVC /W4 对可能有意贯穿的 case 通常不给警告（编译器无法判断意图），\n");
    printf("        所以这个 bug 通常要靠单步调试或代码评审发现。\n");

                /* ---- 3. 故意贯穿 ---- */
    printf("---- 3. 故意贯穿（合法且常用）----\n");
    const char *word = "hello world";
    printf("  \"%s\" 里的元音字母：", word);
    for (const char *p = word; *p; p++) {
        if (is_vowel(*p)) {
            printf("%c ", *p);
        }
    }
    printf("\n");

                /* ---- 4. case 里的声明需要大括号 ---- */
    printf("---- 4. case 里声明变量 ----\n");
    int code = 2;
    switch (code) {
    case 1: {
        int tmp = 10;                               /* 用 {} 把作用域框起来 */
        printf("  case 1: tmp=%d\n", tmp);
        break;
    }
    case 2: {
        int tmp = 20;                               /* 这里的 tmp 和上面那个不冲突 */
        printf("  case 2: tmp=%d\n", tmp);
        break;
    }
    default:
        printf("  default\n");
        break;
    }
    printf("【坑】不在 case 里加大括号、又声明变量并初始化，\n");
    printf("      会报 error C2360: \"tmp\"的初始化操作由\"case\"标签跳过。\n");

                /* ---- 5. switch 只能用于整型 ---- */
    printf("---- 5. switch 的限制 ----\n");
    char ch = 'B';
    switch (ch) {                           /* char 可以，因为它会被提升为 int */
    case 'A': printf("  A\n"); break;
    case 'B': printf("  B\n"); break;
    default:  printf("  其它\n"); break;
    }
        /* 下面是编译不过的写法（列出来是为了让你记住）： double d = 1.5; switch (d) { ... }                  // 错：浮点不行 switch ("abc") { ... }              // 错：字符串不行 */
    printf("  switch 只吃整型常量 case；字符串、浮点、区间都不行。\n");
    printf("  需要按字符串分派，只能 if-else + strcmp，或查表。\n");

                /* ---- 6. 用查表代替 switch：更短更快 ---- */
    printf("---- 6. switch 的替代方案：查表 ----\n");
    static const char *names[] = { "IDLE", "ARMED", "FLYING", "FAILSAFE" };
    for (int s = 0; s < 4; s++) {
                                /* 取值连续、范围已知时，查表比 switch 更清晰，也没有贯穿风险 */
        printf("  表查法 state %d -> %s\n", s, names[s]);
    }
    printf("【什么时候用 switch】分支里要做的事情不一样、每个分支好几行；\n");
    printf("【什么时候查表】只是把编号翻译成名字/系数这种一一对应。\n");

                /* ---- 7. 枚举 + switch：最推荐的组合 ---- */
    printf("---- 7. 枚举 + switch ----\n");
    enum flight_mode { MODE_IDLE = 0, MODE_ARM = 1, MODE_FLY = 2 };
    enum flight_mode fm = MODE_FLY;
    switch (fm) {
    case MODE_IDLE: printf("  空闲\n"); break;
    case MODE_ARM:  printf("  已解锁\n"); break;
    case MODE_FLY:  printf("  飞行中\n"); break;
    default:        printf("  未知模式\n"); break;
    }
    printf("用枚举而不是裸数字，编译器能帮你检查拼写错误，代码也自解释。\n");
    return 0;
}
