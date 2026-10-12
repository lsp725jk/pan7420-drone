// ch20_ex06_string_view_span.cpp —— std::string_view 与 std::span：不拷贝的"视图"
// 编译: check.bat ch20_ex06_string_view_span.cpp c++latest
#include <iostream>
#include <string_view>
#include <span>
#include <string>
#include <vector>
#include <array>
#include <cstring>
#include <cstddef>

// ============================================================
// A. string_view：只借字符串，不拥有
// ============================================================
std::size_t countChar(std::string_view sv, char c)     // 参数按值传，很便宜（指针+长度）
{
    std::size_t n = 0;
    for (char ch : sv) if (ch == c) ++n;
    return n;
}

// 对比：老式 C 接口 + strlen，遇到"非空结尾的子串"就没办法
std::size_t countCharC(const char* s, char c)
{
    std::size_t n = 0;
    for (const char* p = s; *p; ++p) if (*p == c) ++n;
    return n;
}

// ============================================================
// B. span：连续内存的视图（数组/vector/array 都能接）
// ============================================================
int sumSpan(std::span<const int> data)                 // 不知道来源，只知道是连续 int
{
    int s = 0;
    for (int v : data) s += v;
    return s;
}

void doubleInPlace(std::span<int> data)                // 可写视图
{
    for (int& v : data) v *= 2;
}

int main()
{
    std::cout << "=== A. string_view 最实用的地方：子串零拷贝 ===\n";
    std::string big = "ATCG-ATCG-ATCG-ATCG";
    std::string_view sv(big.data() + 5, 4);            // 指向 big 中间 4 个字符，没有拷贝
    std::cout << "  big          = " << big << "\n";
    std::cout << "  sv (中间4个) = " << sv << "\n";
    std::cout << "  用 C 接口做不到这一点（中间没 '\\0'）。\n";
    std::cout << "  countChar(sv, 'A') = " << countChar(sv, 'A') << "\n";
    std::cout << "  直接传字符串字面量也可以，不会构造 std::string：\n";
    std::cout << "  countChar(\"aaa\", 'a') = " << countChar("aaa", 'a') << "\n";
    std::cout << "  countCharC(\"aaa\", 'a') = " << countCharC("aaa", 'a') << "（老写法也行，\n"
                 "     但拿不到\"长度已知的非空结尾子串\"）\n";

    std::cout << "\n=== B. string_view 的坑：它不拥有数据 ===\n";
    {
        std::string_view dangling;
        {
            std::string temp = "我是临时字符串";
            dangling = temp;                       // 只是记下指针和长度
        }                                          // temp 在这里销毁
        std::cout << "  危险！temp 已销毁，下面读的是释放后的内存（UB）：\n";
        std::cout << "  （本程序故意不打印它的内容，避免读到脏数据骗人）\n";
        std::cout << "  dangling.size() 仍然等于 " << dangling.size()
                  << " —— 长度还在，但数据没了。这就是 string_view 最危险的坑。\n";
        std::cout << "  规则：string_view 只能指向\"生命周期比你长\"的字符串；\n";
        std::cout << "        不要从临时 std::string 构造 string_view 并保存下来。\n";
    }

    std::cout << "\n=== C. span：一个函数接三种来源 ===\n";
    int carr[4] = {1, 2, 3, 4};
    std::vector<int> vec{10, 20, 30};
    std::array<int, 3> stdarr{100, 200, 300};
    std::cout << "  C 数组  sum = " << sumSpan(carr) << "\n";
    std::cout << "  vector  sum = " << sumSpan(vec) << "\n";
    std::cout << "  array   sum = " << sumSpan(stdarr) << "\n";
    std::cout << "  一个函数搞定三种来源，而且**不拷贝**。\n";
    std::cout << "  span 知道自己的长度，所以不需要再传一个 size 参数。\n";

    std::cout << "\n=== D. span 是可写视图时能改原数据 ===\n";
    std::vector<int> data{1, 2, 3};
    std::cout << "  改之前: ";
    for (int v : data) std::cout << v << " ";
    std::cout << "\n";
    doubleInPlace(data);
    std::cout << "  改之后: ";
    for (int v : data) std::cout << v << " ";
    std::cout << "   <- 改的是原 vector\n";

    std::cout << "\n=== E. 静态长度 vs 动态长度 ===\n";
    std::span<int>                 dyn{carr};       // 动态长度
    std::span<int, 4>              stat{carr};      // 编译期长度（必须正好 4）
    std::cout << "  span<int>     .size() = " << dyn.size()  << "\n";
    std::cout << "  span<int,4>   .size() = " << stat.size() << "（编译期常量）\n";
    std::cout << "  span<int,4> 传错长度会编译失败 —— 又一处\"把错误提前\"。\n";
    std::cout << "  .first(n) / .last(n) / .subspan(off, n) 都能取子视图，零拷贝。\n";
    auto sub = std::span<int>(carr).subspan(1, 2);
    std::cout << "  carr.subspan(1,2) = { ";
    for (int v : sub) std::cout << v << " ";
    std::cout << "}\n";

    std::cout << "\n=== F. 嵌入式视角（PWM/ADC 缓冲区的标准写法）===\n";
    std::cout << "  void fillPwm(std::span<uint16_t> out);   // 调用者给多大就填多大\n";
    std::cout << "  好处：不用再写 (uint16_t* buf, size_t len) 这种容易传错的两个参数；\n";
    std::cout << "        span 本身只有\"指针 + 长度\"，和手写两个参数一样大，零开销。\n";
    std::cout << "  string_view 在单片机上要慎用：它依赖 std::string，\n";
    std::cout << "        裸机上通常只用 const char* 或 string_view 指向字面量。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
