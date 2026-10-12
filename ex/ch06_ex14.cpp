// ch06_ex14.cpp —— 智能指针一句话预告：现代 C++ 怎么把上面这些坑都堵住
// 编译：check.bat ch06_ex14.cpp c++latest
// 说明：本章只做"对照展示"，unique_ptr/shared_ptr 的细节留到 C++ 章节。
#include <cstdio>
#include <memory>
#include <cstring>

struct Msg {
    char text[16];
    Msg(const char* s) { strncpy(text, s, sizeof(text) - 1); text[sizeof(text) - 1] = '\0'; }
    ~Msg() { printf("    ~Msg() 被调用（自动释放）\n"); }
};

// 手写 malloc/free 的版本：异常/提前 return 就会漏
void manual_way()
{
    printf("  [手动管理]\n");
    Msg* m = (Msg*)malloc(sizeof(Msg));
    memset(m, 0, sizeof(Msg));
    strncpy(m->text, "hi", sizeof(m->text) - 1);
    printf("    内容: %s\n", m->text);
    free(m);
    printf("    手动 free 完成\n");
}

// unique_ptr 版本：离开作用域自动释放，不用写 free
void raii_way()
{
    printf("  [unique_ptr]\n");
    std::unique_ptr<Msg> m(new Msg("hi"));
    printf("    内容: %s\n", m->text ? m->text : "?");
    printf("    函数即将返回，谁都不用写 delete\n");
        // 这里没有 delete，也没有 free —— 出作用域时自动调用 ~Msg()
}

int main()
{
    printf("[1] 同一件事，两种写法：\n");
    manual_way();
    raii_way();

    printf("\n[2] 对照表（本章的坑 vs C++ 的解法）:\n");
    printf("    忘记 free               → unique_ptr/shared_ptr 析构时自动释放\n");
    printf("    悬垂指针（use-after-free）→ 出作用域后指针就不存在了；weak_ptr 能探测失效\n");
    printf("    重复释放（double free）  → unique_ptr 不可拷贝，天然不会有两个持有者\n");
    printf("    容量算错、越界写         → std::vector 带 size()，at() 会查边界\n");
    printf("    忘记置空、野指针         → 智能指针默认就是空，且能直接判 if (p)\n");

    printf("\n[3] 但在单片机上：\n");
    printf("    unique_ptr/vector 会引入堆分配和异常/模板开销，PY32F030 这种 64KB Flash、\n");
    printf("    8KB RAM 的芯片上通常直接禁用堆（不链接 malloc），全用静态数组 + 自己管理的池。\n");
    printf("    智能指针的完整讲解放在 C++ 章节。\n");

    printf("\n[4] 一个能立刻用上的折中：自己写「申请-使用-释放」三段式函数，\n");
    printf("    中间所有退出路径都 goto cleanup，就等价于有了 RAII。\n");
    return 0;
}
