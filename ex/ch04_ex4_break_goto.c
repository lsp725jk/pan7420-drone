/**
 * ch04_ex4_break_goto.c —— break / continue / return 的区别，以及 goto 的唯一正当用法
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * ---- 1. break 只跳一层：嵌套循环的经典困惑 ----
 */
static void break_one_level(void)
{
    printf("  两层循环里 break 的效果：\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (j == 1) {
                break;                                              /* 只跳出 j 这层，i 那层继续 */
            }
            printf("    i=%d j=%d\n", i, j);
        }
    }
    printf("  【症状】以为 break 能一次跳出两层，结果外层还在转。\n");
}

/* ---- 2. 想跳出两层：加标志位 ---- */
static void break_two_levels_flag(void)
{
    int found = 0;
    int target_row = -1, target_col = -1;
    for (int i = 0; i < 3 && !found; i++) {                 /* 条件里带上标志 */
        for (int j = 0; j < 3; j++) {
            if (i * 3 + j == 4) {
                found = 1;
                target_row = i;
                target_col = j;
                break;                                                              /* 跳出内层 */
            }
        }
    }
    printf("  用标志位跳出两层：找到 (%d,%d)\n", target_row, target_col);
}

/* ---- 3. 想跳出两层：抽成函数 + return（最推荐）---- */
static int find_first(int *row, int *col)
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i * 3 + j == 4) {
                *row = i;
                *col = j;
                return 1;                                                           /* 一次跳到底 */
            }
        }
    }
    return 0;
}

/* ---- 4. continue：跳过本次 ---- */
static void continue_demo(void)
{
    printf("  把 1..10 里的奇数挑出来（偶数 continue 跳过）：\n  ");
    for (int i = 1; i <= 10; i++) {
        if (i % 2 == 0) {
            continue;
        }
        printf("%d ", i);
    }
    printf("\n");
    printf("  【坑】while 循环里用 continue 要小心：如果更新语句在循环体末尾，\n");
    printf("       continue 会跳过它，导致死循环：\n");
    int i = 0;
    int safety = 0;
    while (i < 5) {
        i++;                                        /* 把更新写在最前面才是安全的 */
        if (i == 3) {
            continue;                                       /* 这里跳过后 i 已经加过了，不会死循环 */
        }
        if (++safety > 20) {
            break;
        }
    }
    printf("      正确写法（i++ 放在 continue 之前）最终 i = %d\n", i);
}

/* ---- 5. goto 的正当用法：统一清理 ---- */
static int process_with_cleanup(int fail_at_step)
{
    char *buf1 = NULL;
    char *buf2 = NULL;
    int   rc = -1;                          /* 默认返回失败 */

    buf1 = (char *)malloc(64);
    if (buf1 == NULL) {
        goto cleanup;                               /* 还没申请到东西，直接去清理 */
    }
    if (fail_at_step == 1) {
        goto cleanup;
    }

    buf2 = (char *)malloc(64);
    if (buf2 == NULL) {
        goto cleanup;
    }
    if (fail_at_step == 2) {
        goto cleanup;
    }

    strcpy(buf1, "hello");
    strcpy(buf2, "world");
    rc = 0;                                 /* 全部成功 */
    printf("  两步都成功: %s %s\n", buf1, buf2);

                /* 这里是唯一的清理出口：不管从哪一步失败，都走到这 */
cleanup:
    free(buf1);                             /* free(NULL) 是安全的，所以不用判空 */
    free(buf2);
    return rc;
}

int main(void)
{
    printf("---- 1. break 只跳一层 ----\n");
    break_one_level();

    printf("---- 2. 跳出两层（标志位）----\n");
    break_two_levels_flag();

    printf("---- 3. 跳出两层（函数 + return，最推荐）----\n");
    {
        int r = -1, c = -1;
        if (find_first(&r, &c)) {
            printf("  找到 (%d,%d)\n", r, c);
        }
    }

    printf("---- 4. continue ----\n");
    continue_demo();

    printf("---- 5. goto 统一清理 ----\n");
    printf("  不失败: rc = %d\n", process_with_cleanup(0));
    printf("  第 1 步失败: rc = %d\n", process_with_cleanup(1));
    printf("  第 2 步失败: rc = %d\n", process_with_cleanup(2));

    printf("---- 6. goto 的规则与忌讳 ----\n");
    printf("  规则：goto 只能跳到**同一个函数内**的标号，不能跨函数。\n");
    printf("  规则：不能跳进一个变量的作用域（跳过它的初始化）。\n");
    printf("  忌讳：用它做循环（goto loop;）—— 那会写出没法维护的代码。\n");
    printf("  准许：用它做'单一出口的错误清理'，尤其是要释放多个资源时。\n");

                /* 用 goto 做循环（反面教材，跑一次就出来，别学） */
    {
        int n = 0;
    again:
        if (n < 3) {
            n++;
            goto again;                                     /* 这就是"面条代码"，只作演示 */
        }
        printf("  goto 当循环用了一次，n = %d（不推荐，能用 for 就用 for）\n", n);
    }
    return 0;
}
