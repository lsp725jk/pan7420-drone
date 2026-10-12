// 附录B 探针 10：const 对象被改、字符串字面量被改、缓冲区函数越界
#include <cstdio>
#include <cstring>

const int kConstGlobal = 42;

char* getLiteral() {
    return (char*)"hello";      // 字符串字面量是只读的
}

int main(int argc, char** argv) {
    std::printf("=== 1. 修改真正的 const 对象 ===\n");
    std::printf("  kConstGlobal = %d\n", kConstGlobal);
    if (argc > 4) {
        int* p = const_cast<int*>(&kConstGlobal);
        std::printf("  即将通过 const_cast 写它...\n");
        std::fflush(stdout);
        *p = 100;                              // UB：对象本身是 const
        std::printf("  写完了，读回 = %d\n", kConstGlobal);
    } else {
        std::printf("（未执行。const_cast 去掉 const 后写入 const 对象是 UB，\n");
        std::printf("  编译器可能把值放在只读段导致直接崩，也可能把它当常量折叠掉）\n");
    }

    std::printf("=== 2. 修改字符串字面量 ===\n");
    char* lit = getLiteral();
    std::printf("  字面量内容 = %s\n", lit);
    if (argc > 6) {
        std::printf("  即将写 lit[0] = 'H' ...\n");
        std::fflush(stdout);
        lit[0] = 'H';                          // UB：字面量在只读数据段
        std::printf("  写成功? %s\n", lit);
    } else {
        std::printf("（未执行。字面量通常在 .rdata 只读段，写它一般直接访问违例）\n");
    }

    std::printf("=== 3. strcpy/strcat 越界（C 里最常见的 UB）===\n");
    {
        char small[8];
        std::printf("  往 char[8] 里 strcpy 一个 20 字符的串...\n");
        std::fflush(stdout);
        std::strcpy(small, "0123456789ABCDEFGHI");   // 越界写：UB，栈被踩坏
        std::printf("  strcpy 返回了，内容 = %s （栈上的其它数据可能已被破坏）\n", small);
    }
    std::printf("  安全写法：snprintf / strncpy_s / std::string\n");
    {
        char safe[8];
        std::snprintf(safe, sizeof safe, "%s", "0123456789ABCDEFGHI");
        std::printf("  snprintf 截断后 = %s\n", safe);
    }
    return 0;
}
