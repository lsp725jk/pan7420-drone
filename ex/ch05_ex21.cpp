// ch05_ex21.cpp —— 数组退化（decay）的完整对照表：什么时候退、什么时候不退
// 编译：check.bat ch05_ex21.cpp c++latest
#include <cstdio>
#include <cstring>

// 用模板把"数组类型"原样保留下来，用来证明"传进去就退化了"
template <typename T>
const char* type_of(const T&) { return "unknown"; }
template <typename T, size_t N>
const char* type_of(const T (&)[N]) { return "T[N]  —— 拿到了真正的数组类型"; }

// 用重载区分"数组引用"和"指针"：这里故意先只声明指针版本
void probe(const int* p)        { printf("      → 命中 const int* 版本，长度信息没了，sizeof=%zu\n", sizeof(p)); }

// 数组引用版本：和上面的 probe 是不同名字，方便对照
void probe_ref(const int (&arr)[5])
{
    printf("命中 const int(&)[5] 版本，长度信息还在，sizeof=%zu\n", sizeof(arr));
}



int main()
{
    int a[5] = { 1, 2, 3, 4, 5 };

    printf("[1] 不退化的场合（数组名 = 整个数组）:\n");
    printf("    sizeof(a)          = %zu   ← 运算符 sizeof\n", sizeof(a));
    printf("    &a 的类型          = int(*)[5]（指向数组的指针）\n");
    printf("    alignof(int[5])    = %zu   ← alignof 也可以用（注意 MSVC 对表达式要用 __alignof）\n",
           alignof(int[5]));
    printf("    用引用绑定 a: %s\n", type_of(a));
    printf("    C++ 的 std::begin/std::end、范围 for 也都保留数组信息\n");

    printf("\n[2] 会退化的场合（数组名 = 首元素地址）:\n");
    int* p = a;                                     // (a) 赋给指针
    printf("    (a) int* p = a;        p==&a[0]? %s\n", (p == &a[0]) ? "是" : "否");
    printf("    (b) 传给函数: ");
    probe(a);                                       // (b) 函数实参（两个重载只匹配指针那个）
    printf("    (c) a + 1 参与算术     值 = %p（等于 &a[1]）\n", (void*)(a + 1));
    printf("    (d) 下标 a[2]          值 = %d（等于 *(a+2)）\n", a[2]);
    printf("    (e) 比较 a == &a[0]    结果 = %s\n", (a == &a[0]) ? "真" : "假");
    printf("    (f) 三元运算 cond ? a : p 的类型是 int*\n");

        // 对照：如果改用"数组引用"形参，数组就不会退化、长度信息还在
    printf("    (g) 换成接收 const int(&)[5] 的版本: ");
    probe_ref(a);

    printf("\n[3] 关键差异：&a 和 a 数值相同、类型不同\n");
    printf("    a      = %p   a+1 跳 %td 字节\n", (void*)a, (char*)(a + 1) - (char*)a);
    printf("    &a     = %p   &a+1 跳 %td 字节\n", (void*)&a, (char*)(&a + 1) - (char*)&a);
    printf("    sizeof(a)  = %zu     sizeof(&a)  = %zu\n", sizeof(a), sizeof(&a));

    printf("\n[4] 为什么 C 要这么设计（历史原因）:\n");
    printf("    C 想把「数组名」和「指针」统一处理，这样 a[i] 只有一套规则。\n");
    printf("    代价是：数组长度信息在传参时丢失，所以必须额外传一个长度参数。\n");
    printf("    C++ 的补救办法：\n");
    printf("      - 用引用收数组（模板 + (const T(&)[N])），长度不丢\n");
    printf("      - 用 std::array<T,N> 或 std::vector<T>，把长度装进类型/对象里\n");
    printf("      - C++20 的 std::span<T>：既接受数组又接受指针+长度\n");

    printf("\n[5] 一句话总结:\n");
    printf("    「数组名不是指针」——它是数组类型的对象；\n");
    printf("    「大多数场合会退化成指针」——只要用在需要指针的地方，就隐式取 &a[0]，\n");
    printf("      同时丢掉长度信息。sizeof / & / 引用绑定 / alignof 是几个例外。\n");
    return 0;
}
