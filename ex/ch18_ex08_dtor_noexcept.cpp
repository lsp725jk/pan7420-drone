// ch18_ex08_dtor_noexcept.cpp —— 析构函数为什么默认不许抛异常
// 编译: check.bat ch18_ex08_dtor_noexcept.cpp c++latest
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <string>

// ---------- A. 编译器给析构函数加的"隐形 noexcept" ----------
struct Plain {
    ~Plain() {}                       // 没写任何东西
};
struct WithThrowableMember {
    std::string s;                    // std::string 的析构是 noexcept
    ~WithThrowableMember() {}
};
struct ExplicitlyThrowy {
    ~ExplicitlyThrowy() noexcept(false) {}   // 显式声明"我可能抛"
};

// ---------- B. 析构里抛异常，但只要不飞出析构函数就没事 ----------
struct Logger {
    bool bad;
    explicit Logger(bool b) : bad(b) {}
    ~Logger() noexcept {
        try {
            if (bad) throw std::runtime_error("析构里发现的错误");
            std::cout << "    ~Logger: 正常收尾\n";
        }
        catch (const std::exception& e) {
                        // 关键：就地吃掉，一个字节都不许飞出去
            std::cout << "    ~Logger: 捕获并吞掉 -> " << e.what() << "\n";
        }
    }
};

// ---------- C. 正确姿势：真正会失败的收尾动作，交给用户显式调用 close() ----------
class FileLike {
public:
    explicit FileLike(std::string n) : name_(std::move(n)), open_(true) {
        std::cout << "    open " << name_ << "\n";
    }

        // 显式收尾：允许抛异常，因为它是普通函数
    void close() {
        if (!open_) return;
        open_ = false;
        std::cout << "    close() 被显式调用: " << name_ << "\n";
        if (name_ == "bad.dat") {
            throw std::runtime_error("close 时刷盘失败: " + name_);   // 可以抛！
        }
    }

        // 兜底：析构时如果还没关，只能尽力而为，绝不抛
    ~FileLike() noexcept {
        if (open_) {
            open_ = false;
            std::cout << "    ~FileLike 兜底关闭 " << name_ << "（错误只能丢弃）\n";
        }
    }

private:
    std::string name_;
    bool open_;
};

int main()
{
    std::cout << "=== A. 析构函数的 noexcept 是编译器自动加的 ===\n";
    std::cout << "  noexcept(Plain{})                   = "
              << noexcept(Plain{}) << "\n";
    std::cout << "  noexcept(WithThrowableMember{})     = "
              << noexcept(WithThrowableMember{}) << "\n";
    std::cout << "  noexcept(ExplicitlyThrowy{})        = "
              << noexcept(ExplicitlyThrowy{}) << "   <- 显式 noexcept(false) 才为 0\n";
    std::cout << "  is_nothrow_destructible_v<std::string> = "
              << std::is_nothrow_destructible_v<std::string> << "\n";

    std::cout << "\n=== B. 析构里出了错：就地 try/catch 吞掉，程序继续跑 ===\n";
    {
        std::cout << "  进入作用域\n";
        Logger a(false);
        Logger b(true);              // 这个会在析构时报错，但只是打印
        std::cout << "  离开作用域（析构顺序：b 先，a 后）\n";
    }
    std::cout << "  程序安然无恙地走到这里\n";

    std::cout << "\n=== C. 推荐的分离式设计：close() 会抛，析构只兜底 ===\n";
    {
        FileLike f("good.dat");
        f.close();                     // 显式收尾，错误能上报
    }
    {
        FileLike f("good2.dat");
                // 故意不 close()：析构兜底，且绝不抛
    }
    try {
        FileLike f("bad.dat");
        f.close();                     // 这一句会抛，调用者能处理
    }
    catch (const std::exception& e) {
        std::cout << "  catch 到 close 的错误: " << e.what() << "\n";
    }

    std::cout << "\nmain 正常结束\n";
    return 0;
}
