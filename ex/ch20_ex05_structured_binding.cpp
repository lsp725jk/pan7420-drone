// ch20_ex05_structured_binding.cpp —— 结构化绑定（structured bindings）
// 编译: check.bat ch20_ex05_structured_binding.cpp c++latest
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <array>

struct ImuSample {
    int   accelX;
    int   accelY;
    float gyroZ;
    std::string tag;
};

std::tuple<int, double, std::string> makeTuple()
{
    return {7, 2.5, "tuple 里的字符串"};
}

// 返回"成功与否 + 值"，这是结构化绑定的经典场景
std::pair<bool, int> tryParse(const std::string& s)
{
    if (s.empty()) return {false, 0};
    return {true, static_cast<int>(s.size())};
}

int main()
{
    std::cout << "=== A. 绑定结构体的成员 ===\n";
    ImuSample s{100, -200, 0.5f, "imu0"};
    auto [ax, ay, gz, tag] = s;          // 拷贝一份（注意 auto 默认是值语义）
    std::cout << "  ax=" << ax << " ay=" << ay << " gz=" << gz << " tag=" << tag << "\n";
    ax = 999;                            // 改的是副本，不影响原对象
    std::cout << "  改了 ax 之后: 副本 ax=" << ax << ", 原对象 s.accelX=" << s.accelX << "\n";
    std::cout << "  想要可写就用引用：auto& [bx, by, bz, btag] = s; bx = 555;\n";
    auto& [bx, by, bz, btag] = s;
    bx = 555;
    std::cout << "  现在 s.accelX = " << s.accelX << "\n";

    std::cout << "\n=== B. 绑定 tuple / pair ===\n";
    auto [i, d, str] = makeTuple();
    std::cout << "  tuple: " << i << ", " << d << ", " << str << "\n";
    if (auto [ok, len] = tryParse("hello"); ok) {
        std::cout << "  tryParse(\"hello\") -> 成功，长度 " << len << "\n";
    }
    if (auto [ok, len] = tryParse(""); !ok) {
        std::cout << "  tryParse(\"\")     -> 失败（C++17 起 if 里也能声明结构化绑定，\n"
                     "                        但注意这里 ok/len 只在 if 语句内有效）\n";
    }

    std::cout << "\n=== C. 遍历 map：最常见的用途 ===\n";
    std::map<std::string, int> ages{{"alice", 30}, {"bob", 25}, {"carol", 35}};
    for (const auto& [name, age] : ages) {
        std::cout << "  " << name << " -> " << age << "\n";
    }
    std::cout << "  比 it->first / it->second 清楚太多了。\n";
    std::cout << "  想改值：for (auto& [name, age] : ages) age += 1;\n";

    std::cout << "\n=== D. 绑定 array / 数组成员数量必须吻合 ===\n";
    std::array<int, 3> arr{10, 20, 30};
    auto [x, y, z] = arr;
    std::cout << "  array: " << x << " " << y << " " << z << "\n";
    std::cout << "  C 数组也能绑：int raw[2] = {1,2}; auto [p, q] = raw;\n";
    int raw[2] = {1, 2};
    auto [p, q] = raw;
    std::cout << "  raw 数组: " << p << " " << q << "\n";

    std::cout << "\n=== E. 坑合集 ===\n";
    std::cout << "  坑 1：auto [a, b] = v; 是**拷贝**。想改原对象必须写 auto& [a, b] = v;\n";
    std::cout << "        对 map 的 value 做修改时忘了 & 是极常见的 bug。\n";
    std::cout << "  坑 2：C++17 里结构化绑定**不能**被 lambda 捕获（C++20 才行）。\n";
    std::cout << "        C++17 下要写 [ax = ax] 这样逐个捕获。\n";
    std::cout << "  坑 3：结构化绑定不是普通变量，不能取地址、不能当返回值转发（C++17）。\n";
    std::cout << "  坑 4：数量必须完全对上，多一个少一个都是编译错误。\n";
    std::cout << "  坑 5：绑定一个返回引用的函数时，注意悬垂\n";
    std::cout << "        （auto [a,b] = f() 里如果 f 返回临时对象的引用就完蛋）。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
