// ch13_ex15.cpp —— 四种类型转换对比（2）：dynamic_cast 与 RTTI
// 编译：check.bat ch13_ex15.cpp c++latest

// dynamic_cast 在运行期检查真实类型（依赖 RTTI，Run-Time Type Information，
// 运行期类型信息），失败时：指针返回 nullptr，引用抛 std::bad_cast。
#include <cstdio>
#include <typeinfo>

class Base {
public:
    virtual ~Base() = default;                 // 有虚函数才有 RTTI
};
class DerivedA : public Base { public: int a = 1; };
class DerivedB : public Base { public: int b = 2; };

// 1) 用 dynamic_cast 做"安全的向下转型 + 失败判断"
void handle(Base* p)
{
    if (DerivedA* pa = dynamic_cast<DerivedA*>(p)) {          // C++11 起的写法
        std::printf("   这是一个 DerivedA, a=%d\n", pa->a);
    } else if (DerivedB* pb = dynamic_cast<DerivedB*>(p)) {
        std::printf("   这是一个 DerivedB, b=%d\n", pb->b);
    } else {
        std::printf("   既不是 A 也不是 B（可能是纯 Base）\n");
    }
}

// 2) 引用版本：失败抛异常
void handle_ref(Base& r)
{
    try {
        DerivedA& ra = dynamic_cast<DerivedA&>(r);
        std::printf("   引用转换成功, a=%d\n", ra.a);
    } catch (const std::bad_cast& e) {
        std::printf("   引用转换失败: %s\n", e.what());
        std::printf("   （异常语法详见第 18 章）\n");
    }
}

int main()
{
    DerivedA a;
    DerivedB b;
    Base     base;

    std::printf("1) 指针版 dynamic_cast：\n");
    handle(&a);
    handle(&b);
    handle(&base);

    std::printf("2) 引用版 dynamic_cast（失败抛 std::bad_cast）：\n");
    handle_ref(a);
    handle_ref(base);

        // 3) typeid：查询真实类型名（也是 RTTI 的一部分）
    Base* p = &a;
    std::printf("3) typeid(*p).name() = %s\n", typeid(*p).name());
    std::printf("   typeid 去掉了引用/顶层 const；对多态类型返回动态类型\n");

        // 4) 代价与建议
    std::printf("4) dynamic_cast 的代价：运行期查表比较，比 static_cast 慢得多\n");
    std::printf("   建议：能用虚函数解决就不要 dynamic_cast（多态才是本意）\n");
    std::printf("   需要 dynamic_cast 才能干活，往往说明接口设计得不够好\n");
    return 0;
}
