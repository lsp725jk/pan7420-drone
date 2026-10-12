// 附录A 探针 1：类型大小、字面量后缀、字面量写法
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <string>
#include <type_traits>

int main() {
    std::puts("=== 1. 基本类型大小（本机 x64 / MSVC 14.51）===");
    std::printf("bool=%zu  char=%zu  wchar_t=%zu  char16_t=%zu  char32_t=%zu  char8_t=%zu\n",
        sizeof(bool), sizeof(char), sizeof(wchar_t), sizeof(char16_t),
        sizeof(char32_t), sizeof(char8_t));
    std::printf("short=%zu  int=%zu  long=%zu  long long=%zu\n",
        sizeof(short), sizeof(int), sizeof(long), sizeof(long long));
    std::printf("float=%zu  double=%zu  long double=%zu\n",
        sizeof(float), sizeof(double), sizeof(long double));

    std::puts("=== 2. 固定宽度类型（<cstdint>）===");
    std::printf("int8_t=%zu int16_t=%zu int32_t=%zu int64_t=%zu\n",
        sizeof(std::int8_t), sizeof(std::int16_t), sizeof(std::int32_t), sizeof(std::int64_t));
    std::printf("intptr_t=%zu uintptr_t=%zu intmax_t=%zu\n",
        sizeof(std::intptr_t), sizeof(std::uintptr_t), sizeof(std::intmax_t));
    std::printf("size_t=%zu ptrdiff_t=%zu\n", sizeof(std::size_t), sizeof(std::ptrdiff_t));

    std::puts("=== 3. int32_t 与 int 是不是同一个类型 ===");
    std::printf("is_same<int32_t,int>          = %d\n", (int)std::is_same<std::int32_t, int>::value);
    std::printf("is_same<int64_t,long>         = %d\n", (int)std::is_same<std::int64_t, long>::value);
    std::printf("is_same<int64_t,long long>    = %d\n", (int)std::is_same<std::int64_t, long long>::value);
    std::printf("is_same<size_t,unsigned long long> = %d\n",
        (int)std::is_same<std::size_t, unsigned long long>::value);
    std::printf("is_same<size_t,unsigned __int64>   = %d\n",
        (int)std::is_same<std::size_t, unsigned long long>::value);

    std::puts("=== 4. 整数字面量：进制与前缀（同一数值的不同写法）===");
    std::printf("42=%d  052(八进制)=%d  0x2A(十六进制)=%d  0b101010(二进制)=%d\n",
        42, 052, 0x2A, 0b101010);
    std::printf("1'000'000(数位分隔符)=%d\n", 1'000'000);

    std::puts("=== 5. 整数后缀决定类型 ===");
    std::printf("sizeof(1)=%zu sizeof(1u)=%zu sizeof(1L)=%zu sizeof(1LL)=%zu sizeof(1ULL)=%zu\n",
        sizeof(1), sizeof(1u), sizeof(1L), sizeof(1LL), sizeof(1ULL));
    std::printf("sizeof(0xFFFFFFFF)=%zu (值 %llu)  sizeof(0xFFFFFFFFu)=%zu\n",
        sizeof(0xFFFFFFFF), (unsigned long long)0xFFFFFFFFULL, sizeof(0xFFFFFFFFu));

    std::puts("=== 6. 浮点字面量 ===");
    std::printf("sizeof(3.14f)=%zu sizeof(3.14)=%zu sizeof(3.14L)=%zu\n",
        sizeof(3.14f), sizeof(3.14), sizeof(3.14L));
    std::printf("1e3=%.1f  1.5e-3=%.6f  0x1p3(十六进制浮点)=%.1f  0x1.8p1=%.1f\n",
        1e3, 1.5e-3, 0x1p3, 0x1.8p1);

    std::puts("=== 7. 字符与字符串字面量前缀 ===");
    std::printf("sizeof('A')=%zu  sizeof(L'A')=%zu  sizeof(u'A')=%zu  sizeof(U'A')=%zu\n",
        sizeof('A'), sizeof(L'A'), sizeof(u'A'), sizeof(U'A'));
    std::printf("sizeof(\"abc\")=%zu（含结尾 '\\0'）  sizeof(u8\"abc\")=%zu\n",
        sizeof("abc"), sizeof(u8"abc"));
    std::printf("sizeof(u\"abc\")=%zu  sizeof(U\"abc\")=%zu  sizeof(L\"abc\")=%zu\n",
        sizeof(u"abc"), sizeof(U"abc"), sizeof(L"abc"));

    std::puts("=== 8. 圆括号初始化 vs 花括号初始化（窄化）===");
    int a = 3.9;      // 隐式窄化，编译通过（可能只警告）
    int b(3.9);       // 同上
    int c{3};         // 列表初始化，不允许窄化
    std::printf("int a = 3.9 -> %d ; int b(3.9) -> %d ; int c{3} -> %d\n", a, b, c);

    std::puts("=== 9. 整数回绕（无符号定义良好，有符号是 UB）===");
    unsigned u = 0u;
    u = u - 1u;
    std::printf("0u - 1u = %u （无符号回绕，标准保证）\n", u);
    return 0;
}
