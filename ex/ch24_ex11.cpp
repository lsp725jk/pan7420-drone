// ch24_ex11.cpp —— 例 24-11：const 正确性
// const 不是"给编译器看的装饰"，它是一份**接口契约**：
// "这个函数不会改对象状态"。写对了，调用者敢传 const 对象，编译器也能帮你优化。
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

class FlightLog {
public:
    void append(const char* msg) { entries_.emplace_back(msg); }

        // ---- const 成员函数：承诺"不改对象" ----
        // 没有 const 的话，const FlightLog 对象连 size() 都调不了
    std::size_t size() const { return entries_.size(); }

        // ---- 返回 const 引用：让调用者读，但不让它改内部数据 ----
    const std::string& at(std::size_t i) const { return entries_.at(i); }

        // ---- 返回非 const 引用：只有非 const 对象才能调 ----
    std::string& mutableAt(std::size_t i) { return entries_.at(i); }

        // ---- const 成员函数里的"计数器"：mutable ----
        // 逻辑上不算"修改对象状态"的东西（缓存、计数、互斥量）可以声明为 mutable
    std::size_t readCount() const {
        ++reads_;  // 没有 mutable 就编不过
        return reads_;
    }

private:
    std::vector<std::string> entries_;
    mutable std::size_t reads_ = 0;
};

// ---- 参数传递：能写 const& 就别写值 ----
void print_log(const FlightLog& log) {  // 只读 -> const&
    std::printf("  日志 %zu 条（这是第 %zu 次读）\n", log.size(), log.readCount());
}

int main() {
    FlightLog log;
    log.append("armed");
    log.append("takeoff");
    log.append("failsafe");

    const FlightLog& cref = log;  // 通过 const 引用看它
    std::printf("通过 const 引用只能调 const 成员函数：\n");
    std::printf("  size() = %zu\n", cref.size());
    std::printf("  at(0)  = %s\n", cref.at(0).c_str());
        // cref.mutableAt(0) = "x";   // 编译错误：const 对象不能调非 const 成员函数
        // cref.append("x");          // 同上

    std::printf("\n非 const 对象什么都能调：\n");
    log.mutableAt(1) = "takeoff(v2)";
    std::printf("  改完 at(1) = %s\n", log.at(1).c_str());

    std::printf("\n打印日志（顺便演示 mutable 计数器）：\n");
    print_log(log);
    print_log(log);

    std::printf("\nconst 正确性的四条实用规则：\n");
    std::printf("  1) 不修改对象状态的成员函数，一律加 const。不加的话，\n");
    std::printf("     const 对象/const 引用就调不了，别人只能去 const_cast，更危险。\n");
    std::printf("  2) 只读参数一律用 const T&（大对象）或 T（小对象，如 int/double）。\n");
    std::printf("  3) 返回内部数据的引用时，确认调用者不该改 -> 返回 const 引用。\n");
    std::printf("  4) mutable 只用于【逻辑上不算状态】的东西：缓存、计数器、mutex。\n");
    std::printf("\n坑 1：const 成员函数返回非 const 指针/引用，等于把门打开了：\n");
    std::printf("   const std::vector<int>& data() const { return v_; }  // 正确\n");
    std::printf("   std::vector<int>& data() const { return v_; }        // 编译错误（好事）\n");
    std::printf("坑 2：const 只是【接口承诺】，const_cast 能绕过它。\n");
    std::printf("   对**本来就是 const 定义**的对象做 const_cast 再写 -> 未定义行为。\n");
    std::printf("坑 3：C 里常见的手工字符串函数没有 const 意识：\n");
    std::printf("   size_t my_strlen(char* s)      // 应该写 const char* s\n");
    const char* ro = "read only";
    std::printf("   本机实测可以这么调：strlen(\"%s\") = %zu（标准库就是 const char*）\n", ro,
                std::strlen(ro));
    return 0;
}
