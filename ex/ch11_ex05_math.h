// ch11_ex05_math.h —— 正确写法：头文件里只留声明，定义放到 .cpp
#ifndef CH11_EX05_MATH_H
#define CH11_EX05_MATH_H

int square(int x);                       // 声明

inline int cube(int x) { return x * x * x; }   // 想在头文件里写定义就加 inline

#endif
