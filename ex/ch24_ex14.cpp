// ch24_ex14.cpp —— 例 24-14：C++ Core Guidelines 的核心建议（配可运行的反例/正例）
// 官方地址：https://isocpp.github.io/CppCoreGuidelines
// 这里挑最常被引用的几条，每条给一个"坏代码 -> 好代码"的对照。
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// ==================================================================
// P.1 / P.4：直接用代码表达意图；不要用"聪明的"技巧
// ==================================================================
int pick(bool a) {
    return a ? 1 : 2;  // 意图一目了然
}

// ==================================================================
// ES.20 / ES.21：变量永远初始化；不要在声明前使用
// ==================================================================
int safe_div(int a, int b) {
    const int result = (b != 0) ? (a / b) : 0;  // 声明即初始化
    return result;
}

// ==================================================================
// I.11 / F.22：不要用裸指针表达所有权；用 unique_ptr / 容器
// ==================================================================
// 坏：Sensor* make();   // 谁负责 delete？不知道
// 好：
std::unique_ptr<std::string> make_name() {
    return std::make_unique<std::string>("IMU0");
}
// 不表达所有权时，用 T* 或 T& 是可以的（"观察者"语义）：
void observe(const std::string& s) {
    std::printf("    观察到 %s\n", s.c_str());
}

// ==================================================================
// R.1 / R.3 / R.5：RAII；裸 new/delete 尽量不写；容器优先
// ==================================================================
class Resource {
public:
    Resource() { std::printf("    [Resource] 申请\n"); }
    ~Resource() { std::printf("    [Resource] 释放\n"); }
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
};

// ==================================================================
// ES.5 / ES.11：作用域尽量小；用 auto 避免重复写类型名
// ==================================================================
int sum_of(const std::vector<int>& v) {
    int sum = 0;
    for (const auto& x : v) {  // 不重复写 int
        sum += x;
    }
    return sum;
}

// ==================================================================
// C.128 / C.132：虚函数写 override；接口不要加没必要的虚函数
// ==================================================================
struct Base {
    virtual ~Base() = default;
    virtual int value() const { return 1; }
};
struct Derived : Base {
    int value() const override { return 2; }  // 写 override，写错签名编译器会告诉你
};

// ==================================================================
// SL.con.1：优先用标准容器；下标优先用 .at() 而不是 []
// ==================================================================
void use_at(const std::vector<int>& v, std::size_t i) {
        // v[i] 越界是 UB；v.at(i) 越界抛异常
    try {
        std::printf("    v.at(%zu) = %d\n", i, v.at(i));
    } catch (const std::out_of_range&) {
        std::printf("    v.at(%zu) 越界，被抓住了\n", i);
    }
}

int main() {
    std::printf("P.1   pick(true)=%d pick(false)=%d\n", pick(true), pick(false));
    std::printf("ES.20 safe_div(10,3)=%d safe_div(10,0)=%d\n", safe_div(10, 3), safe_div(10, 0));

    std::printf("F.22  智能指针自动释放：\n");
    {
        auto name = make_name();
        observe(*name);
    }  // 自动释放
    std::printf("      已经出了作用域\n");

    std::printf("R.1   RAII：\n");
    {
        Resource r;
        std::printf("      用资源...\n");
    }

    std::printf("ES.11 范围 for + auto：sum=%d\n", sum_of({1, 2, 3, 4}));

    Base b;
    Derived d;
    Base* p = &d;
    std::printf("C.128 多态: b.value()=%d p->value()=%d\n", b.value(), p->value());

    std::printf("SL.con.1 边界检查：\n");
    use_at({10, 20, 30}, 1);
    use_at({10, 20, 30}, 5);

    std::printf("\n记住这几条就够日常用了：\n");
    std::printf("  1) 让每个资源都有个负责人（RAII / 智能指针 / 容器）\n");
    std::printf("  2) 每个变量声明时就初始化\n");
    std::printf("  3) 用类型表达意图（enum class、强类型、const）\n");
    std::printf("  4) 优先用标准库，别自己造容器/字符串/排序\n");
    std::printf("  5) 编译告警必须清零（/W4 或 -Wall -Wextra）\n");
    std::printf("  6) 虚函数写 override，接口要小\n");
    return 0;
}
