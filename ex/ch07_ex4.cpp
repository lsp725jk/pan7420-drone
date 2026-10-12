// ch07_ex4.cpp —— 结构体传参的代价：值传递到底拷贝了多少
// 编译：check.bat ch07_ex4.cpp c++latest
#include <cstdio>
#include <cstring>

struct Small { int a; int b; };                     // 8 字节

struct Big {                                        // 大约 1KB
    int  id;
    char name[64];
    double samples[100];
    int  flags[32];
};

// 三种形参写法
void by_value(struct Big b)             { b.id = -1; }                 // 整个结构体拷一份
void by_pointer(struct Big* b)          { b->id = -1; }                // 只传 8 字节地址
void by_const_pointer(const struct Big* b) { (void)b; }                // 只读，最推荐

// 返回结构体：也会拷贝（编译器可能用 RVO 优化掉，但不是标准保证）
struct Big make_big(int id)
{
    struct Big b;
    memset(&b, 0, sizeof(b));
    b.id = id;
    return b;
}

int main()
{
    printf("[1] 大小对比:\n");
    printf("    sizeof(struct Small) = %zu 字节\n", sizeof(struct Small));
    printf("    sizeof(struct Big)   = %zu 字节\n", sizeof(struct Big));
    printf("    传值一次就拷 %zu 字节；传指针只拷 %zu 字节\n", sizeof(struct Big), sizeof(struct Big*));

    struct Big b;
    memset(&b, 0, sizeof(b));
    b.id = 42;

    printf("\n[2] 传值改不动外面:\n");
    by_value(b);
    printf("    by_value 之后 b.id    = %d  ← 没变\n", b.id);

    printf("\n[3] 传指针能改:\n");
    by_pointer(&b);
    printf("    by_pointer 之后 b.id  = %d  ← 变了\n", b.id);

    printf("\n[4] 只读就用 const 指针（既省拷贝，又保证不被改）:\n");
    by_const_pointer(&b);
    printf("    by_const_pointer(&b) 编译通过；如果函数里写 b->id = 1 会直接编译报错\n");

    printf("\n[5] 返回结构体:\n");
    struct Big made = make_big(7);
    printf("    make_big(7).id = %d\n", made.id);
    printf("    返回值通常被编译器优化成「直接在调用者栈上构造」，不会真的拷两遍\n");
    printf("    （这叫 RVO/复制消除，C++17 起部分场景是标准强制的；未实测汇编，仅说明）\n");

    printf("\n[6] 什么时候可以放心传值:\n");
    printf("    - 结构体很小（比如 2 个 int，8 字节），和传指针开销差不多\n");
    printf("    - 函数本来就需要一份自己的副本（要在里面改而不影响调用者）\n");
    printf("    其余情况一律 const 指针/const 引用\n");
    return 0;
}
