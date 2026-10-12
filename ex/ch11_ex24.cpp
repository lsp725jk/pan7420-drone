// ch11_ex24.cpp —— 结构化绑定：一次拆开多个值
// 编译：check.bat ch11_ex24.cpp c++latest
// 结构化绑定是 C++17 起的特性。
#include <cstdio>
#include <tuple>

struct ImuSample {          // 聚合类型也能拆
    int    id;
    double ax, ay, az;
};

std::pair<int, double> read_once() { return { 7, 1.25 }; }
std::tuple<int, double, char> read_three() { return { 1, 2.5, 'x' }; }

int main()
{
        // 1) 拆 pair：不用 .first / .second
    auto [id, v] = read_once();
    std::printf("1) id=%d, v=%.2f\n", id, v);

        // 2) 拆 tuple：个数要对得上
    auto [a, b, c] = read_three();
    std::printf("2) a=%d, b=%.1f, c=%c\n", a, b, c);

        // 3) 拆结构体
    ImuSample s{ 3, 0.1, 0.2, 9.8 };
    auto [sid, sax, say, saz] = s;
    std::printf("3) id=%d acc=(%.1f, %.1f, %.1f)\n", sid, sax, say, saz);

        // 4) 拆数组（C++ 起就能拆内置数组）
    int arr[3] = { 10, 20, 30 };
    auto [x, y, z] = arr;
    std::printf("4) x=%d y=%d z=%d\n", x, y, z);

        // 5) 用引用拆，才能改到原对象（auto& / const auto&）
    auto& [rid, rax, ray, raz] = s;
    rax = 9.9;
    std::printf("5) 引用拆解后改 rax: s.ax=%.1f\n", s.ax);

    const ImuSample cs{ 4, 1, 2, 3 };
    const auto& [cid, cax, cay, caz] = cs;   // 只读，不拷贝
    std::printf("   const auto& 拆解: id=%d ax=%.1f\n", cid, cax);

    (void)ray; (void)raz; (void)cay; (void)caz;
    return 0;
}
