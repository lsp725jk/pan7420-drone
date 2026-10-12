/**
 * ch08_ex18_fail.c —— 故意让断言失败，看真实输出与退出码
 */
#include <assert.h>
#include <stdio.h>

int main(void)
{
    int battery_mv = 3600;
    printf("电池电压 = %d mV，下面这一句断言会失败\n", battery_mv);
    fflush(stdout);                      /**
     * 先把上面的输出刷出来，否则 abort 可能丢掉它
     */
    assert(battery_mv >= 4000);          /**
     * 条件不成立 -> 打印诊断信息并 abort()
     */
    printf("这一行永远不会执行\n");
    return 0;
}
