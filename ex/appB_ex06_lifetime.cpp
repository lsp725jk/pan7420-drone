// 附录B 探针 6：对象生存期类 UB
// 1) new[] 配 delete（不是 delete[]）
// 2) 通过非虚析构的基类指针 delete
// 3) 返回局部变量的地址/引用
// 4) std::move 之后继续使用被移走的对象
#include <cstdio>
#include <memory>
#include <string>
#include <vector>
#include <utility>

struct NonVirtBase {
    int* buf;
    NonVirtBase() : buf(new int[3]{1, 2, 3}) {}
    ~NonVirtBase() { std::printf("    ~NonVirtBase 被调用\n"); delete[] buf; }
};
struct DerivedBig : NonVirtBase {
    int* extra;
    DerivedBig() : extra(new int[8]{0}) {}
    ~DerivedBig() { std::printf("    ~DerivedBig 被调用\n"); delete[] extra; }
};

// 返回局部对象的引用：UB
const std::string& danglingRef() {
    std::string local = "我是局部对象";
    return local;                     // local 在函数返回时被销毁
}

int* danglingPtr() {
    int local = 12345;
    return &local;
}

int main() {
    std::printf("=== 1. new[] 配 delete（而不是 delete[]）===\n");
    {
        int* arr = new int[4]{1, 2, 3, 4};
        std::printf("  new int[4] 的指针用 delete（少写方括号）释放...\n");
        std::fflush(stdout);
        delete arr;                   // UB：应当是 delete[]
        std::printf("  没崩，但这种写法在别的类型上会漏掉析构调用\n");
    }

    std::printf("=== 2. 基类析构函数不是 virtual 时用基类指针 delete ===\n");
    {
        NonVirtBase* p = new DerivedBig();
        std::printf("  delete p（p 的静态类型是 NonVirtBase*）...\n");
        std::fflush(stdout);
        delete p;                     // UB：只会调用 ~NonVirtBase
        std::printf("  看上面的输出：~DerivedBig 根本没被调用 -> 它的 extra 泄漏了\n");
    }

    std::printf("=== 3. 返回局部变量的引用 ===\n");
    {
        const std::string& r = danglingRef();
        std::printf("  读到悬空引用: %s （内容可能已经被覆盖）\n", r.c_str());
        int* q = danglingPtr();
        std::printf("  读到悬空指针: %d （也可能是别的值）\n", *q);
    }

    std::printf("=== 4. std::move 之后继续使用 ===");
    {
        std::vector<int> src = {1, 2, 3, 4, 5};
        std::vector<int> dst = std::move(src);
        std::printf("  move 之后 src.size() = %zu （标准只说「有效但未指定」）\n", src.size());
        std::printf("  src.clear() 这类不依赖内容的操作仍然合法\n");
        src.clear();
                // 依赖 src 里的内容是错的：内容已被搬走
        std::printf("  dst.size() = %zu dst[0] = %d\n", dst.size(), dst[0]);
    }
    {
        std::string s1 = "hello";
        std::string s2 = std::move(s1);
        std::printf("  move 后 s1 = 「%s」 size=%zu （本机实现留空，但标准不保证）\n",
                    s1.c_str(), s1.size());
    }
    return 0;
}
