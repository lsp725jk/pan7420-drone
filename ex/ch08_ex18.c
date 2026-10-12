/**
 * ch08_ex18.c —— assert（断言）：只在调试期生效的"自检"
 */
#include <assert.h>
#include <stdio.h>

static int clamp_pwm(int pwm)
{
                /**
         * 断言写"绝对不能发生的事"。它检查的是"我的程序逻辑对不对"，
         */
    assert(pwm >= 0);
    assert(pwm <= 2000);
    if (pwm > 1000) pwm = 1000;
    return pwm;
}

int main(void)
{
                /**
         * 变式 1：用 NDEBUG 看当前断言是开还是关
         */
#ifdef NDEBUG
    printf("NDEBUG 已定义 -> assert 被完全删除，一个字节的代码都不生成\n");
#else
    printf("NDEBUG 未定义 -> assert 生效（Release 构建请加 /DNDEBUG）\n");
#endif

                /* 变式 2：断言为真时什么也不做，程序继续 */
    printf("clamp_pwm(500)  = %d\n", clamp_pwm(500));
    printf("clamp_pwm(1500) = %d\n", clamp_pwm(1500));



    printf("断言全部通过\n");



                /* 坑 2：assert 失败会 abort()，在单片机上表现为"卡死/复位"， 所以嵌入式里要么保留断言配合看门狗，要么定义一个"失败就记录到 Flash"的版本 */
    return 0;
}
