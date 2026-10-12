// ch07_ex3.cpp —— 指向结构体的指针与 -> 运算符
// 编译：check.bat ch07_ex3.cpp c++latest
#include <cstdio>
#include <cstring>

struct Motor {
    int id;
    int rpm;
    char name[12];
};

// 传指针：只传 8 字节，且函数里能改到外面的对象
void set_rpm_ptr(struct Motor* m, int rpm)
{
    m->rpm = rpm;                    // -> 等价于 (*m).rpm
}

// 传值：拷贝一份，改不动外面
void set_rpm_val(struct Motor m, int rpm)
{
    m.rpm = rpm;                     // 只改了副本
}

int main()
{
    struct Motor m = { 1, 1000, "M1" };

        // 三种访问写法，完全等价
    printf("[1] 三种等价写法:\n");
    printf("    m.rpm        = %d   （对象用点号）\n", m.rpm);
    struct Motor* p = &m;
    printf("    (*p).rpm     = %d   （先解引用再用点号，括号不能省）\n", (*p).rpm);
    printf("    p->rpm       = %d   （-> 是 (*p). 的简写）\n", p->rpm);
    printf("    p->name      = \"%s\"\n", p->name);

        // -> 的真实便利：链式访问不用写括号
    struct Node { int v; struct Node* next; };
    struct Node n2 = { 2, nullptr };
    struct Node n1 = { 1, &n2 };
    printf("\n[2] 链表式的链式访问:\n");
    printf("    n1.next->v        = %d\n", n1.next->v);
    printf("    (*(*n1.next).next) 太丑，所以有了 ->\n");

        // 通过指针改内容
    p->rpm = 2000;
    printf("\n[3] p->rpm = 2000 之后 m.rpm = %d  （改的是同一个对象）\n", m.rpm);

        // 结构体指针的运算：步长是整个结构体
    struct Motor arr[3] = { {1,100,"A"}, {2,200,"B"}, {3,300,"C"} };
    struct Motor* q = arr;
    printf("\n[4] 结构体指针的步长:\n");
    printf("    sizeof(struct Motor) = %zu\n", sizeof(struct Motor));
    printf("    q   = %p  q->id = %d\n", (void*)q, q->id);
    printf("    q+1 = %p  (q+1)->id = %d   地址差 %td 字节\n",
           (void*)(q + 1), (q + 1)->id, (char*)(q + 1) - (char*)q);
    printf("    q[2].id = %d  （指针当下标用也行）\n", q[2].id);

        // 传值和传指针的差别
    printf("\n[5] 函数传参:\n");
    set_rpm_val(m, 5555);
    printf("    传值后 m.rpm   = %d  ← 没变（改的是副本，白花一次拷贝）\n", m.rpm);
    set_rpm_ptr(&m, 5555);
    printf("    传指针后 m.rpm = %d  ← 变了\n", m.rpm);
    printf("    sizeof(struct Motor) = %zu，传值每次都要拷这么多字节\n", sizeof(struct Motor));

        // 指向结构体指针的指针
    struct Motor** pp = &p;
    (*pp)->rpm = 7777;
    printf("\n[6] 二级指针 (*pp)->rpm = 7777 之后 m.rpm = %d\n", m.rpm);
    return 0;
}
