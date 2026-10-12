// ch05_ex14.cpp —— char* 与字符串字面量：能不能改？
// 编译：check.bat ch05_ex14.cpp c++latest
#include <cstdio>
#include <cstring>

int main()
{
        // 1) 字符串字面量的类型是 const char[N]（C++ 里带 const）
    const char* s1 = "hello";
    char buf[16] = "hello";      // 数组：把字面量的内容拷进自己的栈内存

    printf("s1        = \"%s\"   sizeof(s1)=%zu （指针）\n", s1, sizeof(s1));
    printf("buf       = \"%s\"   sizeof(buf)=%zu （数组，15 个字符空格 + 结尾的 \\0）\n",
           buf, sizeof(buf));
    printf("strlen(s1)=%zu  strlen(buf)=%zu\n", strlen(s1), strlen(buf));

        // 两个内容一样的字面量，地址可能相同（编译器合并字符串）
    const char* s2 = "hello";
    printf("\n字面量地址可能被合并: s1=%p s2=%p 相同? %s\n",
           (void*)s1, (void*)s2, (s1 == s2) ? "是" : "否");

        // 数组和字面量的地址必然不同，因为数组是独立内存
    printf("buf=%p  （一定和上面不同）\n", (void*)buf);

        // 2) 可以改的是数组
    buf[0] = 'H';
    printf("\n改 buf[0]='H' 之后: %s  （合法：buf 是自己的一块可写内存）\n", buf);

        // 3) 不能改的是字面量 —— 下面这行会崩，已注释
        // s1[0] = 'H';   // UB：写只读数据段。MSVC 下通常报"访问冲突"
    printf("s1[0]='%c' 只读，不能写（写它会崩）\n", s1[0]);

        // 4) 数组形式的只读字符串：内容在数组自己的内存里，但因为数组是 const 的，
        // 编译期就禁止你写它
    const char msg[] = "read-only";
    printf("\nconst char msg[] = \"%s\"  sizeof(msg)=%zu\n", msg, sizeof(msg));
        // msg[0] = 'R';   // 编译期就报错：不能给 const 变量赋值

        // 对照：不加 const 的字符数组是可写的（内容在栈上，不是只读区）
    char mutable_msg[] = "read-only";
    mutable_msg[0] = 'R';
    printf("char mutable_msg[] = \"%s\"  ← 可写，因为内容在栈上\n", mutable_msg);

        // 5) 用指针遍历字符串
    printf("\n用指针遍历 s1: ");
    for (const char* p = s1; *p != '\0'; ++p) putchar(*p);
    printf("\n用数组下标遍历: ");
    for (size_t i = 0; s1[i] != '\0'; ++i) putchar(s1[i]);
    printf("\n");
    return 0;
}
