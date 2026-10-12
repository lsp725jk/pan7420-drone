// ch13_ex06.cpp —— 切分（object slicing）：派生类对象赋给基类对象
// 编译：check.bat ch13_ex06.cpp c++latest

// 实验：把派生类对象"按值"放进基类对象/基类容器，观察虚函数是否还生效。
#include <cstdio>

class Base {
public:
    virtual const char* who() const { return "Base"; }
    virtual double area() const { return 0.0; }
    virtual ~Base() = default;
    int base_data = 1;
};

class Derived : public Base {
public:
    const char* who() const override { return "Derived"; }
    double area() const override { return 42.0; }
    int derived_data = 99;               // 基类里没有的成员
};

// 按值接收基类：传派生类对象会发生切分
void take_by_value(Base b)
{
    std::printf("   按值传参: who=%s area=%.1f（派生部分已被切掉）\n", b.who(), b.area());
}

// 按引用接收：不切分，虚函数正常
void take_by_ref(const Base& b)
{
    std::printf("   按引用传参: who=%s area=%.1f（多态生效）\n", b.who(), b.area());
}

int main()
{
    Derived d;
    std::printf("1) 派生类对象本身: who=%s area=%.1f, derived_data=%d\n",
                d.who(), d.area(), d.derived_data);

        // 2) 赋值给基类对象 -> 切分
    Base b = d;                          // 只拷贝基类那部分，派生部分丢掉
    std::printf("2) Base b = d; 之后 b.who()=%s area=%.1f\n", b.who(), b.area());
    std::printf("   （注意 who() 调用的是 Base 版本，派生类的数据和行为都没了）\n");

        // 3) 按值传参 -> 同样切分
    std::printf("3) 传参对比：\n");
    take_by_value(d);
    take_by_ref(d);

        // 4) 放进"基类对象的容器" -> 仍然切分
    Base arr[2] = { Base(), d };         // 第二个元素被切成 Base
    std::printf("4) Base arr[2] = { Base(), d }; arr[1].who()=%s area=%.1f\n",
                arr[1].who(), arr[1].area());

    std::printf("5) 症状 -> 原因 -> 修法：\n");
    std::printf("   症状：明明传的是派生类对象，虚函数却调用了基类版本；派生类成员丢失\n");
    std::printf("   原因：按值拷贝只复制基类子对象（sizeof(Base) 那一段），派生部分是切掉的\n");
    std::printf("   修法：多态对象一律用 引用(&) 或 指针(*)，容器用 vector<unique_ptr<Base>>\n");
    return 0;
}
