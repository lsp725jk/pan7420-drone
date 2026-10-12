// ch22_ex02.cpp —— 例 22-2：函数模板，一份代码配所有类型
// 解决的问题：例 22-1 里那三样东西（指针、元素大小、比较器）全都不用给了。
#include <cstddef>
#include <cstdio>

// 最易读的写法：一个函数模板。
// template <typename T> 的意思是"下面这段代码里 T 是个占位类型"，
// 编译器看到调用时才把 T 换成实参的类型，现场生成一份真正的函数。
template <typename T>
T tmax(const T* p, std::size_t n) {
    T best = p[0];
    for (std::size_t i = 1; i < n; ++i) {
        if (p[i] > best) {
            best = p[i];
        }
    }
    return best;
}

// 两个参数共用一个 T：要求两个实参推出来的是同一个类型
template <typename T>
T tmax2(T a, T b) {
    return a > b ? a : b;
}

// 用户自定义类型也行，只要有 operator>
struct Version {
    int major;
    int minor;
};
bool operator>(const Version& a, const Version& b) {
    return (a.major != b.major) ? (a.major > b.major) : (a.minor > b.minor);
}

int main() {
    int ia[5] = {3, 9, 4, 1, 7};
    double da[5] = {3.5, 9.25, 4.0, 1.5, 7.75};
    Version va[3] = {{1, 4}, {2, 0}, {1, 12}};

        // 隐式推导：T 由实参自己定
    std::printf("T=int    : %d\n", tmax(ia, 5));
    std::printf("T=double : %g\n", tmax(da, 5));

    Version bv = tmax(va, 3);
    std::printf("T=Version: v%d.%d\n", bv.major, bv.minor);

        // 显式指定模板实参：用在"推导不出来"或"想改类型"的场合
    std::printf("显式指定 : %g\n", tmax2<double>(3, 4.5));  // 3 被转成 3.0

        // 证明"一个模板真的生成了好几份函数"：地址不一样
    std::printf("&tmax<int>    = %p\n", reinterpret_cast<const void*>(&tmax<int>));
    std::printf("&tmax<double> = %p\n", reinterpret_cast<const void*>(&tmax<double>));
    std::printf("两个地址不同 -> 内存里是两份独立代码\n");
    return 0;
}
