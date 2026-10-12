// ch14_ex08.cpp —— auto_ptr 为什么被废弃：拷贝会"偷走"原对象，且与 STL 容器不兼容
// 说明：auto_ptr 已在 C++17 被标准移除，本机 MSVC 用 /std:c++latest 无法直接编译它。
// 这里用"自己写的语义等价简化版 AutoPtr"来复现它的行为。
#include <cstdio>
#include <memory>
#include <utility>
#include <vector>

// ---- 模仿 std::auto_ptr 语义的简化版（1:1 复刻它的两个致命设计）----
template <class T>
class AutoPtr {
public:
    explicit AutoPtr(T* p = nullptr) : p_(p) {}
        // 致命设计一：拷贝构造的参数**不是 const 引用**，而且会把源对象掏空
    AutoPtr(AutoPtr& o) : p_(o.p_) { o.p_ = nullptr; }
    AutoPtr& operator=(AutoPtr& o) {
        if (this != &o) { delete p_; p_ = o.p_; o.p_ = nullptr; }
        return *this;
    }
        // 致命设计二：没有移动构造（auto_ptr 那年代还没有移动语义）
    AutoPtr(AutoPtr&&) = delete;

    ~AutoPtr() { delete p_; }
    T* get() const { return p_; }
    T* operator->() const { return p_; }
    T& operator*() const { return *p_; }
private:
    T* p_;
};

struct Res {
    int id;
    explicit Res(int i) : id(i) { std::printf("  Res(%d) 构造\n", id); }
    ~Res() { std::printf("  Res(%d) 析构\n", id); }
};

// 一个"看起来无害"的函数：按值接收，实际上把调用者的指针偷走了
void consume(AutoPtr<Res> p)
{
    std::printf("  consume 内部 p.get() = %p\n", static_cast<void*>(p.get()));
}

int main()
{
    std::printf("--- 1) 拷贝构造会掏空源对象 ---\n");
    {
        AutoPtr<Res> a(new Res(1));
        AutoPtr<Res> b(a);                       // 看起来是"拷贝"，其实是"搬家"
        std::printf("  a.get() = %p   b.get() = %p\n",
                    static_cast<void*>(a.get()), static_cast<void*>(b.get()));
        std::printf("  a 已被掏空：后面再写 a->id 就是空指针解引用\n");
    }

    std::printf("--- 2) 传参被偷走 ---\n");
    {
        AutoPtr<Res> a(new Res(2));
        consume(a);
        std::printf("  调用 consume(a) 之后 a.get() = %p\n", static_cast<void*>(a.get()));
        std::printf("  一次普通的函数调用就把资源弄丢了，而且编译器一声不吭\n");
    }

    std::printf("--- 3) 放进容器：连 reserve 都会编译失败 ---\n");
    {
                // 下面这段是**真实编译错误**（本机 MSVC 14.51 实测）：
                // std::vector<AutoPtr<Res>> v;
                // v.reserve(4);
                // v.emplace_back(new Res(3));
                // 报错摘要：
                // error C2672: “std::construct_at”: 未找到匹配的重载函数
                // note: “AutoPtr<Res>::AutoPtr(AutoPtr<Res> &&)”: 尝试引用已删除的函数
                // note: 查看对正在编译的函数 模板 实例化
                // “std::_Uninitialized_move<...>”的引用
                // 原因：vector 在分配新空间时要"搬"已有元素，而 AutoPtr
                // 既不能拷贝（拷贝构造参数不是 const&）也不能移动 -> 无法实例化。
        
                // 换成一个不会搬元素的容器（固定大小数组）就又能用了：
        AutoPtr<Res> slots[2] = { AutoPtr<Res>(new Res(3)), AutoPtr<Res>(new Res(4)) };
        std::printf("  用数组装：slot0.id=%d, slot1.id=%d\n", slots[0]->id, slots[1]->id);
        std::printf("  只有「从不搬元素」的容器才敢用 auto_ptr —— 这条件太苛刻了\n");
    }

    std::printf("--- 4) 正确写法：unique_ptr 删掉拷贝，只留移动 ---\n");
    {
        auto a = std::make_unique<Res>(6);
                // auto b = a;                  // 编译错误 error C2280: 尝试引用已删除的函数
        auto b = std::move(a);          // 必须明写 move，所有权转移写在脸上
        std::printf("  move 之后 a.get() = %p  b.get() = %p\n",
                    static_cast<void*>(a.get()), static_cast<void*>(b.get()));

        std::vector<std::unique_ptr<Res>> v;
        v.push_back(std::make_unique<Res>(7));
        v.push_back(std::move(b));      // 放进容器也要明写 move
        std::printf("  vector 大小 = %zu，元素 id = %d\n", v.size(), v[1]->id);
    }

    std::printf("main 结束\n");
    return 0;
}
