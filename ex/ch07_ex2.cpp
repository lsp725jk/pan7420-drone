// ch07_ex2.cpp —— 嵌套结构体：结构体里放结构体、放数组、放指针
// 编译：check.bat ch07_ex2.cpp c++latest
#include <cstdio>
#include <cstring>

struct Date { int year, month, day; };

struct Person {
    char name[16];
    struct Date birthday;      // 嵌套：值语义，内存就在 Person 里面
    struct Date* p_hire;       // 指针：内存不在 Person 里，需要另外分配
};

// 三层嵌套
struct Line { struct Point { int x, y; } a, b; };
struct Rect { struct Line top, bottom; };

int main()
{
    struct Person p;
    strcpy(p.name, "Amy");
    p.birthday.year = 2001;    // 逐级用点号访问
    p.birthday.month = 5;
    p.birthday.day = 20;

    struct Date hire = { 2024, 3, 1 };
    p.p_hire = &hire;          // 指向外面的一个 Date

    printf("[1] 嵌套访问:\n");
    printf("    p.name           = \"%s\"\n", p.name);
    printf("    p.birthday       = %d-%02d-%02d\n",
           p.birthday.year, p.birthday.month, p.birthday.day);
    printf("    p.p_hire->year   = %d  （指针成员要用 ->）\n", p.p_hire->year);

    printf("\n[2] 嵌套初始化（大括号逐层对应）:\n");
    struct Person q = { "Bob", { 1999, 12, 31 }, nullptr };
    printf("    q.name=\"%s\" q.birthday=%d-%02d-%02d q.p_hire=%p\n",
           q.name, q.birthday.year, q.birthday.month, q.birthday.day, (void*)q.p_hire);

        // C++20 指定初始化可以只写想给的字段
    struct Person r = { .name = "Cid", .birthday = { .year = 2010 } };
    printf("    r（C++20 指定初始化）: name=\"%s\" birthday=%d-%02d-%02d\n",
           r.name, r.birthday.year, r.birthday.month, r.birthday.day);

    printf("\n[3] 内存有多大：嵌套是「包含」，指针只是「引用」\n");
    printf("    sizeof(struct Date)   = %zu\n", sizeof(struct Date));
    printf("    sizeof(struct Person) = %zu\n", sizeof(struct Person));
    printf("      = char[16] (%zu) + Date (%zu) + 对齐填充 + 指针 (%zu)\n",
           sizeof(char[16]), sizeof(struct Date), sizeof(struct Date*));
    printf("    如果 birthday 改成 Date*，Person 就只剩 16+8 = 24 字节左右\n");
    printf("    结构体里有指针 → 拷贝结构体时是「浅拷贝」，两个结构体指向同一个 Date\n");

    printf("\n[4] 多层嵌套的定义写法:\n");
    struct Rect rect;
    rect.top.a.x = 1; rect.top.a.y = 2;
    rect.bottom.b.x = 30; rect.bottom.b.y = 40;
    printf("    rect.top.a = (%d,%d), rect.bottom.b = (%d,%d)\n",
           rect.top.a.x, rect.top.a.y, rect.bottom.b.x, rect.bottom.b.y);
    printf("    更常见的做法是先定义 struct Point，再 struct Line { Point a, b; };\n");
    return 0;
}
