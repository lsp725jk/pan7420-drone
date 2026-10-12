// ch13_ex16.cpp —— 四种类型转换对比（3）：const_cast 与 reinterpret_cast
// 编译：check.bat ch13_ex16.cpp c++latest
#include <cstdio>
#include <cstdint>

// 老式 C 接口：参数不是 const（比如早期写的不规范库函数）
void legacy_print(char* s)
{
    std::printf("   legacy_print: %s\n", s);
}

class Blob {
public:
    explicit Blob(int v) : v_(v) { }
    int v() const { return v_; }
private:
    int v_;
};

int main()
{
        // ---- const_cast：只用来去掉/加上 const，不做别的转换 ----
    const char* msg = "hello";
        // legacy_print(msg);                    // 错误：不能把 const char* 给 char*
    legacy_print(const_cast<char*>(msg));    // 能编过（前提：这个函数真的不改内容）

        // 1) 反例：真的修改了原本就 const 的对象 -> UB
    const int ci = 42;
    int* pci = const_cast<int*>(&ci);
    std::printf("1) const_cast 去 const 后的值=%d（读没问题）\n", *pci);
    *pci = 7;                                // UB：ci 本身是 const 对象
    std::printf("   写完之后 *pci=%d, ci=%d（读 ci 的结果不可靠，可能仍是 42）\n", *pci, ci);

        // 2) 正确用法：对象本身不是 const，只是你手上拿的是 const 引用
    int normal = 5;
    const int& r = normal;
    const_cast<int&>(r) = 9;                 // 合法：normal 本身可修改
    std::printf("2) 正确用法: normal=%d\n", normal);

        // ---- reinterpret_cast：重新解释比特，最危险 ----
    std::printf("3) reinterpret_cast 的几种用法：\n");
    Blob blob(0x01020304);
    unsigned char* bytes = reinterpret_cast<unsigned char*>(&blob);
    std::printf("   把对象当字节数组看: %02X %02X %02X %02X\n",
                bytes[0], bytes[1], bytes[2], bytes[3]);

    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(&blob);
    std::printf("   指针 <-> 整数: addr=0x%llX\n", (unsigned long long)addr);

        // 4) 四种转换的适用场合总结
    std::printf("4) 选择顺序：\n");
    std::printf("   static_cast      : 数值转换、向上转型、明确的向下转型、void* 往返（首选）\n");
    std::printf("   dynamic_cast     : 多态类型的安全向下转型（有运行期检查，失败返回 nullptr）\n");
    std::printf("   const_cast       : 只加减 const（不要用它改真正 const 的对象）\n");
    std::printf("   reinterpret_cast : 位重解释、指针与整数互转（平台相关，尽量别用）\n");
    std::printf("   一次都不要用 C 风格的 (T)x：上面四种它都能干，出问题时无法检索\n");
    return 0;
}
