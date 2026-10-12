// ch16_ex01.cpp —— 函数模板：从"写四遍"到"写一遍"
#include <cstdio>
#include <cstring>
#include <string>
#include <typeinfo>

// ---------- C 的做法：为每种类型写一遍（宏也能凑合，但没有类型检查） ----------
int  imax(int a, int b)   { return a > b ? a : b; }
double dmax(double a, double b) { return a > b ? a : b; }
#define CMAX(a, b) ((a) > (b) ? (a) : (b))

// ---------- C++ 的做法：写一个函数模板 ----------
template <class T>
T tmax(T a, T b)
{
    return a > b ? a : b;
}

int main()
{
    std::printf("--- 1) C 风格：每种类型一个函数 ---\n");
    std::printf("  imax(3, 5)      = %d\n", imax(3, 5));
    std::printf("  dmax(2.5, 1.5)  = %.2f\n", dmax(2.5, 1.5));
    std::printf("  宏 CMAX(3, 5)   = %d（但宏没有类型检查，也没有调试信息）\n", CMAX(3, 5));

    std::printf("--- 2) C++ 函数模板：编译器按实参类型\"套\"出具体函数 ---\n");
    std::printf("  tmax(3, 5)          = %d     （T 推导为 int）\n", tmax(3, 5));
    std::printf("  tmax(2.5, 1.5)      = %.2f   （T 推导为 double）\n", tmax(2.5, 1.5));
    std::printf("  tmax('a', 'z')      = %c     （T 推导为 char）\n", tmax('a', 'z'));

    std::printf("--- 3) 模板对自定义类型同样有效（只要有 operator>）---\n");
    {
        std::string s1 = "apple", s2 = "banana";
        std::printf("  tmax(std::string)  = %s\n", tmax(s1, s2).c_str());
        std::printf("  注意：string 的 > 是按字典序比较的\n");
    }

    std::printf("--- 4) 显式指定模板参数 ---\n");
    std::printf("  tmax<double>(1, 2)   = %.2f（把 int 都转成 double 再比）\n", tmax<double>(1, 2));
        // tmax(1, 2.5);      // 编译错误：T 无法同时推成 int 和 double
    std::printf("  tmax(1, 2.5) 会报错：模板参数 T 有歧义（int vs double）\n");
    std::printf("  修法一：tmax<double>(1, 2.5) = %.2f\n", tmax<double>(1, 2.5));
    std::printf("  修法二：给两个模板参数（见例 16-3）\n");

    std::printf("--- 5) 模板不是\"一个函数\"，而是\"造函数的图纸\" ---\n");
    std::printf("  编译器看到 tmax(3, 5) 时，才用 T=int 实例化出一份真正的函数\n");
    std::printf("  sizeof(tmax(3,5)) = %zu，它就是个普通的 int\n", sizeof(tmax(3, 5)));
    std::printf("  这是编译期行为，运行时没有任何额外开销\n");

    return 0;
}
