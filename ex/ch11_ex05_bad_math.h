// ch11_ex05_bad_math.h —— 反面教材：普通函数定义直接写进头文件
// 这个头文件会被 ch11_ex05_bad_a.cpp 和 ch11_ex05_bad_b.cpp 同时包含，
// 于是 square 出现了两份定义 -> 链接期 duplicate symbol。
#ifndef CH11_EX05_BAD_MATH_H
#define CH11_EX05_BAD_MATH_H

int square(int x) { return x * x; }   // 错误：函数定义在头文件里

#endif
