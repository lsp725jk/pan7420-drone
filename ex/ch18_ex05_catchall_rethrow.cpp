// ch18_ex05_catchall_rethrow.cpp —— catch(...) 与重新抛出，以及 RAII 为什么是异常安全的地基
// 编译: check.bat ch18_ex05_catchall_rethrow.cpp c++latest
#include <iostream>
#include <stdexcept>
#include <string>

// ---------- 工具：把"构造了几次、析构了几次"记下来 ----------
struct Counter {
    static int ctor, dtor;
    std::string name;
    explicit Counter(std::string n) : name(std::move(n)) { ++ctor; }
    ~Counter() { ++dtor; }
    static void reset() { ctor = 0; dtor = 0; }
    static void report(const char* title) {
        std::cout << "    " << title << ": 构造 " << ctor
                  << " 次, 析构 " << dtor << " 次, 差 = "
                  << (ctor - dtor) << "   <- 0 表示没有泄漏\n";
    }
};
int Counter::ctor = 0;
int Counter::dtor = 0;

// ---------- 1) catch(...) 只做记录，再 throw; 原样往上抛 ----------
void deepThrow()
{
    throw std::out_of_range("来自 deepThrow");
}

void rethrowDemo()
{
    try {
        deepThrow();
    }
    catch (...) {                       // 先兜住，做点记录
        std::cout << "  [中间层] catch(...) 到了，但我不知道怎么处理\n";
        throw;                          // 原样再抛（对比：throw e; 会改变类型）
    }
}

// ---------- 2) 反例：用 throw e; 重新抛出会"切掉"类型 ----------
void rethrowChangesType()
{
    try {
        throw std::out_of_range("原始 out_of_range");
    }
    catch (const std::exception& e) {
        std::cout << "  [反例] 内层捕获到: " << e.what() << "\n";
        throw e;                        // ✘ 按值 throw：对象被切片成 std::exception 再抛
    }
}

// ---------- 3) RAII 版本：无论怎么退出，都不会漏 ----------
void raiiVersion(bool doThrow)
{
    Counter::reset();
    Counter c("raii-counter");
    if (doThrow) {
        Counter::report("抛异常时（在函数体内）");
        throw std::runtime_error("raii 路径里出错");
    }
    Counter::report("正常返回前");
}

// ---------- 4) 手动版本：异常一飞，资源就漏了 ----------
void manualVersion(bool doThrow)
{
    Counter::reset();
    Counter* raw = new Counter("manual-counter");   // 裸指针，没有 RAII
    if (doThrow) {
        throw std::runtime_error("manual 路径里出错");  // ← 这一句直接漏掉 raw
    }
    delete raw;
    Counter::report("正常返回前");
}

int main()
{
    std::cout << "=== A. catch(...) 只负责记录，然后 throw; 继续往上 ===\n";
    try {
        rethrowDemo();
    }
    catch (const std::out_of_range& e) {
        std::cout << "  [最外层] 终于处理: " << e.what() << "\n";
        std::cout << "  注意：异常类型一路保住了 out_of_range\n";
    }

    std::cout << "\n=== B. 反例：throw e;（按值）会把类型切掉 ===\n";
    try {
        rethrowChangesType();
    }
    catch (const std::out_of_range&) {
        std::cout << "  没被子类捕获 —— 因为类型已经丢了\n";
    }
    catch (const std::exception& e) {
        std::cout << "  被基类捕获: " << e.what()
                  << "   <- 类型退化成 std::exception\n";
    }

    std::cout << "\n=== C. RAII 版本：抛异常路径上析构照样被调用 ===\n";
    try {
        raiiVersion(true);
        std::cout << "  这行不会打印\n";
    }
    catch (const std::exception& e) {
        std::cout << "  catch 到: " << e.what()
                  << "（此时局部对象已经析构完了）\n";
    }
    Counter::report("异常飞出去之后");

    std::cout << "\n=== D. 对比：手动 delete 的版本，异常路径漏掉了对象 ===\n";
    try {
        manualVersion(true);
    }
    catch (const std::exception& e) {
        std::cout << "  catch 到: " << e.what() << "\n";
    }
    Counter::report("异常飞出去之后");

    std::cout << "\nmain 正常结束\n";
    return 0;
}
