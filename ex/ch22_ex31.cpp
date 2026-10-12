// ch22_ex31.cpp —— 例 22-31：CRTP 当"混入"（mixin）——自动补齐运算符
// 需求：只写 == 和 <，其余四个比较运算符自动拥有。
// 每个类都手写一遍太啰嗦，写成宏不好调试；CRTP 混入是标准答案。
#include <cstdio>
#include <string>

template <typename Derived>
struct Comparable {
    friend bool operator!=(const Derived& a, const Derived& b) { return !(a == b); }
    friend bool operator>(const Derived& a, const Derived& b) { return b < a; }
    friend bool operator<=(const Derived& a, const Derived& b) { return !(b < a); }
    friend bool operator>=(const Derived& a, const Derived& b) { return !(a < b); }
};

struct Version : Comparable<Version> {
    int major;
    int minor;

        // 坑：有基类之后就不再有"聚合初始化"了，{1, 4} 会被理解成
        // "先用 1 初始化基类 Comparable<Version>"，于是报 error C2078。
        // 解决：给一个普通构造函数。
    Version(int ma, int mi) : major(ma), minor(mi) {}

    friend bool operator==(const Version& a, const Version& b) {
        return a.major == b.major && a.minor == b.minor;
    }
    friend bool operator<(const Version& a, const Version& b) {
        return a.major != b.major ? a.major < b.major : a.minor < b.minor;
    }
};

// 第二个混入：给任何有 name() 的类型自动加 operator<<
template <typename Derived>
struct Printable {
    friend std::ostream& operator<<(std::ostream& os, const Derived& d) {
        return os << d.name();
    }
};

struct Device : Comparable<Device>, Printable<Device> {
    int id;
    explicit Device(int i) : id(i) {}  // 有基类时不能再用聚合初始化 {1}，得给构造函数
    const char* name() const { return id == 1 ? "IMU" : "BARO"; }

    friend bool operator==(const Device& a, const Device& b) { return a.id == b.id; }
    friend bool operator<(const Device& a, const Device& b) { return a.id < b.id; }
};

int main() {
    Version a{1, 4}, b{2, 0};
    std::printf("a==b %d  a!=b %d  a<b %d  a>b %d  a<=b %d  a>=b %d\n", (int)(a == b), (int)(a != b),
                (int)(a < b), (int)(a > b), (int)(a <= b), (int)(a >= b));
    std::printf("v%d.%d vs v%d.%d\n", a.major, a.minor, b.major, b.minor);

    Device d1{1}, d2{2};
    std::printf("d1=%d, d2=%d, d1<d2=%d, d1>=d2=%d\n", d1.id, d2.id, (int)(d1 < d2),
                (int)(d1 >= d2));

        // C++20 起有更省事的做法：三路比较运算符 <=>，一行全生成
    struct Modern {
        int major, minor;
        auto operator<=>(const Modern&) const = default;  // 自动生成全部 6 个
    };
    Modern m1{1, 4}, m2{2, 0};
    std::printf("C++20 <=> 默认版本: m1<m2=%d, m1==m2=%d\n", (int)(m1 < m2), (int)(m1 == m2));

        // 坑：混入的 == 必须由派生类提供（或者声明为 = default）。
        // 忘了写 == 时，operator!= 里的 a == b 找不到匹配，
        // 报错会出现在 Comparable 里而不是你的类里，第一次见很容易懵。
    std::printf("sizeof(Version)=%zu（混入是空基类，不占空间）\n", sizeof(Version));
    return 0;
}
