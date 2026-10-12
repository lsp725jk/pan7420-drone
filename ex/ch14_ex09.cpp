// ch14_ex09.cpp —— 自定义删除器（deleter）：让智能指针管 FILE*、malloc 内存、带状态的资源
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

int main()
{
    std::printf("--- 1) 用 unique_ptr 管 FILE*（删除器 = fclose）---\n");
    {
                // decltype(&std::fclose) 就是 int(*)(FILE*)，作为模板第二个参数
        std::unique_ptr<FILE, decltype(&std::fclose)> fp(std::fopen("ch14_ex09_tmp.txt", "w"),
                                                         &std::fclose);
        if (!fp) {
            std::printf("  打开文件失败\n");
            return 1;
        }
        std::fprintf(fp.get(), "写一行字\n");
        std::printf("  写文件完成，即将离开作用域（会自动 fclose）\n");
    }
    std::printf("  作用域结束，文件已自动关闭\n");

    {   // 把文件读回来，证明真的写进去了、也真的关掉了
        std::unique_ptr<FILE, decltype(&std::fclose)> fp(std::fopen("ch14_ex09_tmp.txt", "r"),
                                                         &std::fclose);
        char line[64] = {0};
        if (fp.get() != nullptr) {
            char* got = std::fgets(line, sizeof(line), fp.get());
            if (got != nullptr) {
                std::printf("  读回内容: %s", line);
            }
        }
    }
    std::remove("ch14_ex09_tmp.txt");

    std::printf("--- 2) 用 unique_ptr 管 malloc 出来的内存（删除器 = free）---\n");
    {
        std::unique_ptr<char, decltype(&std::free)> buf(
            static_cast<char*>(std::malloc(32)), &std::free);
        if (buf) {
            std::snprintf(buf.get(), 32, "%s", "malloc 的内存");
            std::printf("  buf = %s\n", buf.get());
        }
        std::printf("  作用域结束，自动 free\n");
    }

    std::printf("--- 3) 需要记住事情的删除器：用 lambda 捕获 ---\n");
    {
        int close_count = 0;
                // lambda 有捕获时类型写不出来，用 auto 接住，再把它作为删除器传进去
        auto closer = [&close_count](FILE* f) {
            if (f != nullptr) {
                std::fclose(f);
                ++close_count;
                std::printf("  [删除器被调用] close_count 现在 = %d\n", close_count);
            }
        };
        {
            std::unique_ptr<FILE, decltype(closer)> fp(
                std::fopen("ch14_ex09_tmp2.txt", "w"), closer);
            std::fprintf(fp.get(), "x\n");
            std::printf("  文件已打开\n");
        }
        std::printf("  离开作用域后 close_count = %d（删除器捕获了它）\n", close_count);
        std::remove("ch14_ex09_tmp2.txt");
    }

    std::printf("--- 4) shared_ptr 的删除器：写在构造函数里，不改变类型 ---\n");
    {
                // 对比：unique_ptr 的删除器是类型的一部分，shared_ptr 的不是
        std::shared_ptr<FILE> fp(std::fopen("ch14_ex09_tmp3.txt", "w"), &std::fclose);
        std::shared_ptr<FILE> fp2(nullptr, [](FILE*) {                 /* 什么都不做 */ });
        std::printf("  sizeof(fp)  = %zu\n", sizeof(fp));
        std::printf("  sizeof(fp2) = %zu  —— 两者类型相同，都是 shared_ptr<FILE>\n", sizeof(fp2));
        std::printf("  unique_ptr 带 fclose 删除器时 sizeof = %zu（多存一个函数指针）\n",
                    sizeof(std::unique_ptr<FILE, decltype(&std::fclose)>));
        std::fprintf(fp.get(), "y\n");
        std::remove("ch14_ex09_tmp3.txt");
    }

    std::printf("main 结束\n");
    return 0;
}
