// ch22_ex03.cpp —— 例 22-3：类模板 + 非类型模板参数（把"值"变成编译期常量）
// 模板参数不只能是类型，也可以是整数。这是"编译期计算"的第一块砖：
// 凡是写在 <> 里的东西，编译期就必须知道，于是它可以拿来当数组长度、
// 拿来当循环上界、拿来在编译期做判断。
#include <cstddef>
#include <cstdio>
#include <typeinfo>

template <typename T, std::size_t N>
class FixedVec {
        // 模板参数 N 是编译期常量，所以 static_assert 能用它
    static_assert(N > 0, "FixedVec 的容量必须大于 0");

public:
    static constexpr std::size_t capacity() { return N; }

    std::size_t size() const { return size_; }

        // 越界直接拒绝，而不是踩内存
    bool push_back(const T& v) {
        if (size_ >= N) {
            return false;
        }
        data_[size_++] = v;
        return true;
    }

    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

private:
    T data_[N]{};          // 数组长度直接用 N —— 运行期不可变
    std::size_t size_ = 0;
};

// 再加一个演示专用的小模板：把整数当模板参数
template <int Shift>
struct Shifter {
    static int apply(int x) { return x << Shift; }
};

int main() {
    FixedVec<int, 8> vi;
    FixedVec<double, 8> vd;
    FixedVec<int, 4> v4;

    for (int i = 1; i <= 10; ++i) {
        vi.push_back(i * i);
    }
    std::printf("vi.size()=%zu 容量=%zu\n", vi.size(), FixedVec<int, 8>::capacity());
    std::printf("sizeof(FixedVec<int,8>)=%zu  = 8*4 字节数据 + %zu 字节 size_\n",
                sizeof(vi), sizeof(std::size_t));
    std::printf("sizeof(FixedVec<int,4>)=%zu\n", sizeof(v4));

        // 容量不同的两个 FixedVec 是**两个不同的类型**
    std::printf("typeid: %s\n", typeid(FixedVec<int, 8>).name());
    std::printf("typeid: %s\n", typeid(FixedVec<int, 4>).name());
    std::printf("typeid: %s\n", typeid(FixedVec<double, 8>).name());

    vd.push_back(1.5);
    std::printf("vd[0]=%g\n", vd[0]);

        // 非类型模板参数：左移位数在编译期就定了
    std::printf("Shifter<1>::apply(6) = %d\n", Shifter<1>::apply(6));
    std::printf("Shifter<3>::apply(6) = %d\n", Shifter<3>::apply(6));
    return 0;
}
