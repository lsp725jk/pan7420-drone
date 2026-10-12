// ch11_ex22.cpp —— enum class：C++11 起的作用域枚举
// 编译：check.bat ch11_ex22.cpp c++latest
#include <cstdio>

// 名字被关进枚举自己的作用域里，不再污染外层
enum class Color { Red, Green, Blue };
enum class State { Idle, Running, Error };      // 和 Color 里的名字不再冲突

// 可以指定底层类型，跨平台大小确定（嵌入式里很有用）
enum class Pin : unsigned char { PA0, PA1, PA2 };

// 想当整数用必须显式转换 static_cast
int to_code(int c) { return c + 1; }

int main()
{
        // 1) 必须带枚举名，可读性更好
    Color c = Color::Green;
    State s = State::Running;
    std::printf("1) Color::Green=%d, State::Running=%d\n", (int)c, (int)s);

        // 2) 不会隐式转成 int
        // int x = Color::Green;                 // 错误：无法从 Color 转换为 int
    int x = static_cast<int>(Color::Green);  // 必须显式转换
    std::printf("2) static_cast<int>(Color::Green)=%d\n", x);

        // 3) 不同类型不能互相比较
        // if (c == s) ...                       // 错误：没有匹配的 == 运算符
    std::printf("3) Color 和 State 不能直接比较，编译器帮你挡住了\n");

        // 4) 指定底层类型后大小确定
    std::printf("4) sizeof(Pin)=%llu, sizeof(Color)=%llu, sizeof(State)=%llu\n",
                (unsigned long long)sizeof(Pin),
                (unsigned long long)sizeof(Color),
                (unsigned long long)sizeof(State));
    std::printf("   Pin::PA2 = %d\n", (int)Pin::PA2);

    (void)to_code(x);
    return 0;
}
