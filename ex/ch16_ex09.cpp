// ch16_ex09.cpp —— using 别名：给模板类型起短名字
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

// ---------------- 1) 传统 typedef 的毛病：不能直接给模板起别名 ----------------
typedef std::map<std::string, int> ScoreMap;        // 具体类型：可以
// typedef std::vector Vec;                         // 错误：vector 需要模板参数
typedef std::vector<std::pair<std::string, int>> PairVec;   // 具体类型：可以，但很长

// ---------------- 2) using 别名模板（alias template）：给"模板"起别名 ----------------
template <class T>
using Vec = std::vector<T>;                         // 语法干净

template <class K, class V>
using Map = std::map<K, V>;

template <class T>
using Matrix = std::vector<std::vector<T>>;         // 嵌套模板也能起名

// ---------------- 3) 别名模板可以带默认参数（typedef 做不到这个） ----------------
template <class T, class Alloc = std::allocator<T>>
using MyVec = std::vector<T, Alloc>;

// ---------------- 4) 别名 vs 新类型：using 只是"改名字"，不是"造新类型" ----------------
template <class T>
using VecAlias = std::vector<T>;

template <class T>
struct VecStruct : std::vector<T> {};               // 这才是新类型

// ---------------- 5) 用别名模板简化函数签名 ----------------
template <class T>
using Table = std::vector<std::pair<std::string, T>>;

template <class T>
void print_table(const Table<T>& t)
{
    for (const auto& kv : t) {
        std::printf("    %-8s = %s\n", kv.first.c_str(), std::to_string(kv.second).c_str());
    }
}

// ---------------- 6) 别名模板 + 类型特性：给"分数"造一个类型 ----------------
template <class Rep>
using Scaled = std::pair<Rep, Rep>;                 // (分子, 分母)

template <class Rep>
Scaled<Rep> make_scaled(Rep num, Rep den) { return { num, den }; }

int main()
{
    std::printf("--- 1) typedef 对比 using：可读性 ---\n");
    {
        PairVec pv;
        pv.push_back({ "apple", 3 });
        std::printf("    PairVec 的元素：%s = %d\n", pv[0].first.c_str(), pv[0].second);
        std::printf("    PairVec 的本质：%s\n", typeid(PairVec).name());
    }

    std::printf("--- 2) using 别名模板：给模板起名字 ---\n");
    {
        Vec<int>         vi;
        Vec<std::string> vs;
        Map<std::string, int> m;
        vi.push_back(1);
        vs.push_back("hello");
        m["a"] = 1;
        std::printf("    Vec<int> 大小 = %zu\n", vi.size());
        std::printf("    Vec<std::string> 大小 = %zu\n", vs.size());
        std::printf("    Map<string,int> 大小 = %zu\n", m.size());
    }

    std::printf("--- 3) 嵌套模板：Matrix<T> ---\n");
    {
        Matrix<int> mat(2, std::vector<int>(3, 0));   // 2 行 3 列
        mat[1][1] = 7;
        std::printf("    Matrix<int> 有 %zu 行，第 1 行有 %zu 列\n",
                    mat.size(), mat[0].size());
        std::printf("    mat[1][1] = %d\n", mat[1][1]);
        std::printf("    如果不写别名，声明要写成：\n");
        std::printf("      std::vector<std::vector<int>> mat(2, std::vector<int>(3, 0));\n");
    }

    std::printf("--- 4) 别名模板带默认参数 ---\n");
    {
        MyVec<int> v;
        v.push_back(42);
        std::printf("    MyVec<int> 就是 vector<int, allocator<int>>\n");
        std::printf("    is_same<MyVec<int>, std::vector<int>> = %d\n",
                    std::is_same_v<MyVec<int>, std::vector<int>>);
    }

    std::printf("--- 5) 关键区别：using 只是别名，不是新类型 ---\n");
    {
        std::printf("    is_same<VecAlias<int>, std::vector<int>>   = %d（同一个类型）\n",
                    std::is_same_v<VecAlias<int>, std::vector<int>>);
        std::printf("    is_same<VecStruct<int>, std::vector<int>>  = %d（不同类型）\n",
                    std::is_same_v<VecStruct<int>, std::vector<int>>);
        std::printf("    想做\"强类型\"（比如 Meters / Seconds 不混用），要写 struct 包装\n");
    }

    std::printf("--- 6) 别名模板让函数签名短很多 ---\n");
    {
        Table<int> scores;
        scores.push_back({ "alice", 90 });
        scores.push_back({ "bob", 85 });
        print_table(scores);
    }

    std::printf("--- 7) 别名模板的另一个常见用法：给\"依赖类型\"起名字 ---\n");
    {
        std::printf("    标准库里到处是这种写法：\n");
        std::printf("      template<class T> using value_type = typename T::value_type;\n");
        std::printf("    这样在模板里就不用反复写 typename T::value_type 了\n");
        std::printf("    本机验证：std::vector<int>::value_type 就是 int -> %d\n",
                    std::is_same_v<std::vector<int>::value_type, int>);
    }

    return 0;
}
