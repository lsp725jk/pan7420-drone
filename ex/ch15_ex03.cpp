// ch15_ex03.cpp —— 右值引用 && 的绑定规则与它最常见的误用（返回局部变量的引用）
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <string>
#include <utility>

int main()
{
    std::printf("--- 1) 右值引用只能绑右值 ---\n");
    int  n = 1;
    int&& r1 = 2;              // OK：绑到纯右值
    int&& r2 = std::move(n);   // OK：绑到将亡值
        // int&& r3 = n;           // 编译错误：无法将右值引用绑定到左值
    std::printf("  r1 = %d, r2 = %d, n 现在 = %d\n", r1, r2, n);
    std::printf("  注意 r2 绑的是 n 本身，不是 n 的副本\n");
    r2 = 100;
    std::printf("  通过 r2 改值之后 n = %d（同一个对象）\n", n);

    std::printf("--- 2) const 左值引用什么都能绑（这是 C++98 的老规矩）---\n");
    const int& c1 = n;         // 绑左值
    const int& c2 = 3;         // 绑右值
    const int& c3 = n + 1;     // 绑表达式结果
    std::printf("  c1 = %d, c2 = %d, c3 = %d\n", c1, c2, c3);
    std::printf("  所以 const T& 参数可以接任何东西 —— 这就是为什么老代码里到处是 const T&\n");

    std::printf("--- 3) const 右值引用（几乎没用，但要知道它存在）---\n");
    const int&& cr = 5;
    std::printf("  const int&& cr = %d，不能被修改，也不能被移动\n", cr);
    std::printf("  std::is_same_v<decltype(cr), const int&&> = %d\n",
                std::is_same_v<decltype(cr), const int&&>);

    std::printf("--- 4) 延长生命期：右值引用绑定的临时对象会被延长 ---\n");
    {
        const std::string& sref = std::string("临时字符串");
        std::printf("  const string& 绑临时对象，生命期延长到 sref 作用域结束：\"%s\"\n",
                    sref.c_str());
    }

    std::printf("--- 5) 经典坑：返回局部对象的右值引用 ---\n");
    std::printf("  下面这段是错误示范（注释掉，因为它是 UB）：\n");
    std::printf("    std::string&& bad() {{\n");
    std::printf("        std::string local = \"local\";\n");
    std::printf("        return std::move(local);   // 返回的是悬空引用！\n");
    std::printf("    }}\n");
    std::printf("  local 在函数返回时已析构，调用者拿到的是悬空引用 —— UB\n");
    std::printf("  编译器通常只给 warning C4172: 返回局部变量或临时量的地址\n");

    std::printf("--- 6) 正确做法：返回对象本身（值），靠 RVO/移动 ---\n");
    std::printf("  std::string good() {{ std::string local = \"local\"; return local; }}\n");
    std::printf("  返回类型是 string（值），编译器会做 RVO 或者至少用移动 —— 见例 15-10\n");

    return 0;
}
