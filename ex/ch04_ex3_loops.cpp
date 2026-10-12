/**
 * ch04_ex3_loops.c —— while / do-while / for 三种循环
 */
#include <stdio.h>
#include <stdint.h>

int main(void)
{
                /**
         * ---- 1. for：最常用 ----
         */
    printf("---- 1. for ----\n");
    printf("  0..4: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", i);
    }
    printf("\n");

                /* for 的第三个表达式可以用逗号运算符同时推进两个变量 */
    printf("  双变量: ");
    for (int i = 0, j = 10; i < j; i++, j--) {
        printf("(%d,%d) ", i, j);
    }
    printf("\n");

                /* for 的三个表达式都可以为空，但两个分号不能省 */
    printf("  空初始化和空更新: ");
    int k = 0;
    for (; k < 3;) {
        printf("%d ", k);
        k++;
    }
    printf("\n");

                /* 死循环的写法 */
    int guard = 0;
    for (;;) {                              /* 等价于 while (1) */
        if (++guard >= 3) {
            break;                                          /* 用 break 出来 */
        }
    }
    printf("  for(;;) 循环了 %d 次后用 break 出来\n", guard);

                /* ---- 2. while：条件驱动 ---- */
    printf("---- 2. while ----\n");
    int n = 1;
    int sum = 0;
    while (n <= 5) {                        /* 累加 1..5 */
        sum += n;
        n++;
    }
    printf("  1+2+3+4+5 = %d\n", sum);

                /* 模拟"等标志位"：嵌入式里最典型的 while 用法 */
    int ready = 0;
    int tries = 0;
    while (!ready && tries < 100) {          /* 必须有超时，否则可能死等 */
        tries++;
        if (tries == 3) {
            ready = 1;                                       /* 模拟硬件在第 3 次就绪 */
        }
    }
    printf("  等待就绪：第 %d 次探测到 ready=%d\n", tries, ready);
    printf("  【嵌入式铁律】等待硬件标志一定要带超时，否则硬件坏了程序就卡死。\n");

                /* ---- 3. do-while：至少执行一次 ---- */
    printf("---- 3. do-while ----\n");
    int zero = 0;
                /* 用 while 时，条件一开始就是假，一次都不执行 */
    int while_count = 0;
    while (zero > 0) {
        while_count++;
    }
    printf("  while(0>0) 执行了 %d 次\n", while_count);
                /* 用 do-while 时，先做一次再说 */
    int do_count = 0;
    do {
        do_count++;
    } while (zero > 0);
    printf("  do { } while(0>0) 执行了 %d 次  <-- 至少一次\n", do_count);

                /* do-while 的经典写法：把"读-判断"合成一个循环 */
    const char *cmds[] = { "AT", "AT+VER?", "OK", "AT+RESET" };
    int idx = 0;
    do {
        printf("  发送命令: %s\n", cmds[idx]);
        idx++;
    } while (idx < 4 && cmds[idx][0] != 'O');           /* 遇到以 O 开头的就停 */
    printf("  实际循环了 %d 次（送出 OK 之后，下一次判断发现命令以 O 开头就停了）\n",
           idx);

                /* 另一个常见用法：宏里的 do{}while(0) 包裹多条语句 */
#define DO_TWICE()  do { printf("  第一次\n"); printf("  第二次\n"); } while (0)
    DO_TWICE();
    printf("  【为什么宏要包 do{}while(0)】这样它在 if 后面当一条语句用，不会漏大括号。\n");

                /* ---- 4. break 和 continue ---- */
    printf("---- 4. break / continue ----\n");
    printf("  continue 跳过本次，break 结束整个循环：\n  ");
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            continue;                                       /* 偶数跳过 */
        }
        if (i > 7) {
            break;                                          /* 大于 7 就不再继续 */
        }
        printf("%d ", i);
    }
    printf("\n");

                /* ---- 5. 循环边界的两个经典错误 ---- */
    printf("---- 5. 边界错误 ----\n");
    int arr[5] = { 1, 2, 3, 4, 5 };
                /* 错误写法 1：i <= 5 会越界读 arr[5] */
    printf("  数组长度 = %zu，合法下标是 0..%zu\n",
           sizeof(arr) / sizeof(arr[0]), sizeof(arr) / sizeof(arr[0]) - 1);
                /* 错误写法 2：无符号下标倒着减会下溢成巨大值 */
    for (unsigned int i = 4; i != 0; i--) {
                                /* 必须写成 i != 0 或 i > 0，写成 i >= 0 会永远成立（无符号恒 >= 0） */
        if (i == 4) {
            printf("  倒着遍历: ");
        }
        printf("%d ", arr[i]);
    }
    printf("%d\n", arr[0]);
    printf("  【症状】unsigned i; for (i = n-1; i >= 0; i--) 是死循环，\n");
    printf("         因为 i>=0 对无符号永远为真，减到 0 后会变成 4294967295。\n");

                /* ---- 6. 循环里的性能与嵌入式注意点 ---- */
    printf("---- 6. 嵌入式里的循环 ----\n");
    printf("  a) 循环体里的除法/取模很贵（Cortex-M0+ 没有硬件除法），\n");
    printf("     能提到循环外就提出来，能用移位就用移位。\n");
    printf("  b) 空循环延时 for(i=0;i<1000;i++); 会被优化器删掉，\n");
    printf("     需要真延时要用 volatile 计数或者调用库延时函数。\n");
    printf("  c) 死等硬件的 while 必须带超时计数，见上面第 2 点。\n");
    uint32_t t = 0;
    volatile uint32_t spin = 0;
    for (t = 0; t < 1000; t++) {
        spin++;                                     /* volatile 保证不会被优化掉 */
    }
    printf("  volatile 计数循环实际跑了 %u 次（spin=%u）\n",
           (unsigned)t, (unsigned)spin);
    return 0;
}
