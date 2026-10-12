// ch17_ex04.cpp —— string 与 string_view
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

// 演示"C 风格用法"的坑：返回内部指针
const char* c_style_get(const std::string& s)
{
    return s.c_str();          // 只要 s 活着，指针就有效
}

// 用 string_view 做参数：不拷贝，也不要求以 '\0' 结尾
std::size_t count_char(std::string_view sv, char c)
{
    std::size_t n = 0;
    for (char ch : sv) {
        if (ch == c) ++n;
    }
    return n;
}

// string_view 可以从 string / const char* / 字面量 / 子串构造
std::string_view first_word(std::string_view sv)
{
    auto pos = sv.find(' ');
    return pos == std::string_view::npos ? sv : sv.substr(0, pos);
}

int main()
{
    std::printf("--- 1) string 和 C 的 char[] 对比 ---\n");
    {
                // C 风格
        char buf[32];
        std::strcpy(buf, "hello");
        std::strcat(buf, " world");            // 必须自己确认 buf 够大
        std::printf("    char[32]      : \"%s\"，strlen=%zu\n", buf, std::strlen(buf));
        std::printf("                    越界不会报错，长度要自己算\n");

                // C++ 风格
        std::string s = "hello";
        s += " world";
        s += "!";
        std::printf("    std::string   : \"%s\"，size=%zu\n", s.c_str(), s.size());
        std::printf("                    自动扩容，size() 随时可查\n");
        std::printf("    C++14 起还有字面量后缀：\"abc\"s 就是 std::string\n");
    }

    std::printf("--- 2) string 的常用操作速查 ---\n");
    {
        std::string s = "Hello World";
        std::printf("    s              = \"%s\"\n", s.c_str());
        std::printf("    s.substr(0,5)  = \"%s\"\n", s.substr(0, 5).c_str());
        std::printf("    s.find(\"World\")= %zu\n", s.find("World"));
        std::printf("    s.find(\"xyz\")  = npos? %d\n", s.find("xyz") == std::string::npos);
        std::printf("    s.empty()      = %d\n", s.empty() ? 1 : 0);
        s.replace(0, 5, "Goodbye");
        std::printf("    replace(0,5,\"Goodbye\") 后 = \"%s\"\n", s.c_str());
        s.erase(0, 8);
        std::printf("    erase(0,8) 后 = \"%s\"\n", s.c_str());
        s.insert(0, ">> ");
        std::printf("    insert(0,\">> \") 后 = \"%s\"\n", s.c_str());
    }

    std::printf("--- 3) string_view：只\"看\"字符串，不持有、不拷贝 ---\n");
    {
        std::string s = "hello world";
        std::string_view sv = s;                    // 不拷贝，只是 (指针, 长度)
        std::printf("    sizeof(std::string)      = %zu（通常 32 字节，含 SSO 缓冲）\n", sizeof(std::string));
        std::printf("    sizeof(std::string_view) = %zu（只有指针 + 长度）\n", sizeof(std::string_view));
        std::printf("    sv = \"%.*s\"（用 %%.*s 打印不需要 \\0 结尾）\n",
                    (int)sv.size(), sv.data());
        std::printf("    sv.size()=%zu\n", sv.size());

        std::string_view w = first_word(sv);
        std::printf("    first_word(sv) = \"%.*s\"（这是原字符串上的一段视图）\n",
                    (int)w.size(), w.data());
        std::printf("    它没有分配任何内存\n");
    }

    std::printf("--- 4) 关键坑：string_view 不拥有数据，源必须活着 ---\n");
    {
        std::string_view sv;
        {
            std::string tmp = "临时字符串";
            sv = tmp;
            std::printf("    tmp 还活着：sv = \"%.*s\"\n", (int)sv.size(), sv.data());
        }   // tmp 在这里销毁
        std::printf("    tmp 已销毁：sv 现在是**悬空视图**，访问它就是 UB\n");
        std::printf("    本示例不再访问它（避免真的崩）\n");

        std::printf("    同样危险：\n");
        std::printf("      std::string_view bad = std::string(\"x\") + \"y\";  // 临时对象立刻销毁\n");
        std::printf("      std::string_view bad2 = get_string_returning_by_value();\n");
    }

    std::printf("--- 5) string_view 的正确用法：做函数参数 ---\n");
    {
        std::string s = "mississippi";
        std::printf("    count_char(s, 's')        = %zu（string -> string_view，无拷贝）\n",
                    count_char(s, 's'));
        std::printf("    count_char(\"mississippi\", 's') = %zu（字面量直接用，无拷贝）\n",
                    count_char("mississippi", 's'));
        std::printf("    count_char(s.substr(0,4), 's') = %zu（注意 substr 这里产生了一次临时 string）\n",
                    count_char(s.substr(0, 4), 's'));
        std::printf("    如果参数是 const std::string&，传字面量会构造临时 string（有分配）\n");
        std::printf("    string_view 参数则完全没有分配\n");
    }

    std::printf("--- 6) 什么时候不能用 string_view ---\n");
    {
        std::printf("    X 存成员变量并长期持有（源可能先销毁）\n");
        std::printf("    X 需要以 '\\0' 结尾（string_view 不保证！）\n");
        std::printf("      传给 printf(\"%%s\") / strcmp / fopen 等 C 函数要小心\n");
        std::printf("    X 需要修改内容（string_view 是只读的）\n");
        std::printf("    正确用法：函数参数、临时的字符串切片\n");

        std::string s = "abc";
        std::string_view sv = s;
        std::printf("    从 std::string 得到的 view：sv.data()[sv.size()] == '\\0' ? %d（MSVC 上是 1）\n",
                    sv.data()[sv.size()] == '\0' ? 1 : 0);
        std::printf("    但从\"字符串中间\"切出来的 view 后面不是 '\\0'，所以不能直接给 %%s\n");
    }

    std::printf("--- 7) string 的 SSO（小字符串优化）---\n");
    {
        std::string small = "hi";
        std::string big(100, 'x');
        std::printf("    \"hi\"      大小=%zu，capacity=%zu\n", small.size(), small.capacity());
        std::printf("    100 个 'x' 大小=%zu，capacity=%zu\n", big.size(), big.capacity());
        std::printf("    短字符串直接存在对象内部（SSO），不分配堆内存\n");
        std::printf("    这一点对嵌入式很重要：短字符串（<16 字节左右）几乎零开销\n");
        std::printf("    （具体的 SSO 阈值是实现的细节，MSVC 上是 15 个字符）\n");
    }

    return 0;
}
