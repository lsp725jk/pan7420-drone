// ch18_ex02_unwinding_raii.cpp —— 栈展开（stack unwinding）与 RAII 的配合
// 编译: check.bat ch18_ex02_unwinding_raii.cpp c++latest
#include <iostream>
#include <stdexcept>
#include <string>

// 一个"会说话"的资源类：构造时申请，析构时释放，并且每次进出都打印。
// 它用来把"析构函数什么时候被调用"变成肉眼可见的输出。
struct Tracer {
    std::string name;
    explicit Tracer(std::string n) : name(std::move(n)) {
        std::cout << "    + 构造 " << name << "\n";
    }
    ~Tracer() {
        std::cout << "    - 析构 " << name << "\n";
    }
};

// 第 3 层：抛异常的地方
void level3()
{
    Tracer t3("Tracer@level3");
    std::cout << "  level3: 即将 throw\n";
    throw std::runtime_error("level3 出错");
        // 这里本来可以再写一句 std::cout << "这行不执行";
        // 用来证明 throw 立刻中断本函数。但编译器会报 warning C4702
        // （无法访问的代码），而且那句话**永远不会出现在运行结果里**，
        // 所以用注释代替，保持示例零警告。
}

// 第 2 层：自己不清理，让 RAII 清理
void level2()
{
    Tracer t2("Tracer@level2");
    level3();
    std::cout << "  level2: 这行也不执行\n";
}

// 第 1 层：不用 try，让异常继续往上飞
void level1()
{
    Tracer t1("Tracer@level1");
    level2();
    std::cout << "  level1: 这行也不执行\n";
}

int main()
{
    std::cout << "=== A. 异常穿过三层函数，看析构顺序 ===\n";
    try {
        Tracer tmain("Tracer@main-try");
        level1();
    }
    catch (const std::runtime_error& e) {
        std::cout << "  catch 到: " << e.what() << "\n";
    }

    std::cout << "\n=== B. 同样的过程，但中间层用 try 提前拦下 ===\n";
    struct Guard {                       // 局部 RAII 守卫：离开作用域就打印
        ~Guard() { std::cout << "    - Guard 析构（作用域结束）\n"; }
    };
    {
        Guard g;
        std::cout << "    + Guard 构造\n";
        try {
            level3();
        }
        catch (const std::runtime_error& e) {
            std::cout << "  就地处理: " << e.what() << "\n";
        }
        std::cout << "  异常已处理，函数继续往下走\n";
    }

    std::cout << "\n=== C. 对比：C 风格的手动清理要写几遍 ===\n";
    std::cout << "  （看代码注释：3 个 return 点就要写 3 次清理）\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}

/* C 风格手动清理的样子（不要抄，这是反例）： int work(int mode) { FILE* f = fopen("a.txt", "r"); if (!f) return -1; */
