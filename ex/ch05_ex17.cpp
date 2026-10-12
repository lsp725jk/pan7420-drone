// ch05_ex17.cpp —— const 与指针的四种组合
// 编译：check.bat ch05_ex17.cpp c++latest
#include <cstdio>

void read_only(const char* s)      { printf("  只读: %s\n", s); }
void write_ptr(char* const s)      { s[0] = 'X'; printf("  改内容(指针本身不能改): %s\n", s); }

int main()
{
    char buf[16] = "hello";
    const char* lit = "literal";

        // 组合 1：指向常量的指针 —— 不能改内容，能改指针
    const char* p1 = buf;
    printf("[1] const char* p1（内容只读）\n");
    printf("    *p1 = '%c'\n", *p1);
        // *p1 = 'H';    // 编译错误：不能给 const 对象赋值
    p1 = lit;        // 合法：改的是 p1 自己
    printf("    p1 重新指向字面量后 = \"%s\"\n", p1);

        // 组合 2：常量指针 —— 能改内容，不能改指针
    char* const p2 = buf;
    printf("[2] char* const p2（指针只读）\n");
    *p2 = 'H';       // 合法：buf 是可写的
    printf("    *p2='H' 之后 buf = \"%s\"\n", buf);
        // p2 = lit;     // 编译错误：p2 本身是 const

        // 组合 3：两者都 const
    const char* const p3 = buf;
    printf("[3] const char* const p3（都只读）\n");
    printf("    *p3 = '%c'，p3 = %p\n", *p3, (const void*)p3);
        // *p3 = 'x';    // 错误
        // p3 = lit;     // 错误

        // 组合 4：都不 const —— 能改内容也能改指针
    char* p4 = buf;
    printf("[4] char* p4（都不只读）\n");
    *p4 = 'Y';
    p4 = (char*)lit;      // 需要强制转换去掉 const，因为 lit 指向只读内存
    printf("    buf=\"%s\"  p4 现在指向只读区（写它会崩）\n", buf);
    p4 = buf;             // 转回可写内存，避免踩坑

    printf("\n函数参数里 const 的作用：\n");
    read_only("abc");            // 字面量能直接传进来
    read_only(buf);
    write_ptr(buf);

    printf("\n怎么读这些声明（从变量名往外读）：\n");
    printf("  const char* p        → p 是「指向 const char 的指针」\n");
    printf("  char* const p        → p 是「const 的、指向 char 的指针」\n");
    printf("  const char* const p  → 两样都 const\n");
    printf("\n最后 buf = \"%s\"\n", buf);
    return 0;
}
