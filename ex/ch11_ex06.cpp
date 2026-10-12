// ch11_ex06.cpp —— 输出中文：控制台代码页与 UTF-8
// 编译：check.bat ch11_ex06.cpp c++latest
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

// 把 MSVC 的宽字符串（UTF-16）转成 UTF-8，模拟"从别处拿到中文"的场景
static std::string to_utf8(const std::wstring& w)
{
    if (w.empty()) return std::string();
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
                                nullptr, 0, nullptr, nullptr);
    std::string out((size_t)n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(),
                        out.data(), n, nullptr, nullptr);
    return out;
}

// 写宽字符：控制台句柄存在就直接 WriteConsoleW；
// 输出被重定向到文件/管道时 WriteConsoleW 会失败，退回 UTF-8 窄字符。
static void print_wide(const wchar_t* text)
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h && h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        std::wcout.flush();
        DWORD written = 0;
        WriteConsoleW(h, text, (DWORD)wcslen(text), &written, nullptr);
        return;
    }
#endif
    std::cout << to_utf8(text);          // 非控制台：直接给 UTF-8 字节
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // 控制台代码页切到 UTF-8
    SetConsoleCP(CP_UTF8);         // 输入也切过去
#endif

        // 1) 源码里的窄字符串字面量：MSVC 用 /utf-8 编译，字节就是 UTF-8
    std::cout << "1) 源文件里的中文：姿态角 = 12.5 度\n";

        // 2) 由 UTF-16 转出来的中文
    std::cout << to_utf8(L"2) 运行时转出来的中文：电机 1 转速 = 1200") << "\n";

        // 3) 字符个数 vs 字节个数：UTF-8 下一个汉字 3 字节
    std::string cn = "姿态角";
    std::cout << "3) 汉字 " << cn.size() << " 字节, strlen=" << std::strlen(cn.c_str())
              << " 字节\n";

        // 4) 宽字符输出（自动适配控制台/重定向两种情况）
    print_wide(L"4) 宽字符中文输出：横滚 3.5 度\n");

    std::cout << "5) 混合输出顺序正常\n";
    return 0;
}
