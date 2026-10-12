// ch24_ex12.cpp —— 例 24-12：值语义 vs 引用语义
// 接口设计里最容易含糊的一件事：这个参数/返回值，到底"属于谁"？
// 三种语义各有对应的写法，选错了就是性能问题或者悬空引用。
#include <cstdio>
#include <string>
#include <vector>

struct Telemetry {
    int roll_cdeg;   // 横滚，0.01 度
    int pitch_cdeg;
    int yaw_cdeg;
    std::string tag;
};

// ==================================================================
// 参数：三种传法怎么选
// ==================================================================

// 1) 按值传（拷贝一份，函数里随便改，不影响调用者）
// 适合：小的、平凡的类型（int/double/指针/小结构体，一般 <= 16 字节）
int clamp_by_value(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// 2) 按 const 引用传（不拷贝、不能改）
// 适合：大的、只读的参数
void print_telemetry(const Telemetry& t) {
    std::printf("    roll=%d pitch=%d yaw=%d tag=%s\n", t.roll_cdeg, t.pitch_cdeg, t.yaw_cdeg,
                t.tag.c_str());
}

// 3) 按非 const 引用/指针传（要修改调用者的对象）
void normalize(Telemetry& t) {
    t.roll_cdeg %= 36000;
    if (t.roll_cdeg < 0) t.roll_cdeg += 36000;
}

// 4) 按值传 + std::move（函数要"拿走"这个对象，调用者不再需要）
void store(std::vector<std::string>& dst, std::string s) {  // 按值接
    dst.push_back(std::move(s));  // 再移进容器：整个链路只有一次拷贝（在调用处）
}

// ==================================================================
// 返回：三种情况
// ==================================================================

// 1) 返回小对象：直接按值，编译器会优化（RVO）
Telemetry make_default() {
    return Telemetry{0, 0, 0, "default"};
}

// 2) 返回大对象：也直接按值，别返回 new 出来的指针
std::vector<int> make_table(int n) {
    std::vector<int> v;
    v.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        v.push_back(i * i);
    }
    return v;  // NRVO / 移动，不会真的拷贝整个 vector
}

// 3) 返回内部成员的引用：必须确认生命周期
class Registry {
public:
    void add(const std::string& s) { items_.push_back(s); }
        // 返回 const 引用：调用者能读，不能改，也不会拷贝
    const std::string& at(std::size_t i) const { return items_.at(i); }
        // 返回 std::string_view 也可以，但要保证底层 string 还活着
    std::size_t size() const { return items_.size(); }

private:
    std::vector<std::string> items_;
};

int main() {
    std::printf("参数传递：\n");
    std::printf("  clamp_by_value(37000, 0, 36000) = %d（按值，不影响外面）\n",
                clamp_by_value(37000, 0, 36000));

    Telemetry t{37000, -500, 20000, "imu0"};
    std::printf("  归一化前: ");
    print_telemetry(t);
    normalize(t);
    std::printf("  归一化后: ");
    print_telemetry(t);

    std::vector<std::string> log;
    std::string s = "armed";
    store(log, std::move(s));  // 明确表示"我不要了"
    std::printf("  store 之后 s = \"%s\"（被搬空，内容长度 %zu）\n", s.c_str(), s.size());
    std::printf("  log[0] = %s\n", log[0].c_str());

    std::printf("\n返回：\n");
    Telemetry d = make_default();
    std::printf("  make_default().tag = %s\n", d.tag.c_str());
    auto tbl = make_table(5);
    std::printf("  make_table(5) 大小 = %zu，[4] = %d\n", tbl.size(), tbl[4]);

    Registry reg;
    reg.add("first");
    reg.add("second");
    const std::string& ref = reg.at(0);  // 不拷贝
    std::printf("  reg.at(0) = %s（返回 const 引用，零拷贝）\n", ref.c_str());

    std::printf("\n一句口诀：\n");
    std::printf("  进来的大对象看情况：只读用 const&，要改就 &，要拿走就按值 + move\n");
    std::printf("  出去的大对象：直接按值返回，让编译器做 RVO\n");
    std::printf("  绝不返回指向局部变量的引用/指针\n");
    std::printf("\n坑 1：const& 接了临时对象没问题（生命周期延长），\n");
    std::printf("   但把成员声明成 const& 接构造参数，就会留下悬空引用：\n");
    std::printf("     struct S { const std::string& name; };   // 危险！\n");
    std::printf("     S s{std::string(\"tmp\")};  // 临时对象活不过这一行\n");
    std::printf("   要存下来就按值存，或者用 std::string_view 并自己保证生命周期。\n");
    std::printf("坑 2：返回 vector/string 的引用时，里面一旦扩容/修改，引用就可能失效。\n");
    std::printf("sizeof(Telemetry) = %zu 字节（含 std::string，属于【大对象】）\n", sizeof(Telemetry));
    return 0;
}
