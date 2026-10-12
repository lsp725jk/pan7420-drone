// ch18_ex07_ctor_throw.cpp —— 构造函数抛异常会怎样（对象、成员、基类各是什么命运）
// 编译: check.bat ch18_ex07_ctor_throw.cpp c++latest
#include <iostream>
#include <stdexcept>
#include <string>

struct Member {                                  // 被当作成员的对象
    std::string name;
    explicit Member(std::string n) : name(std::move(n)) {
        std::cout << "    + 成员构造 " << name << "\n";
    }
    ~Member() { std::cout << "    - 成员析构 " << name << "\n"; }
};

struct Base {
    Base()  { std::cout << "    + 基类构造\n"; }
    ~Base() { std::cout << "    - 基类析构\n"; }
};

// 构造顺序：基类 -> 成员（按声明顺序）-> 自己的构造体
struct Widget : Base {
    Member m1;
    Member m2;

    Widget(int failAt)
        : m1("m1")
        , m2("m2")                                // 先都正常构造出来
    {
        std::cout << "    + Widget 构造体开始, failAt = " << failAt << "\n";
        if (failAt == 1) {
            throw std::runtime_error("Widget 构造体里抛异常");
        }
        std::cout << "    + Widget 构造体结束（对象正式诞生）\n";
    }

    ~Widget() { std::cout << "    - Widget 析构\n"; }
};

// 成员自己构造失败：Widget 的构造体根本没机会跑
struct Fragile {
    Member bad;
    Fragile(bool fail) : bad("bad-member") {
        if (fail) throw std::runtime_error("Fragile 构造体抛异常");
    }
    ~Fragile() { std::cout << "    - Fragile 析构\n"; }
};

struct MemberThrows {
    MemberThrows() { throw std::runtime_error("成员构造时就炸了"); }
    ~MemberThrows() { std::cout << "    - MemberThrows 析构\n"; }
};

struct Holder {
    Member ok;
    MemberThrows boom;               // 声明在后面，构造在后面
    Holder() : ok("ok-member") { std::cout << "    + Holder 构造体\n"; }
    ~Holder() { std::cout << "    - Holder 析构\n"; }
};

int main()
{
    std::cout << "=== A. 构造体里抛：对象没建成，析构函数不会被调用 ===\n";
    try {
        Widget w(1);
        std::cout << "  不会到这里\n";
    }
    catch (const std::exception& e) {
        std::cout << "  catch: " << e.what() << "\n";
        std::cout << "  观察上面：基类和 m1/m2 都析构了，只有 - Widget 析构 没出现\n";
    }

    std::cout << "\n=== B. 正常构造：析构顺序和构造顺序相反 ===\n";
    {
        Widget w(0);
        std::cout << "  （离开作用域）\n";
    }

    std::cout << "\n=== C. 成员构造失败：本类的构造体一行都没执行 ===\n";
    try {
        Holder h;
    }
    catch (const std::exception& e) {
        std::cout << "  catch: " << e.what() << "\n";
        std::cout << "  观察：ok-member 被析构了（已建成的成员要拆掉），"
                     "但 Holder 的构造体从未运行\n";
    }

    std::cout << "\n=== D. 结论表（对照上面输出看）===\n";
    std::cout <<
        "  构造阶段抛异常时：\n"
        "    已完成的基类子对象      -> 析构 ✔\n"
        "    已完成的成员子对象      -> 析构 ✔（逆序）\n"
        "    本类自己的析构函数      -> 不调用 ✘（对象从未诞生）\n"
        "    本类构造函数体里 new 出来的资源 -> 泄漏！必须靠成员 RAII 兜住\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
