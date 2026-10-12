// ch11_ex17.cpp —— nullptr：C 的 NULL 退休了
// 编译：check.bat ch11_ex17.cpp c++latest
#include <cstddef>
#include <cstdio>

// 重载：一个收整数，一个收指针
void pick(int)         { std::printf("  调用了 pick(int)\n"); }
void pick(void*)       { std::printf("  调用了 pick(void*)\n"); }

int main()
{
        // ---- 1) NULL 到底是什么 ----
    std::printf("1) sizeof(NULL)=%llu, sizeof(nullptr)=%llu\n",
                (unsigned long long)sizeof(NULL), (unsigned long long)sizeof(nullptr));
    std::printf("   NULL 在 C++ 里通常就是整数 0，所以它有类型歧义\n");

        // ---- 2) 歧义现场：传 NULL 会选错/编不过 ----
        // pick(NULL);            // 在 MSVC 上会选 pick(int)（因为 NULL 就是 0）
    pick(nullptr);            // 明确调用 pick(void*)
    pick(0);                  // 明确调用 pick(int)

        // ---- 3) nullptr 的类型是 std::nullptr_t，能隐式转成任何指针类型 ----
    int*  pi = nullptr;
    char* pc = nullptr;
    void (*fp)() = nullptr;
    std::printf("3) int*=%p char*=%p 函数指针=%p（都是空）\n",
                (void*)pi, (void*)pc, (void*)fp);

        // ---- 4) 和 C 的写法对照：判空、比较 ----
    if (pi == nullptr) std::printf("4) pi 是空指针（写 !pi 或 pi == nullptr 都行）\n");
        // if (pi == 0)      ...  也能编过，但语义不如 nullptr 清楚
        // delete/free 之后置空的习惯写法：pi = nullptr;

        // ---- 5) 常见坑：解引用空指针 ----
        // std::printf("%d", *pi);   // 能编过，运行时崩溃（访问冲突）
    std::printf("5) 空指针解引用不会在编译期报错，只会在运行时崩\n");

    return 0;
}
