// ch11_ex05_bad_b.cpp —— 反面教材（会链接失败）：翻译单元 B + main
#include "ch11_ex05_bad_math.h"
#include <cstdio>
int use_square_a(int v);
int main()
{
    std::printf("use_square_a(5)=%d, square(6)=%d\n", use_square_a(5), square(6));
    return 0;
}
