// ch11_ex12.cpp —— const 与函数、返回值、成员的组合
// 编译：check.bat ch11_ex12.cpp c++latest
// 说明：const 成员函数要做到"*this 是常量"，这是第 12 章"常量成员函数"的伏笔。
#include <cstdio>
#include <cstring>

class Sensor {
public:
    Sensor() { std::snprintf(name_, sizeof(name_), "%s", "imu0"); value_ = 0; }

        // 读取：不修改对象 -> 函数尾部加 const
    int  value() const { return value_; }
    const char* name() const { return name_; }

        // 写入：修改对象 -> 不加 const
    void set_value(int v) { value_ = v; }

private:
    char name_[8];
    int  value_;
};

// 参数是 const 引用时，只能调用它的 const 成员函数
void dump(const Sensor& s)
{
    std::printf("  dump: %s = %d\n", s.name(), s.value());
        // s.set_value(1);          // 错误：s 是 const，不能调用非 const 成员
}

int main()
{
    Sensor s;
    s.set_value(42);
    dump(s);

        // 1) 非 const 对象既能调用 const 成员，也能调用非 const 成员
    std::printf("1) s.value()=%d\n", s.value());

        // 2) const 对象只能调用 const 成员
    const Sensor cs;
    std::printf("2) const 对象: cs.value()=%d\n", cs.value());
        // cs.set_value(1);         // 错误：对象含不可修改的类型限定符

        // 3) 用 const 引用指同一块对象，仍然能被别的路径改
    const Sensor& ref = s;
    s.set_value(99);
    std::printf("3) ref.value()=%d（ref 是只读视图，对象本身可以被别人改）\n", ref.value());

    return 0;
}
