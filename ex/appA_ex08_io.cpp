// 附录A 探针 8：输入输出对照（printf / iostream / std::format / std::print）
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <format>
#include <print>
#include <string>
#include <sstream>

int main() {
    std::puts("=== 1. 基本格式化：同一件事三种写法 ===");
    int    n = 42;
    double d = 3.14159;
    std::string name = "pan-fc";

    std::printf("printf   : %s n=%d d=%.2f hex=%08X\n", name.c_str(), n, d, n);
    std::cout << "iostream : " << name << " n=" << n << " d=" << std::fixed
              << std::setprecision(2) << d << " hex=" << std::hex << std::uppercase
              << std::setw(8) << std::setfill('0') << n << std::dec << std::nouppercase
              << std::setfill(' ') << "\n";
    std::cout << std::format("format   : {} n={} d={:.2f} hex={:08X}\n", name, n, d, n);
    std::print("print    : {} n={} d={:.2f} hex={:08X}\n", name, n, d, n);

    std::puts("=== 2. std::print 对 Unicode 的处理 ===");
    std::print("  中文直接输出: 飞控 {} 号\n", 1);
    std::print("  对齐与填充: [{:>8}] [{:*^8}] [{:<8}]\n", "abc", "abc", "abc");
    std::print("  进制: {:b} {:o} {:x} {:#x}\n", 255, 255, 255, 255);

    std::puts("=== 3. 位置参数与命名（printf 靠顺序，format 可以指定）===");
    std::printf("printf  : %d %d %d\n", 1, 2, 3);
    std::print("format  : {2} {1} {0}\n", 1, 2, 3);

    std::puts("=== 4. 类型安全：printf 写错格式符编译器会警告，format 直接编译报错 ===");
    std::printf("  printf(\"%%d\", 3.14) 会输出垃圾，见附录 B 的 UB 清单\n");
    std::print("  format/print 参数类型不符是编译错误，不会拖到运行期\n");

    std::puts("=== 5. 性能相关的写法对照：拼接一个字符串 ===");
    std::string s1;
    for (int i = 0; i < 3; ++i) { char t[32]; std::snprintf(t, sizeof t, "%d,", i); s1 += t; }
    std::printf("  snprintf 拼接: %s\n", s1.c_str());

    std::ostringstream oss;
    for (int i = 0; i < 3; ++i) oss << i << ',';
    std::printf("  ostringstream: %s\n", oss.str().c_str());

    std::string s3;
    for (int i = 0; i < 3; ++i) s3 += std::format("{},", i);
    std::printf("  std::format  : %s\n", s3.c_str());

    std::puts("=== 6. 从字符串/流读数据 ===");
    int a = 0, b = 0;
    std::sscanf("12 34", "%d %d", &a, &b);
    std::printf("  sscanf: %d %d\n", a, b);
    std::istringstream iss("12 34");
    int c = 0, e = 0; iss >> c >> e;
    std::printf("  istringstream: %d %d\n", c, e);

    std::puts("=== 7. iostream 的状态位 vs C 的返回值 ===");
    std::printf("  scanf 靠返回成功转换个数；iostream 靠 fail()/eof() 状态位\n");
    std::cout << "  当前 cin 状态: good=" << std::cin.good() << " fail=" << std::cin.fail() << "\n";

    std::puts("=== 8. 混用 printf 与 cout 的坑（缓冲区不同步）===");
    std::puts("  C 的 stdout 与 C++ 的 std::cout 默认同步（sync_with_stdio(true)），");
    std::puts("  关掉同步会更快的，但混用时顺序可能错乱。");
    return 0;
}
