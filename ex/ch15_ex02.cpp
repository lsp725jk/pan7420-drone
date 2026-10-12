// ch15_ex02.cpp —— 值类别的完整分类：lvalue / xvalue / prvalue，以及 glvalue / rvalue
#include <cstdio>
#include <string>
#include <utility>

// 三个重载，分别对应"左值"和"右值"两种绑定，用 const 区分出更多信息
void kind(int&)              { std::printf("  int&        （非 const 左值）\n"); }
void kind(const int&)        { std::printf("  const int&  （const 左值 或 右值都能绑到 const&）\n"); }
void kind(int&&)             { std::printf("  int&&       （右值，优先匹配右值引用）\n"); }

struct S {
    int v;
};

S   make_S()  { return S{7}; }          // prvalue
S&  get_S(S& s) { return s; }           // lvalue

int main()
{
    std::printf("--- 1) 三种值类别 ---\n");
    int n = 5;
    std::printf("  n            -> "); kind(n);                    // lvalue
    std::printf("  std::move(n) -> "); kind(std::move(n));          // xvalue（将亡值）
    std::printf("  5            -> "); kind(5);                     // prvalue
    std::printf("  n + 1        -> "); kind(n + 1);                 // prvalue

    std::printf("--- 2) 成员访问保持类别 ---\n");
    S s{3};
    std::printf("  s.v          -> "); kind(s.v);                  // s 是左值，s.v 也是左值
    std::printf("  make_S().v   -> "); kind(make_S().v);           // make_S() 是 prvalue，成员访问得到 xvalue

    std::printf("--- 3) std::move 把左值变成将亡值（xvalue）---\n");
    std::string str = "hello";
    std::printf("  移动前 str = \"%s\"，长度 %zu\n", str.c_str(), str.size());
    std::string stolen = std::move(str);
    std::printf("  移动后 stolen = \"%s\"\n", stolen.c_str());
    std::printf("  移动后 str  = \"%s\"，长度 %zu（标准只保证\"有效但未指定\"）\n",
                str.c_str(), str.size());
    str = "恢复";                       // 移到过的对象可以重新赋值
    std::printf("  重新赋值后 str = \"%s\"\n", str.c_str());

    std::printf("--- 4) const 对象不能被移动 ---\n");
    const std::string cstr = "constant";
        // std::string t2 = std::move(cstr);   // 能编译，但实际调用的是**拷贝构造**
    std::string t2 = std::move(cstr);
    std::printf("  std::move(const string) 复制出来的是 \"%s\"\n", t2.c_str());
    std::printf("  原对象还在：\"%s\"（因为 const 阻止了移动，退化成拷贝）\n", cstr.c_str());

    std::printf("--- 5) 一个函数的返回值到底是什么类别 ---\n");
    S s2{9};
    std::printf("  get_S(s2).v  -> "); kind(get_S(s2).v);         // 返回 S& -> 左值
    std::printf("  make_S().v   -> "); kind(make_S().v);          // 返回 S  -> xvalue

    return 0;
}
