// 附录A 探针 14：不要自己定义与 C 标准库同名的函数
// 本机 MSVC 14.51 实测的报错。
#include <cstdlib>   // 这里已经声明了 std::div(int,int)

// 与 <cstdlib> 里的 std::div 同名、同参数、不同返回类型
struct mydiv_t { int quot; int rem; };
mydiv_t div(int a, int b) { mydiv_t r{ a / b, a % b }; return r; }

int main() {
    mydiv_t r = div(7, 2);
    return r.quot + r.rem;
}
