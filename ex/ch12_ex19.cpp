// ch12_ex19.cpp —— 三法则 / 五法则 / 零法则
// 编译：check.bat ch12_ex19.cpp c++latest
#include <cstdio>
#include <cstring>

// ---------- 遵守"三法则"的类：资源用原始指针管 ----------
class RuleOfThree {
public:
    explicit RuleOfThree(const char* s)
    {
        n_ = (int)std::strlen(s);
        p_ = new char[n_ + 1];
        std::memcpy(p_, s, n_ + 1);
        std::printf("  [构造] %s\n", p_);
    }
    ~RuleOfThree()                                  // 1) 析构
    {
        std::printf("  [析构] %s\n", p_);
        delete[] p_;
    }
    RuleOfThree(const RuleOfThree& o)                // 2) 拷贝构造
    {
        n_ = o.n_;
        p_ = new char[n_ + 1];
        std::memcpy(p_, o.p_, n_ + 1);
        std::printf("  [拷贝构造] %s\n", p_);
    }
    RuleOfThree& operator=(const RuleOfThree& o)     // 3) 拷贝赋值
    {
        std::printf("  [拷贝赋值] %s <- %s\n", p_, o.p_);
        if (this != &o) {
            delete[] p_;
            n_ = o.n_;
            p_ = new char[n_ + 1];
            std::memcpy(p_, o.p_, n_ + 1);
        }
        return *this;
    }
    const char* text() const { return p_; }
private:
    char* p_;
    int   n_;
};

// ---------- 遵守"零法则"的类：资源交给成员对象管 ----------
class RuleOfZero {
public:
    explicit RuleOfZero(const char* s) : text_(s) { std::printf("  [零法则构造] %s\n", text_); }
        // 不写析构、不写拷贝构造、不写拷贝赋值 —— 编译器生成的版本全部正确
    const char* text() const { return text_; }
private:
    const char* text_;      // 这里只是个借用指针；真实项目里应换成 std::string / 智能指针
};

int main()
{
    std::printf("1) 三法则的类：我写了析构，所以我也必须写拷贝构造和拷贝赋值\n");
    {
        RuleOfThree a("alpha");
        RuleOfThree b(a);        // 深拷贝
        RuleOfThree c("gamma");
        c = a;
        std::printf("   结果: a=%s b=%s c=%s\n", a.text(), b.text(), c.text());
    }

    std::printf("2) 零法则的类：什么都不用写，编译器生成的就对\n");
    {
        RuleOfZero x("xx");
        RuleOfZero y(x);
        std::printf("   结果: x=%s y=%s\n", x.text(), y.text());
    }

    std::printf("3) 五法则 = 三法则 + 移动构造 + 移动赋值（C++11 起，详见第 14 章）\n");
    std::printf("4) 选择的优先顺序：\n");
    std::printf("   零法则 > 五法则 > 三法则\n");
    std::printf("   能靠成员对象管资源（string/vector/智能指针）就不要自己 new/delete\n");
    return 0;
}
