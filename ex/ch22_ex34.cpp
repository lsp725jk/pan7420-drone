// ch22_ex34.cpp —— 例 22-34：模板与二进制体积
// 模板"按用到的类型各生成一份代码"这条规则，在 PC 上没什么感觉，
// 在 64KB Flash 的 MCU 上会要命。先看清楚代价，再学怎么省。
#include <cstdio>
#include <cstring>

// -------- 写法 1：每一份都是完整实现 --------
// 这个函数体故意写得像回事（一段解析 + 一段校验 + 一段输出）
template <typename T>
long long parse_and_sum(const char* s, int n) {
    long long acc = 0;
    for (int i = 0; i < n; ++i) {
        T v{};
        std::memcpy(&v, s + i * sizeof(T), sizeof(T));  // 假装在解析字节流
        if constexpr (sizeof(T) >= 4) {
            acc += static_cast<long long>(v) / 3;
        } else {
            acc += static_cast<long long>(v) * 2;
        }
        acc ^= (acc >> 7);
    }
    return acc;
}

// -------- 写法 2：把与类型无关的部分抽成普通函数 --------
// 模板只负责"拆字节 / 转成 long long"，真正的大块代码只有一份
long long parse_core(long long v) {
    long long acc = 0;
    acc += v / 3;
    acc ^= (acc >> 7);
    return acc;
}

template <typename T>
long long parse_light(const char* s, int n) {
    long long acc = 0;
    for (int i = 0; i < n; ++i) {
        T v{};
        std::memcpy(&v, s + i * sizeof(T), sizeof(T));
        acc += parse_core(static_cast<long long>(v));
    }
    return acc;
}

int main() {
    char buf[64] = {0};
    buf[0] = 9;

        // 六种类型 -> 六份独立代码
    std::printf("parse_and_sum 的六份实例（地址各不相同 = 六份机器码）：\n");
    std::printf("  <int>     %p\n", reinterpret_cast<const void*>(&parse_and_sum<int>));
    std::printf("  <unsigned>%p\n", reinterpret_cast<const void*>(&parse_and_sum<unsigned>));
    std::printf("  <short>   %p\n", reinterpret_cast<const void*>(&parse_and_sum<short>));
    std::printf("  <long long>%p\n", reinterpret_cast<const void*>(&parse_and_sum<long long>));
    std::printf("  <float>   %p\n", reinterpret_cast<const void*>(&parse_and_sum<float>));
    std::printf("  <double>  %p\n", reinterpret_cast<const void*>(&parse_and_sum<double>));

    std::printf("parse_light 的六份实例（每份只是一层薄壳）：\n");
    std::printf("  <int>     %p\n", reinterpret_cast<const void*>(&parse_light<int>));
    std::printf("  <double>  %p\n", reinterpret_cast<const void*>(&parse_light<double>));

    std::printf("结果: %lld %lld\n", parse_and_sum<int>(buf, 4), parse_light<int>(buf, 4));

        // 结论（.text 段的实测大小见本章后面那张表）：
        // 模板实例化数 = 用到的类型数 × 大函数体
        // 想省 Flash，就把与类型无关的部分下沉到一个普通函数
    return 0;
}
