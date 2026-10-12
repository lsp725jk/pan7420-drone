// 附录B 探针 9：空指针解引用、空指针调用成员函数
#include <cstdio>
#include <cstddef>

struct S {
    int  x = 7;
    void printX()  { std::printf("  x = %d\n", x); }       // 访问了 this->x
    void printHi() { std::puts("  hi（没访问任何成员）"); } // 没解引用 this
    int  getX()    { return x; }
};

int main(int argc, char** argv) {
    std::printf("=== 1. 解引用空指针 ===");
    std::fflush(stdout);
    int* p = nullptr;
    if (argc > 5) {                       // 默认不执行，避免总是崩
        std::printf("  即将 *p = 1 ...\n");
        std::fflush(stdout);
        *p = 1;                           // UB：几乎必然访问违例
    }
    std::printf("（未执行。*nullptr 在 Windows 上通常直接 0xC0000005 访问违例）\n");

    std::printf("=== 2. 通过空指针调用成员函数 ===\n");
    S* s = nullptr;
    std::fflush(stdout);
    s->printHi();                         // 不访问成员时"常常看起来正常"
    std::printf("  上面这行没有崩 —— 很多编译器不检查 this 是否为空\n");
    if (argc > 3) {
        std::printf("  即将调用 s->getX()（会读 this->x）...\n");
        std::fflush(stdout);
        std::printf("  %d\n", s->getX());  // UB：读偏移 0 处的内存
    } else {
        std::printf("（未执行 s->getX()。它会读空指针加偏移的地址，通常崩）\n");
    }

    std::printf("=== 3. 空指针参与算术 ===");
    std::fflush(stdout);
    int* q = nullptr;
    std::printf("  q + 0 仍然是空指针；q + 1 是 UB（即使不算解引用）\n");
    std::printf("  比较 q == nullptr 是合法的，常用于判空\n");
    std::printf("  解引用前一定要判: %s\n", q ? "非空" : "空，不能解引用");

    std::printf("=== 4. 引用绑定到空指针解引用的结果（同样是 UB）===\n");
    std::puts("  int& r = *p; 这种写法在标准里就是 UB，不因为「引用不能为空」而变安全");
    return 0;
}
