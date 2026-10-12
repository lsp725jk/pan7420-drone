// ch07_ex1.cpp —— struct 的定义、初始化、访问
// 编译：check.bat ch07_ex1.cpp c++latest
#include <cstdio>
#include <cstring>

// 定义（declaration）：只是描述"有哪些字段"，不分配内存
struct Point {
    int x;
    int y;
};                     // ← 这个分号不能省，省了报一堆莫名其妙的错

// 定义的同时声明变量（不推荐，但能看懂别人的代码）
struct Sensor { int id; double val; } s_global;

// 结构体里的数组成员：整个数组是结构体的一部分
struct Named { int id; char name[16]; };

int main()
{
        // 初始化方式 1：逐个赋值（最笨但最清楚）
    struct Point p1;
    p1.x = 3;
    p1.y = 4;

        // 初始化方式 2：聚合初始化（aggregate initialization），顺序必须和字段一致
    struct Point p2 = { 5, 6 };

        // 初始化方式 3：C++11 起的列表初始化，= 可省
    struct Point p3{ 7, 8 };
    struct Point p4 = { 9 };          // 只给第一个，其余补 0
    struct Point p5 = { };            // 全 0
    struct Point p6{};

    printf("p1 = (%d, %d)   逐个赋值\n", p1.x, p1.y);
    printf("p2 = (%d, %d)   = {5, 6}\n", p2.x, p2.y);
    printf("p3 = (%d, %d)   {7, 8}\n", p3.x, p3.y);
    printf("p4 = (%d, %d)   = {9}   ← 没给的字段自动补 0\n", p4.x, p4.y);
    printf("p5 = (%d, %d)   = { }\n", p5.x, p5.y);
    printf("p6 = (%d, %d)   { }    （C++11 起）\n", p6.x, p6.y);

        // 指定初始化（C++20 起；C99 也有类似语法，但规则不同：C++ 要求按声明顺序写）
    struct Point p7 = { .x = 10, .y = 20 };
    printf("p7 = (%d, %d)   C++20 指定初始化 { .x=10, .y=20 }\n", p7.x, p7.y);
    printf("  注意：C++ 要求指定初始化按字段声明顺序写，写反了会报 C7560\n");
    printf("  （C 语言里没这个限制，可以 { .y=20, .x=10 }）\n");

        // 结构体可以整体赋值（是"逐字节复制"，不是共享）
    struct Point p8 = p2;
    p8.x = 999;
    printf("\n整体赋值 p8 = p2; 然后改 p8.x=999:\n");
    printf("  p2 = (%d, %d)  p8 = (%d, %d)  ← 互不影响，是两份独立数据\n",
           p2.x, p2.y, p8.x, p8.y);

        // 结构体里的数组成员不能整体赋值，要用 strcpy
    struct Named n1;
    n1.id = 1;
    strcpy(n1.name, "temp");          // n1.name = "xxx"; 是错的
    printf("\n结构体内的数组: id=%d name=\"%s\" sizeof(struct Named)=%zu\n",
           n1.id, n1.name, sizeof(struct Named));

        // sizeof 是各字段对齐后的总和，不一定等于字段之和
    printf("sizeof(struct Point) = %zu （2 个 int = 8）\n", sizeof(struct Point));
    printf("sizeof(struct Named) = %zu （int 4 + char[16] = 20，刚好对齐）\n", sizeof(struct Named));
    printf("全局那个 s_global 也被零初始化了: id=%d val=%f\n", s_global.id, s_global.val);
    return 0;
}
