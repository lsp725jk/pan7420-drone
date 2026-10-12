// ch06_ex13.cpp —— 野指针与"必须初始化"：不初始化的指针里是什么
// 编译：check.bat ch06_ex13.cpp c++latest
// 说明：本程序只在安全范围内演示；真正的解引用野指针会崩（见 ch05_ex6.cpp）。
#include <cstdio>
#include <cstdlib>
#include <cstring>

// 局部结构体不初始化：字段是栈上的垃圾
void show_uninitialized()
{
    struct Sensor { int id; double value; char name[8]; };
    struct Sensor s;                 // 没初始化
    printf("  未初始化的局部结构体: id=%d value=%f name=\"%s\"\n",
           s.id, s.value, s.name);
    printf("  ← 全是垃圾值（名字甚至可能没有 '\\0'，打印时读越界）\n");
}

int main()
{
    printf("[1] 三种「看起来没初始化」的指针，后果完全不同：\n");

        // (a) 全局/静态变量：编译器保证清零 → 就是 nullptr，安全
    static int* g_static_ptr;        // 静态存储期，自动清零
    printf("  (a) static int* p;         p = %p  ← 静态区自动清零，等于 nullptr\n",
           (void*)g_static_ptr);

        // (b) 局部变量：不清零 → 野指针
        // 先制造一些"垃圾"放在栈上，让效果明显（光声明未使用，MSVC 可能给个巧合的 0）
    volatile int junk1 = 0x11111111;
    volatile int junk2 = 0x22222222;
    int* local_ptr;                  // 没初始化
    (void)junk1; (void)junk2;
    printf("  (b) int* p;（局部）        p = %p  ← 栈上残留的垃圾值\n",
           (void*)local_ptr);
    printf("      这是「野指针」（wild pointer）：它指向一个随机地址，"
           "取值每次运行都可能不同\n");

        // (c) 显式初始化：安全
    int* safe_ptr = nullptr;
    printf("  (c) int* p = nullptr;      p = %p  ← 唯一正确的做法\n", (void*)safe_ptr);

    printf("\n[2] 未初始化结构体的内容：\n");
    show_uninitialized();

    printf("\n[3] 初始化到底该用什么：\n");
        // 指针 → nullptr
    int* p1 = nullptr;
        // 数值 → 0（大括号初始化更保险，能防窄化）
    int n1 = 0;
    int n2 = { 0 };
    int n3{};                        // C++11 起：值初始化，保证为 0
        // 数组 → 全零
    int arr1[8] = { 0 };             // 第一个元素给 0，其余自动补 0
    int arr2[8]{};                   // C++11：同样全 0
        // 结构体 → 全零（C 里习惯用 memset 或 = {0}）
    struct Cfg { int a; double b; char c[4]; };
    struct Cfg cfg1 = { 0 };         // C 和 C++ 都可用
    struct Cfg cfg2{};               // C++ 里更简洁

    printf("  int n1=0 → %d;  int n2={0} → %d;  int n3{} → %d\n", n1, n2, n3);
    printf("  arr1[7]=%d  arr2[7]=%d\n", arr1[7], arr2[7]);
    printf("  cfg1.a=%d cfg1.b=%f cfg1.c=\"%s\"\n", cfg1.a, cfg1.b, cfg1.c);
    printf("  cfg2.a=%d  （C++ 的 {} 值初始化同样全零）\n", cfg2.a);
    printf("  (void)p1=%p\n", (void*)p1);

    printf("\n[4] 编译器能帮你抓到的部分：\n");
    printf("  MSVC /W4 对「用了未初始化的局部变量」会给 C4700 警告\n");
    printf("  但如果你把指针传出去、或者通过函数返回值拿回来，编译器多半看不出来\n");
    printf("  所以「定义时就初始化」这个习惯，比依赖警告可靠得多\n");
    return 0;
}
