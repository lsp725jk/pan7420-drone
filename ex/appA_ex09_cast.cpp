// 附录A 探针 9：类型转换对照（C 风格强转 vs 四种 C++ 转换）
#include <cstdio>
#include <cstdint>
#include <memory>
#include <string>

struct Base {
    int b = 1;
    virtual ~Base() = default;          // 有多态才能 dynamic_cast
    virtual const char* who() const { return "Base"; }
};
struct Derived : Base {
    int d = 2;
    const char* who() const override { return "Derived"; }
};
struct Other { int o = 3; };

// 供 const_cast 演示：这个函数拿到的是"逻辑上不该改"的引用
void legacyApi(int* p) { *p += 1; }

int main() {
    std::puts("=== 1. static_cast：编译期就能确定的转换 ===");
    double pi = 3.99;
    int i = static_cast<int>(pi);                     // 有窄化，明确写出来
    int back = static_cast<int>(static_cast<double>(7) / 2);   // 整数除法 -> 浮点除法 -> 3
    Base* pb = new Derived();
    Derived* pd = static_cast<Derived*>(pb);          // 向下转，编译器不检查
    std::printf("  static_cast<int>(3.99)=%d  7/2 转 double 再转回=%d\n", i, back);
    std::printf("  static_cast<Derived*>(Base*) 的 who()=%s（这里恰好对，但编译器没验证过）\n", pd->who());

    std::puts("=== 2. dynamic_cast：运行期检查，失败返回 nullptr（指针）或抛 bad_cast（引用）===");
    Base* real = new Derived();
    Base* plain = new Base();
    std::printf("  真 Derived -> Derived*: %s\n",
                dynamic_cast<Derived*>(real) ? "成功" : "失败(nullptr)");
    std::printf("  真 Base    -> Derived*: %s\n",
                dynamic_cast<Derived*>(plain) ? "成功" : "失败(nullptr)");
    try {
        Base& rb = *plain;
        (void)dynamic_cast<Derived&>(rb);
    } catch (const std::bad_cast&) {
        std::puts("  对引用做 dynamic_cast 失败会抛 std::bad_cast（已捕获）");
    }
    delete real; delete plain;

    std::puts("=== 3. const_cast：只用来去掉/加上 const，不能改变对象本身是不是 const ===");
    int x = 10;
    const int& cr = x;
    legacyApi(const_cast<int*>(&cr));                 // 对象本身不是 const，安全
    std::printf("  对象本身非 const: x=%d（改成功了）\n", x);

    const int y = 20;
    int* py = const_cast<int*>(&y);
    std::printf("  对象本身是 const 时写它 = UB。这里只读不写: *py=%d\n", *py);
    std::printf("  （真去写常量对象属于未定义行为，见附录 B）\n");

    std::puts("=== 4. reinterpret_cast：只重新解释二进制位，不做任何转换 ===");
    float f = 1.0f;
    std::uint32_t bits = *reinterpret_cast<std::uint32_t*>(&f);
    std::printf("  1.0f 的位模式 = 0x%08X（IEEE-754）\n", bits);
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(&x);
    std::printf("  int* -> uintptr_t -> 再转回来是否还原: %s\n",
                reinterpret_cast<int*>(addr) == &x ? "是" : "否");
    std::printf("  char* 逐字节看 int x=%d: ", x);
    for (std::size_t k = 0; k < sizeof(int); ++k)
        std::printf("%02X ", reinterpret_cast<unsigned char*>(&x)[k]);
    std::printf("（本机小端：低位在低地址）\n");

    std::puts("=== 5. C 风格强转为什么危险 ===");
    std::puts("  (T)expr 会依次尝试 const_cast / static_cast / reinterpret_cast，");
    std::puts("  也就是说它可能悄悄做掉 reinterpret_cast 这种最危险的动作，");
    std::puts("  在源码里看不出来。所以 C++ 里应当写清楚是哪一种。");

    std::puts("=== 6. 隐式转换：不一定需要写转换运算符 ===");
    short sh = 100;
    int   ii = sh;                    // 整型提升，隐式且安全
    double dd = ii;                   // 隐式，安全
    std::printf("  short->int->double 隐式: %d %.1f\n", ii, dd);
        // 反过来就要显式，否则 C4244 警告：
    int narrow = static_cast<int>(dd);

    std::puts("=== 7. 指针转换：void* 与继承关系 ===");
    Derived dobj;
    Base* b2 = &dobj;                       // 隐式向上转
    void* vp = b2;                          // 任意对象指针 -> void* 隐式
    Base* b3 = static_cast<Base*>(vp);      // void* -> T* 必须显式
    std::printf("  向上转、void* 往返都正常: who=%s\n", b3->who());

    std::puts("=== 8. 转换与智能指针 ===");
    std::unique_ptr<Base> ub = std::make_unique<Derived>();
    std::unique_ptr<Derived> ud(static_cast<Derived*>(ub.release()));  // 手动移交
    std::printf("  unique_ptr 之间要用 release()+构造或 std::move，不能直接转\n");
    std::printf("  ud->who()=%s\n", ud->who());
    return 0;
}
