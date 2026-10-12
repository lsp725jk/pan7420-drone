// ch12_ex17.cpp —— 运算符重载 2：<< 输出、== 比较、[] 与 () 调用
// 编译：check.bat ch12_ex17.cpp c++latest
#include <cstdio>
#include <iostream>
#include <ostream>

class Ring {                      // 固定长度的小环形缓冲（教学用）
public:
    Ring() { for (int i = 0; i < N; ++i) data_[i] = 0; }

        // ---- [] 下标运算符：返回引用，才能写 r[i] = x ----
    int& operator[](int i) { return data_[i % N]; }
    const int& operator[](int i) const { return data_[i % N]; }   // const 版本

        // ---- () 函数调用运算符：让对象"像函数一样"被调用 ----
    int operator()(int i) const { return data_[i % N]; }

        // ---- == 比较：成员函数写法 ----
    bool operator==(const Ring& o) const
    {
        for (int i = 0; i < N; ++i)
            if (data_[i] != o.data_[i]) return false;
        return true;
    }
    bool operator!=(const Ring& o) const { return !(*this == o); }  // C++20 起可自动生成

        // ---- << 输出：必须是"非成员函数"，因为左操作数是 ostream ----
    friend std::ostream& operator<<(std::ostream& os, const Ring& r);

    static const int N = 4;

private:
    int data_[N];
};

std::ostream& operator<<(std::ostream& os, const Ring& r)
{
    os << "[";
    for (int i = 0; i < Ring::N; ++i) {
        os << r.data_[i];
        if (i + 1 < Ring::N) os << ", ";
    }
    os << "]";
    return os;                    // 返回流引用，才能连写 a << b << c
}

int main()
{
    Ring r;
        // 1) 用 [] 写入
    r[0] = 10; r[1] = 20; r[2] = 30; r[3] = 40;
        // 2) 用 << 输出（自定义类型也能像内置类型一样打印）
    std::cout << "1) r = " << r << "\n";

        // 3) 下标取模：越界不会崩（这是这个类自己的约定）
    std::cout << "2) r[5] = " << r[5] << "（5 % 4 = 1，取到第 2 个元素）\n";

        // 4) 用 () 读
    std::cout << "3) r(2) = " << r(2) << "\n";

        // 5) 用 == 比较
    Ring s;
    std::cout << "4) r == s ? " << (r == s) << "\n";
    s = r;
    std::cout << "   s = r 之后 r == s ? " << (r == s) << "\n";

        // 连写输出
    std::cout << "5) 连写: " << r << " " << s << "\n";
    return 0;
}
