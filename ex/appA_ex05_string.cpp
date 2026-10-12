// 附录A 探针 5：字符串处理对照（C 的 str* vs std::string vs std::string_view）
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <string_view>
#include <algorithm>

int main() {
    std::puts("=== 1. 创建与长度 ===");
    const char* cstr = "hello";
    std::string s = "hello";
    std::string_view sv = s;
    std::printf("strlen(cstr)=%zu  s.size()=%zu  sv.size()=%zu  sizeof(s)=%zu sizeof(sv)=%zu\n",
                std::strlen(cstr), s.size(), sv.size(), sizeof(s), sizeof(sv));

    std::puts("=== 2. 拼接 ===");
    std::string a = "foo", b = "bar";
    std::string cat = a + b;              // 直接 +
    char cbuf[32];
    std::strcpy(cbuf, a.c_str());
    std::strcat(cbuf, b.c_str());         // C：要自己保证缓冲区够大
    std::printf("C++: %s   C: %s\n", cat.c_str(), cbuf);

    std::puts("=== 3. 查找 ===");
    std::string hay = "pan-fc-2024";
    std::printf("C  strstr: %s\n", std::strstr(hay.c_str(), "fc"));
    std::printf("C++ find 返回下标: %zu （npos=%zu）\n", hay.find("fc"), std::string::npos);
    std::printf("找不到时: find(\"zz\")==npos ? %d\n", hay.find("zz") == std::string::npos);
    std::printf("string_view::find 同样可用: %zu\n", std::string_view(hay).find("2024"));

    std::puts("=== 4. 子串 ===");
    std::printf("C  memcpy 取 [4,6): ");
    char sub[8]; std::memcpy(sub, hay.data() + 4, 2); sub[2] = '\0';
    std::printf("%s\n", sub);
    std::printf("C++ substr(4,2) = %s   （返回新 string，有拷贝）\n", hay.substr(4, 2).c_str());
    std::printf("string_view substr(4,2) = %.*s   （不拷贝）\n",
                (int)std::string_view(hay).substr(4, 2).size(), std::string_view(hay).substr(4, 2).data());

    std::puts("=== 5. 数值 <-> 字符串 ===");
    char nbuf[32];
    std::snprintf(nbuf, sizeof nbuf, "%d", 1234);            // C
    std::printf("C snprintf: %s   atoi: %d   strtol: %ld\n",
                nbuf, std::atoi("1234"), std::strtol("-56", nullptr, 10));
    std::printf("C++ to_string: %s   stoi: %d   stod: %.2f\n",
                std::to_string(1234).c_str(), std::stoi("1234"), std::stod("3.14"));

    std::puts("=== 6. c_str() / data() 的 NUL 结尾保证 ===");
    std::string z = "abc";
    std::printf("z[3]（越界读，但 C++11 起保证是 '\\0'）: %d\n", (int)z[3]);
    std::printf("string_view 没有这个保证: sv.data()[sv.size()] 未实测（越界读是 UB）\n");

    std::puts("=== 7. 小字符串优化 SSO（本机实测）===");
    for (int n : {0, 8, 15, 16, 17, 31, 32}) {
        std::string t(n, 'x');
        std::printf("  长度 %2d: sizeof=%zu  data()远离栈变量? %s\n",
                    n, sizeof(t), (std::abs((char*)t.data() - (char*)&t) > 64) ? "堆上" : "对象内部");
    }

    std::puts("=== 8. 修改 ===");
    std::string m = "hello world";
    m.replace(6, 5, "there");        // 原地替换
    m += "!";
    std::printf("replace+append: %s\n", m.c_str());
    char cb2[32] = "hello world";
    std::memcpy(cb2 + 6, "there", 5);
    std::strcat(cb2, "!");
    std::printf("C 版等价: %s （memcpy 长度必须自己算，写错就是 UB）\n", cb2);

    std::puts("=== 9. C 头文件 <-> C++ 头文件都能用 ===");
    std::printf("<cstring> 的 strlen 与 <string.h> 的 strlen 是同一个: %zu\n", std::strlen("1234"));
    return 0;
}
