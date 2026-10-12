// ch12_ex11.cpp —— 只允许一份：删除拷贝（= delete）
// 编译：check.bat ch12_ex11.cpp c++latest
#include <cstdio>

// 独占资源的类型：拷贝没有意义（想想一个串口不能被两个对象同时打开）
class SerialPort {
public:
    explicit SerialPort(int id) : id_(id)
    {
        std::printf("  [打开] 串口 COM%d\n", id_);
    }
    ~SerialPort()
    {
        std::printf("  [关闭] 串口 COM%d\n", id_);
    }

        // C++11 起：显式删掉拷贝操作，编译器会替我们报错
    SerialPort(const SerialPort&)            = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    int id() const { return id_; }

private:
    int id_;
};

// 反过来：一个"只能拷贝"的类比只有一个"只能移动"的类更少见，
// 但可以用同样的语法显式要求编译器生成（= default）
class Copyable {
public:
    Copyable() = default;
    Copyable(const Copyable&)            = default;   // 明确要求生成
    Copyable& operator=(const Copyable&) = default;
};

int main()
{
    SerialPort sp(3);
    std::printf("1) sp.id()=%d\n", sp.id());

        // 下面两行都会编译失败（这正是我们想要的）：
        // SerialPort sp2(sp);            // 错误：尝试引用已删除的函数
        // SerialPort sp3(4); sp3 = sp;   // 错误：同上

    Copyable c1;
    Copyable c2 = c1;                 // 合法：显式要求生成拷贝
    std::printf("2) Copyable 可以拷贝（= default）\n");
    (void)c2;

    std::printf("3) = delete 的好处：错误在编译期就暴露，而不是运行时崩\n");
    return 0;
}
