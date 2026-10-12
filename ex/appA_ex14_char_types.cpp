// 附录A 探针 14：char 的符号性、u8"" 的类型（两处容易记错的细节）
#include <cstdio>
#include <cstdint>
#include <type_traits>

int main() {
    std::puts("=== 1. char 的符号性（实现定义，本机有符号）===");
    char c = (char)0xFF;                 // 255 或 -1，取决于 char 是否有符号
    std::printf("  (char)0xFF 转成 int = %d  -> %s\n", (int)c,
                c < 0 ? "char 是有符号的" : "char 是无符号的");
    std::printf("  std::is_signed<char>::value = %d\n", (int)std::is_signed<char>::value);
    std::printf("  所以参与数值运算时要写清楚: sizeof(int8_t)=%zu sizeof(uint8_t)=%zu\n",
                sizeof(std::int8_t), sizeof(std::uint8_t));

    std::puts("=== 2. u8\"abc\" 的类型（C++20 起是 const char8_t[]）===");
    std::printf("  sizeof(u8\"abc\") = %zu（3 个字符 + 结尾 0，每字符 1 字节）\n", sizeof(u8"abc"));
    std::puts("  直接配 printf(\"%s\") 会报什么，看下面这行的编译警告：");
    std::printf("  %s\n", u8"abc");      // warning C4477：类型是 const char8_t*
    std::puts("  显式转换之后就没有警告：");
    std::printf("  %s\n", reinterpret_cast<const char*>(u8"abc"));

    std::puts("=== 3. 字符串字面量的大小对照（复习）===");
    std::printf("  sizeof(\"abc\")=%zu  sizeof(u8\"abc\")=%zu  sizeof(u\"abc\")=%zu\n",
                sizeof("abc"), sizeof(u8"abc"), sizeof(u"abc"));
    std::printf("  sizeof(U\"abc\")=%zu  sizeof(L\"abc\")=%zu\n",
                sizeof(U"abc"), sizeof(L"abc"));
    return 0;
}
