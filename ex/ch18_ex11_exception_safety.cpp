// ch18_ex11_exception_safety.cpp —— 三种异常安全保证：不抛 / 强保证 / 基本保证
// 编译: check.bat ch18_ex11_exception_safety.cpp c++latest
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <type_traits>

// ---- 故障注入：让"内存不够"这类罕见事件可以被稳定复现 ----
int g_allocBudget = 1000;                 // 还能分配多少个元素
struct InjectedFailure : std::runtime_error {
    InjectedFailure() : std::runtime_error("注入的分配失败 (相当于 bad_alloc)") {}
};

// ================= 反例：只能给"基本保证" =================
class BasicVector {
public:
    void push_back(int v)
    {
        if (g_allocBudget <= 0) throw InjectedFailure{};   // 先"分配"再"改状态"
        --g_allocBudget;
        data_.push_back(v);        // 若这一句之后还有别的修改动作，就会留下半成品状态
    }
    const std::vector<int>& data() const { return data_; }
    std::size_t size() const { return data_.size(); }
private:
    std::vector<int> data_;
};

// ================= 正例：用 copy-and-swap 拿到"强保证" =================
class StrongVector {
public:
        // 关键点 1：先在临时对象上做完全部可能失败的操作
    void push_back(int v)
    {
        StrongVector tmp = *this;                  // 可能抛：拷贝整个容器
        if (g_allocBudget <= 0) throw InjectedFailure{};
        --g_allocBudget;
        tmp.data_.push_back(v);                    // 可能抛：扩容
                // 关键点 2：到这一步已经不可能失败了，才动自己的状态
                // 注意这里不能写 swap(tmp)：成员函数名 swap 会把外层的 swap 全遮住（见 E 段）
        swapWith(tmp);
    }

        // 关键点 3：swap 必须 noexcept，否则强保证链条会断
    void swapWith(StrongVector& o) noexcept { data_.swap(o.data_); }
    const std::vector<int>& data() const { return data_; }
    std::size_t size() const { return data_.size(); }

private:
    std::vector<int> data_;
};

std::string join(const std::vector<int>& v)
{
    std::string s = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i) s += ", ";
        s += std::to_string(v[i]);
    }
    return s + "]";
}

int main()
{
    std::cout << "=== A. 三种保证的定义（背下来）===\n";
    std::cout <<
        "  1. 不抛保证 (nothrow)  : 承诺绝不抛异常，用 noexcept 标注\n"
        "  2. 强保证   (strong)   : 要么成功，要么对象保持原样（事务语义）\n"
        "  3. 基本保证 (basic)    : 失败后对象仍\"有效\"，但状态不确定（可能被改了）\n"
        "  没有任何保证           : 对象坏掉，连析构都不敢调 —— 不可接受\n";

    std::cout << "\n=== B. 强保证演示：失败后内容一字不变 ===\n";
    StrongVector sv{};
    g_allocBudget = 3;
    for (int i = 1; i <= 3; ++i) {
        try { sv.push_back(i * 10); std::cout << "  push " << i * 10 << " 成功 -> " << join(sv.data()) << "\n"; }
        catch (const std::exception& e) { std::cout << "  push " << i * 10 << " 失败: " << e.what() << "\n"; }
    }
    g_allocBudget = 0;                              // 关掉"内存"
    std::cout << "  --- 现在把分配额度用光 ---\n";
    std::cout << "  失败前 : " << join(sv.data()) << "  (size=" << sv.size() << ")\n";
    try { sv.push_back(999); }
    catch (const std::exception& e) { std::cout << "  push 999 失败: " << e.what() << "\n"; }
    std::cout << "  失败后 : " << join(sv.data()) << "  (size=" << sv.size() << ")"
              << "   <- size 和内容都没变，这就是强保证\n";
    std::cout << "    (swapWith 内部是 data_.swap(o.data_)，std::vector 的成员 swap\n"
                 "     对相同分配器是 noexcept，所以整个提交动作不会失败)\n";
    std::cout << "\n=== C. 同一个操作，在 std::vector 上也是强保证 ===\n";
    std::vector<std::string> vs{"a", "b"};
    std::size_t capBefore = vs.capacity();
    try {
        vs.push_back(std::string(1000, 'x'));       // 触发扩容 + 大拷贝
        std::cout << "  push 成功, size=" << vs.size() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "  失败: " << e.what() << "\n";
    }
    std::cout << "  capacity: " << capBefore << " -> " << vs.capacity()
              << "（标准保证 push_back 对 vector 是强保证）\n";

    std::cout << "\n=== D. 基本保证长什么样：失败后状态不确定 ===\n";
    BasicVector bv;
    g_allocBudget = 2;
    bv.push_back(1);
    bv.push_back(2);
    std::cout << "  成功两次 -> " << join(bv.data()) << "\n";
    g_allocBudget = 0;
    try { bv.push_back(3); }
    catch (const std::exception& e) {
        std::cout << "  第三次失败: " << e.what() << "\n";
        std::cout << "  失败后 -> " << join(bv.data())
                  << "  <- 这次凑巧没变，但标准**不承诺**它不变\n";
        std::cout << "        真实代码里你可能看到 size 被 +1、或最后一个元素被改了一半\n";
    }
    std::cout << "  嵌入式提醒：任何被 ISR 读到的结构，都不要接受\"基本保证\"，\n"
                 "              要么强保证，要么用\"双缓冲 + 原子指针切换\"。\n";

    std::cout << "\n=== E. 两个真实踩过的坑（都在写这个例子时踩了）===\n";
    std::cout <<
        "  坑 1：临时对象的类型必须是\"包装类\"本身，不能是它的成员类型\n"
        "    错误写法：std::vector<int> tmp = data_;   ...  swapWith(tmp);\n"
        "    实测报错（C2664，参数类型不匹配）：\n"
        "      void StrongVector::swapWith(StrongVector&) noexcept:\n"
        "      无法将参数 1 从 std::vector<int> 转换为 StrongVector&\n"
        "      note: 非常量引用只能绑定到左值\n"
        "    原因：tmp 是 vector，而 swapWith 要的是包装类；vector 无法隐式转成包装类引用。\n"
        "    修法：写 StrongVector tmp = *this;（拷整个包装对象）\n"
        "  坑 2：成员函数名一旦叫 swap，类作用域里这个名字就会遮住外层的 std::swap，\n"
        "        在类内部写裸 swap(x) 会只匹配到成员那一个。本例把成员改名 swapWith 避开。\n"
        "        若坚持叫 swap，调用处要写全 ::std::swap(...)，或把 swap 写成自由函数靠 ADL 找。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
