// ch22_ex29.cpp —— 例 22-29：CRTP——把派生类的名字交给基类
// CRTP = Curiously Recurring Template Pattern（奇异递归模板模式）。
// 一句话：基类是个模板，模板参数就是派生类自己。于是基类能在**编译期**
// 知道"我下面是谁"，也就能直接调用派生类的函数，不需要虚函数表。
#include <cstdio>

// -------- 用途 1：给每个派生类单独记一份实例计数 --------
template <typename Derived>
class Counted {
public:
    Counted() { ++count_; }
    Counted(const Counted&) { ++count_; }
    Counted& operator=(const Counted&) = default;
    ~Counted() { --count_; }

    static int count() { return count_; }

private:
    static int count_;  // 每个 Derived 一份，因为 Counted<A> 和 Counted<B> 是两个类型
};
template <typename Derived>
int Counted<Derived>::count_ = 0;

// -------- 用途 2：静态接口（基类提供统一入口，实现由派生类给） --------
template <typename Derived>
class Sensor {
public:
        // 基类里的统一接口，编译期绑定到派生类的实现
    void read_and_print() {
        const int v = static_cast<Derived*>(this)->read_raw();  // 注意这个转换
        std::printf("读数 = %d\n", v);
    }
};

class Widget : public Counted<Widget>, public Sensor<Widget> {
public:
    int read_raw() { return 100 + id; }
    int id = 7;
};

class Gadget : public Counted<Gadget>, public Sensor<Gadget> {
public:
    int read_raw() { return 200 + id; }
    int id = 3;
};

int main() {
    std::printf("开始时: Widget=%d Gadget=%d\n", Counted<Widget>::count(),
                Counted<Gadget>::count());
    {
        Widget w1, w2;
        Gadget g1;
        std::printf("构造后: Widget=%d Gadget=%d\n", Counted<Widget>::count(),
                    Counted<Gadget>::count());
        w1.read_and_print();
        g1.read_and_print();
    }
    std::printf("析构后: Widget=%d Gadget=%d\n", Counted<Widget>::count(),
                Counted<Gadget>::count());

        // 关键点：Counted<Widget> 和 Counted<Gadget> 是**两个不同的类**，
        // 各自有一份 static count_。这就是"基类知道派生类是谁"的用处。
    std::printf("sizeof(Widget)=%zu（两个空基类被优化掉，只剩 int id）\n", sizeof(Widget));

        // 坑 1：static_cast<Derived*>(this) 不能省。
        // 写成 this->read_raw() 会去找 Sensor<Derived> 里的 read_raw，
        // 找不到就报错；写成 static_cast<Sensor*>(this)->... 会无限递归。
        // 坑 2：CRTP 基类的析构函数**不要**声明为虚函数（那就白用 CRTP 了），
        // 但要把析构设为 protected，防止有人 delete 一个 Sensor<Widget>* 造成 UB。
    return 0;
}
