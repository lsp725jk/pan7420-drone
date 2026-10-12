/**
 * ch03_ex1_arithmetic.c —— 算术运算符：除法、取模、混合类型
 */
#include <stdio.h>
#include <math.h>       /**
 * fmod 在这里
 */

int main(void)
{
                /**
         * ---- 1. 整数除法是截断，不是四舍五入 ----
         */
    printf("---- 1. 整数除法 ----\n");
    printf("5 / 2     = %d\n", 5 / 2);
    printf("-5 / 2    = %d   (向零取整,不是 -3)\n", -5 / 2);
    printf("5 %% 2     = %d\n", 5 % 2);
    printf("-5 %% 2    = %d   (C99 起:结果符号跟被除数)\n", -5 % 2);
    printf("5 / -2    = %d\n", 5 / -2);
    printf("5 %% -2    = %d\n", 5 % -2);
    printf("7 / 2 * 2 = %d   <-- 先除后乘,精度已经丢了(期待 7,实际 6)\n",
           7 / 2 * 2);

                /* ---- 2. 想得到小数，必须至少一边是浮点 ---- */
    printf("---- 2. 混合类型 ----\n");
    int   a = 5;
    int   b = 2;
    printf("a / b            = %d\n", a / b);
    printf("(double)a / b    = %f   <-- 把一边转成 double 就对了\n",
           (double)a / b);
    printf("a / (double)b    = %f\n", a / (double)b);
    printf("a / 2.0          = %f   (2.0 是 double,整条式子按 double 算)\n",
           a / 2.0);
    printf("a / 2.0f         = %f   (2.0f 是 float,按 float 算再提升)\n",
           (double)(a / 2.0f));
    printf("a * 1.0 / b      = %f   (乘以 1.0 是另一种常见技巧)\n", a * 1.0 / b);

                /* ---- 3. % 只能用于整数：常用场景 ---- */
    printf("---- 3. 取模的实用场景 ----\n");
    for (int i = 0; i < 6; i++) {
        printf("%d %% 3 = %d", i, i % 3);
        if (i % 3 == 0) {
            printf("   <-- 能被 3 整除");
        }
        printf("\n");
    }
        /* 判断奇偶、做环形缓冲区的下标回绕（嵌入式里天天用）： buf_index = (buf_index + 1) % BUF_SIZE; 取模的代价是除法，慢；如果 BUF_SIZE 是 2 的幂， 可以用 & (BUF_SIZE-1) 代替，快得多。 */
    printf("环形下标 (i+1)%%4 的一个周期: ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", (i + 1) % 4);
    }
    printf("\n用位与代替(仅当 4 是 2 的幂): ");
    for (int i = 0; i < 8; i++) {
        printf("%d ", (i + 1) & 3);
    }
    printf("\n");

                /* ---- 4. 一元运算符与复合赋值 ---- */
    printf("---- 4. 一元与复合赋值 ----\n");
    int x = 10;
    printf("-x = %d   +x = %d\n", -x, +x);
    x += 5;  printf("x += 5  -> %d\n", x);
    x -= 3;  printf("x -= 3  -> %d\n", x);
    x *= 2;  printf("x *= 2  -> %d\n", x);
    x /= 4;  printf("x /= 4  -> %d\n", x);
    x %= 4;  printf("x %%= 4  -> %d\n", x);
                /* 复合赋值 = 运算 + 赋值，它只对左边求值一次，比写全更安全也更快 */

                /* ---- 5. 溢出的现实例子：算平均值的经典写法 ---- */
    printf("---- 5. 算平均值的坑 ----\n");
    int p = 2000000000;
    int q = 2000000000;
    printf("(p+q)/2 用 int 算 = %d   <-- p+q 已经溢出(UB)\n", (p + q) / 2);
    printf("p/2 + q/2         = %d   <-- 分着除,不溢出但丢精度\n",
           p / 2 + q / 2);
    printf("用 long long 算   = %lld  <-- 正确做法\n",
           ((long long)p + (long long)q) / 2);

                /* ---- 6. 浮点的 % 要用 fmod ---- */
    printf("---- 6. 浮点取余用 fmod ----\n");
    printf("5.5 对 2.0 取余: fmod(5.5,2.0) = %f\n", fmod(5.5, 2.0));
    printf("（fmod 需要 #include <math.h>，见文件头的包含）\n");
    return 0;
}
