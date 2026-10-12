// ch12_ex15.cpp —— 友元（friend）：给外人开一扇门
// 编译：check.bat ch12_ex15.cpp c++latest
#include <cstdio>

class Matrix;                    // 前向声明（友元声明里要用到）

// 先声明要当"友元成员函数"的那个函数
class Determinant {
public:
    static double of(const Matrix& m);
};

class Matrix {
public:
    explicit Matrix(double diag) : a_(diag), b_(0), c_(0), d_(diag) {}

        // 1) 友元函数：不是成员，但能访问私有成员
    friend void print_matrix(const Matrix& m, const char* who);

        // 2) 友元类：整个类都能访问
    friend class Inspector;

        // 3) 友元成员函数：只给某个类的某个成员开权限
    friend double Determinant::of(const Matrix& m);

    double a() const { return a_; }

private:
    double a_, b_, c_, d_;
};

void print_matrix(const Matrix& m, const char* who)
{
    std::printf("  %s: [%.1f %.1f; %.1f %.1f]（友元函数读到了私有成员）\n",
                who, m.a_, m.b_, m.c_, m.d_);
}

class Inspector {
public:
    static double trace(const Matrix& m) { return m.a_ + m.d_; }
};

double Determinant::of(const Matrix& m) { return m.a_ * m.d_ - m.b_ * m.c_; }

int main()
{
    Matrix m(3.0);
    print_matrix(m, "1) 友元函数");
    std::printf("2) 友元类读到 trace=%.1f\n", Inspector::trace(m));
    std::printf("3) 友元成员函数读到 det=%.1f\n", Determinant::of(m));

    std::printf("4) 友元的三个要点：\n");
    std::printf("   - 友元关系不能被继承，也不互相传递\n");
    std::printf("   - 友元声明写在类里面，表示本类愿意让它访问私有成员\n");
    std::printf("   - 正当用途：运算符重载（operator<<）、测试代码、紧密协作的两个类\n");
    std::printf("   - 滥用友元 = 白写了封装，能用公开接口解决就别用友元\n");
    return 0;
}
