// 附录A 探针 12：头文件与 C++23 库特性可用性（本机 MSVC 14.51 实测）
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <ctime>
#include <cerrno>

// C++ 侧常用头
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <memory>
#include <optional>
#include <variant>
#include <tuple>
#include <utility>
#include <span>
#include <expected>
#include <bit>
#include <numbers>
#include <source_location>
#include <format>
#include <print>
#include <ranges>
#include <concepts>
#include <type_traits>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

// 验证 <xxx.h> 的 C 版本头在 C++ 里也能用（本机实测）
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main() {
    std::puts("=== 1. <cxxx> 与 <xxx.h> 都能用 ===");
    std::printf("  <cstdio> 的 std::printf 与 <stdio.h> 的 printf: %d %d\n",
                std::printf("a") > 0, printf("b") > 0);
    char buf[16];
    std::strcpy(buf, "ok");
    strcpy(buf, "ok2");
    std::printf("  <cstring>::strcpy 与 <string.h>::strcpy 都可用: %s\n", buf);
    std::printf("  <cstdlib>::abs=%d  <stdlib.h>::abs=%d\n", std::abs(-3), abs(-3));
    std::printf("  <cmath>::sqrt=%.3f  <math.h>::sqrt=%.3f\n", std::sqrt(2.0), sqrt(2.0));

    std::puts("=== 2. C++17/20/23 库特性 ===");
    std::optional<int> o = 42;
    std::printf("  std::optional: %d\n", *o);

    std::variant<int, std::string> v = std::string("variant");
    std::printf("  std::variant 持有: %s\n", std::get<std::string>(v).c_str());

    std::span<int> sp;
    int raw[4] = {1, 2, 3, 4};
    sp = raw;
    std::printf("  std::span size=%zu front=%d\n", sp.size(), sp.front());

    std::printf("  std::bit_cast: 1.0f -> 0x%08X\n", std::bit_cast<std::uint32_t>(1.0f));

    std::printf("  std::numbers::pi = %.10f\n", std::numbers::pi);

    auto loc = std::source_location::current();
    std::printf("  std::source_location: line=%u\n", loc.line());

    std::printf("  std::format: %s\n", std::format("{} + {} = {}", 1, 2, 3).c_str());
    std::print("  std::print（C++23）: {}\n", "hello");

    std::puts("=== 3. std::expected（C++23）===");
    auto ok  = []() -> std::expected<int, const char*> { return 7; };
    auto err = []() -> std::expected<int, const char*> { return std::unexpected("出错了"); };
    std::printf("  成功: has_value=%d value=%d\n", ok().has_value(), *ok());
    std::printf("  失败: has_value=%d error=%s\n", err().has_value(), err().error());

    std::puts("=== 4. ranges（C++20）===");
    std::vector<int> nums = {5, 3, 1, 4, 2};
    std::ranges::sort(nums);
    std::printf("  排序后: ");
    for (int n : nums) std::printf("%d ", n);
    std::printf("\n  取偶数并乘 10: ");
    for (int n : nums | std::views::filter([](int x) { return x % 2 == 0; })
                      | std::views::transform([](int x) { return x * 10; }))
        std::printf("%d ", n);
    std::printf("\n");

    std::puts("=== 5. concepts（C++20）===");
    auto show = []<std::integral T>(T x) { return x + 1; };
    std::printf("  受 integral 约束的模板: show(41) = %d\n", show(41));

    std::puts("=== 6. 时间与文件系统 ===");
    auto t0 = std::chrono::steady_clock::now();
    volatile int s = 0;
    for (int i = 0; i < 100000; ++i) s += i;
    auto t1 = std::chrono::steady_clock::now();
    std::printf("  计时: %lld us\n",
                (long long)std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
    std::printf("  std::filesystem 可用，当前目录存在? %d\n",
                (int)std::filesystem::exists(std::filesystem::current_path()));
    return 0;
}
