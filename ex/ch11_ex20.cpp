// ch11_ex20.cpp —— 默认参数：C 里要写一堆重载/宏，C++ 一个声明搞定
// 编译：check.bat ch11_ex20.cpp c++latest
#include <cstdio>

// 默认参数只能写在声明里（如果声明和定义分开，只能写一次，通常是声明处）
int  open_port(int port, int baud = 115200, int stop_bits = 1);
void send(const char* data, int len = -1);        // len = -1 表示"按字符串长度"

int open_port(int port, int baud, int stop_bits)
{
    std::printf("  open_port(port=%d, baud=%d, stop=%d)\n", port, baud, stop_bits);
    return port;
}

void send(const char* data, int len)
{
    if (len < 0) {
        len = 0;
        while (data[len]) ++len;                  // 自己算长度
        std::printf("  按字符串发送 %d 字节: %s\n", len, data);
    } else {
        std::printf("  按长度发送 %d 字节\n", len);
    }
}

// 默认参数必须是"后面的参数才能省"：一旦某个参数有默认值，它右边都得有
int  config(int a, int b = 2, int c = 3);         // 合法
// int bad(int a = 1, int b);                     // 错误：b 没有默认值

int config(int a, int b, int c) { return a + b + c; }

int main()
{
        // 1) 从右往左省略
    open_port(1);
    open_port(1, 9600);
    open_port(1, 9600, 2);

        // 2) 想跳过中间的参数就必须写全（C++ 不支持"跳过"）
    send("hello");
    send("hello", 3);

        // 3) 默认参数与重载的相互作用
    std::printf("3) config(1)=%d, config(1,20)=%d, config(1,20,30)=%d\n",
                config(1), config(1, 20), config(1, 20, 30));

        // 4) 坑：默认参数在"声明处"求值，先声明后改的东西不会影响它
    std::printf("4) 默认值在编译期写死在调用点，改头文件后必须重新编译调用方\n");
    return 0;
}
