// ch12_ex01.cpp —— class 与 struct 的唯一区别：默认访问权限
// 编译：check.bat ch12_ex01.cpp c++latest
#include <cstdio>

// ---- struct：默认 public ----
struct PointS {
    int x;            // 不写访问说明符 -> public
    int y;
};

// ---- class：默认 private ----
class PointC {
    int x;            // 不写访问说明符 -> private（外面访问会编译错误）
    int y;
public:
    void set(int a, int b) { x = a; y = b; }     // 只能通过成员函数访问
    int  get_x() const { return x; }
    int  get_y() const { return y; }
};

int main()
{
        // 1) struct 可以直接点出成员
    PointS s{ 1, 2 };
    std::printf("1) struct: s.x=%d s.y=%d\n", s.x, s.y);
    s.x = 10;
    std::printf("   改完: s.x=%d\n", s.x);

        // 2) class 的成员默认私有，必须走函数
    PointC c;
    c.set(3, 4);
    std::printf("2) class : c.get_x()=%d c.get_y()=%d\n", c.get_x(), c.get_y());
        // c.x = 5;              // 错误：无法访问 private 成员

        // 3) 两种写法在内存布局上没有区别
    std::printf("3) sizeof(PointS)=%llu sizeof(PointC)=%llu（布局一样）\n",
                (unsigned long long)sizeof(PointS), (unsigned long long)sizeof(PointC));

        // 4) 结论：C++ 里 class 和 struct 只是"默认权限"不同，语言层面没有别的差异
    std::printf("4) 惯例：纯数据用 struct，有封装/不变量的用 class\n");
    return 0;
}
