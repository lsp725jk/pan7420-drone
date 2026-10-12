// ch13_ex14.cpp —— 四种类型转换对比（1）：static_cast 与隐式转换
// 编译：check.bat ch13_ex14.cpp c++latest
#include <cstdio>

class Base {
public:
    virtual ~Base() = default;
    int b = 1;
};
class Derived : public Base {
public:
    int d = 2;
};

int main()
{
        // 1) 数值之间：C 里写 (int)x，C++ 推荐 static_cast<int>(x)
    double pi = 3.99;
    int    a  = static_cast<int>(pi);          // 截断
    int    b  = (int)pi;                       // C 风格也能用，但不显眼、不安全检查
    std::printf("1) static_cast<int>(3.99)=%d, C 风格 (int)3.99=%d\n", a, b);

        // 2) 向上转型（派生类 -> 基类）是隐式的，不需要转换
    Derived d;
    Base* pb = &d;                             // 隐式向上转型，永远安全
    std::printf("2) 向上转型不用写转换: pb->b=%d\n", pb->b);

        // 3) 向下转型（基类 -> 派生类）：static_cast 不做运行期检查
    Base* p = &d;
    Derived* pd = static_cast<Derived*>(p);    // 这里我们知道真实类型，所以安全
    std::printf("3) static_cast 向下转型: pd->d=%d\n", pd->d);

        // 4) 危险：真实类型不是 Derived 时，static_cast 依然"成功"，但结果是 UB
    Base real_base;
    Base* p2 = &real_base;
    Derived* bad = static_cast<Derived*>(p2);  // 编译通过，运行期无检查
    std::printf("4) 错误转型的指针=%p（能打印，但访问 bad->d 就是 UB）\n", (void*)bad);
    std::printf("   正确做法：用 dynamic_cast（ch13_ex15.cpp）\n");

        // 5) void* 与指针互转也常用 static_cast
    void* raw = static_cast<void*>(&d);
    Derived* back = static_cast<Derived*>(raw);
    std::printf("5) void* 往返: back->d=%d\n", back->d);
    return 0;
}
