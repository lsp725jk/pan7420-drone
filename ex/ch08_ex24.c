/**
 * ch08_ex24.c —— 【变式】多行宏的"临时变量命名"与 #pragma message
 */
#include <stdio.h>

/**
 * ---------- 正确写法：花括号 + 防撞名的临时变量 ----------
 */
#define SUM_OK(a, b)  do { int sum_ok_tmp_ = (a) + (b); (a) = sum_ok_tmp_; } while (0)

/**
 * ---------- 忘了花括号的写法（只在注释里演示，因为它编译不过） ----------
 */

int main(void)
{
                /* 1) 正常情况下，宏内部的临时变量被块作用域"锁住"，外面的同名变量不受影响 */
    int sum_ok_tmp_ = 999;                         /* 故意和宏内部的名字一样 */
    int x = 1, y = 2;
    SUM_OK(x, y);
    printf("1) SUM_OK 之后：x = %d , 外面的 sum_ok_tmp_ = %d （没被碰到）\n",
           x, sum_ok_tmp_);
    printf("   因为宏体有 do{ }while(0)，临时变量在内部块作用域里\n\n");

                /* 2) 变式：宏体里用"通用名"的后果（这里只演示"遮蔽"这个现象） */
    {
        int sum_ok_tmp_ = 100;                             /* 和外层同名，但在内层块 */
        printf("2) 内层块里的 sum_ok_tmp_ = %d （遮蔽了外层的 999）\n", sum_ok_tmp_);
    }
    printf("   回到外层：sum_ok_tmp_ = %d\n", sum_ok_tmp_);
    printf("   命名约定：宏内部的临时变量加后缀 _ 或前缀，越「不可能重名」越好\n\n");

                /* 3) 变式：宏里需要提前退出时，只能靠 do-while(0) 的 break */
#define FIND_FIRST_NEG(arr, n, out)                       \
    do {                                                  \
        (out) = -1;                                       \
        for (int find_i_ = 0; find_i_ < (n); find_i_++) { \
            if ((arr)[find_i_] < 0) { (out) = find_i_; break; } \
        }                                                 \
    } while (0)

    static const int v[5] = { 3, 7, -2, 9, -5 };
    int idx;
    FIND_FIRST_NEG(v, 5, idx);
    printf("3) FIND_FIRST_NEG 找到第一个负数的下标 = %d （值 %d）\n", idx, v[idx]);
    printf("   宏体里的 break 跳出的是 do-while(0)，不是外层的 for —— 这正是要的效果\n\n");

                /* ---------- 4) #pragma message 替代 #warning ---------- */
                /* 本机实测：MSVC 在 /std:c17 下 #warning 是 fatal error C1188； #pragma message 则只打印一行，编译照常通过。 */
#define DEMO_SHOW_MESSAGE 1
#if DEMO_SHOW_MESSAGE
#pragma message("提示：这一行是 #pragma message 的输出，编译能通过")
#endif

    printf("4) #pragma message 只打印一行提示，不影响编译（本机实测见下面的输出）\n");
    printf("   而 #warning 在 MSVC 的 /std:c17 下是 fatal error（见例 8-13 的实测）\n");
    return 0;
}
