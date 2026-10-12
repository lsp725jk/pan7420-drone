// ch05_ex5.cpp —— NULL / nullptr / 0：空指针的三种写法
// 编译：check.bat ch05_ex5.cpp c++latest
#include <cstdio>
#include <cstddef>   // NULL 在这个头里

void take(int)      { printf("  调用了 take(int)\n"); }
void take(char*)    { printf("  调用了 take(char*)\n"); }

int main()
{
        // 三种写法都能表达"空指针"，但类型不同
    int* p1 = NULL;         // 在 C++ 里 NULL 通常就是 0（整数），类型是 long/int
    int* p2 = nullptr;      // C++11 起：真正的"空指针类型" std::nullptr_t
    int* p3 = 0;            // 老写法，C 里常用

    printf("p1=%p p2=%p p3=%p\n", (void*)p1, (void*)p2, (void*)p3);
    printf("p1==p2? %d   p2==p3? %d\n", p1 == p2, p2 == p3);
    printf("nullptr 自己的大小: sizeof(nullptr) = %zu\n", sizeof(nullptr));

    printf("\n重载解析（这是 nullptr 存在的最大理由）:\n");
        // take(NULL);   // <- 打开这行：编译能过，但调的是 take(int)，很可能不是你要的
    take(nullptr);   // 明确选 take(char*)
    take(0);         // 明确选 take(int)

    printf("\n空指针不要解引用（下面第 3 种写法会崩，已注释）:\n");
    if (p2 == nullptr) printf("  解引用前先判断 p2 == nullptr 才是安全写法\n");
        // printf("%d\n", *p2);       // 运行期访问 0 号地址 -> 崩溃

        // 条件里直接用指针：非空为真
    printf("if (p3) 的结果: %s\n", p3 ? "真" : "假");
    return 0;
}
