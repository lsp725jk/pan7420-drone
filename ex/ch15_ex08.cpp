// ch15_ex08.cpp —— 万能引用 T&& 与引用折叠（reference collapsing）
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>

// 把"推导出来的 T"翻译成人能看懂的文字
const char* refname_rvalue = "&&";
const char* refname_lvalue = "&";
const char* refname_none   = "";

template <class T>
const char* ref_suffix()
{
    if (std::is_lvalue_reference_v<T>) return refname_lvalue;
    if (std::is_rvalue_reference_v<T>) return refname_rvalue;
    return refname_none;
}

template <class T>
void describe(const char* given, const char* tname)
{
    using NoRef = std::remove_reference_t<T>;
    const bool isConst = std::is_const_v<NoRef>;
    std::printf("  传入 %-18s -> T 推导为 %s%s%s  ->  T&& 折叠为 %s%s%s\n",
                given,
                isConst ? "const " : "", tname, ref_suffix<T>(),
                isConst ? "const " : "", tname, ref_suffix<T&&>());
}

// ---- 万能引用（forwarding reference）：模板参数 T&& ----
template <class T>
void forwarder(T&& arg)
{
    (void)arg;
    std::printf("  [forwarder] T&& 是左值引用? %d   是右值引用? %d\n",
                std::is_lvalue_reference_v<T&&>, std::is_rvalue_reference_v<T&&>);
}

// ---- 只是右值引用：不是模板参数，就是普通右值引用 ----
void rvalue_only(int&& arg)
{
    std::printf("    rvalue_only 收到 %d\n", arg);
}

int main()
{
    std::printf("--- 1) T&& 在模板里对左值和右值都能绑 ---\n");
    int       lv  = 1;
    const int clv = 2;
    std::printf("  传左值 lv：\n");        forwarder(lv);
    std::printf("  传 const 左值 clv：\n"); forwarder(clv);
    std::printf("  传右值 3：\n");          forwarder(3);
    std::printf("  传 std::move(lv)：\n");  forwarder(std::move(lv));

    std::printf("--- 2) 推导规则：左值 -> T 推成 U&，然后折叠 ---\n");
    describe<int&>("lv（int 左值）", "int");
    describe<const int&>("clv（const 左值）", "int");
    describe<int>("3（int 右值）", "int");
    describe<int>("std::move(lv)", "int");

    std::printf("--- 3) 引用折叠规则表（只有一种情况是右值引用）---\n");
    std::printf("    T&  &   -> T&\n");
    std::printf("    T&  &&  -> T&\n");
    std::printf("    T&& &   -> T&\n");
    std::printf("    T&& &&  -> T&&   <-- 只有这个才是右值引用\n");
    std::printf("  记忆口诀：只要出现一个 &，结果就是 &（左值引用更容易「赢」）\n");

    std::printf("--- 4) 没有模板参数就不是万能引用 ---\n");
    int n = 5;
    rvalue_only(7);           // OK：右值
        // rvalue_only(n);        // 编译错误：无法将右值引用绑定到左值
    std::printf("  rvalue_only(n) 会报错：int&& 只能绑右值，它只对右值「万能」\n");
    std::printf("  n = %d\n", n);

    std::printf("--- 5) 区分「万能引用」和「右值引用」的条件 ---\n");
    std::printf("  是万能引用：template<class T> void f(T&&);\n");
    std::printf("              auto&& x = ...;                （auto 相当于模板参数）\n");
    std::printf("  不是万能引用：void f(int&&);               （没有类型推导）\n");
    std::printf("                template<class T> void f(std::vector<T>&&);  （不是纯 T&&）\n");
    std::printf("                template<class T> void f(const T&&);         （有 const）\n");

    std::printf("--- 6) auto&& 也是万能引用 ---\n");
    {
        auto&& a1 = lv;        // 左值：a1 是 int&
        auto&& a2 = 42;        // 右值：a2 是 int&&（生命期被延长）
        std::printf("  auto&& a1 = lv   -> int& ? %d\n", std::is_lvalue_reference_v<decltype(a1)>);
        std::printf("  auto&& a2 = 42   -> int&& ? %d\n", std::is_rvalue_reference_v<decltype(a2)>);
        std::printf("  a1 = %d, a2 = %d\n", a1, a2);
    }

    return 0;
}
