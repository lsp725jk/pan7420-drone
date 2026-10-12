// ch24_ex04.cpp —— 例 24-4：单例（Singleton），以及它的正确写法
// 场景：整个固件只有一份配置、一份日志输出、一份传感器总线。
// 单例 = 保证"全局只有一个实例"，并提供全局访问点。
#include <cstdio>
#include <string>

// ==================================================================
// 写法 A：Meyers Singleton（C++11 起最推荐的写法）
// - 函数内 static：第一次调用时构造，程序结束时自动析构
// - C++11 起保证线程安全的初始化（编译器插入一次性守卫）
// - 不会遇到"静态初始化顺序"问题，因为它是**第一次使用时**才构造
// ==================================================================
class Config {
public:
    static Config& instance() {
        static Config c;  // 只构造一次
        return c;
    }

    int loopHz = 1000;
    std::string board = "PY32F030";

    Config(const Config&) = delete;             // 禁止拷贝
    Config& operator=(const Config&) = delete;  // 禁止赋值

private:
    Config() { std::printf("[Config] 被构造了（只会出现一次）\n"); }
};

// ==================================================================
// 写法 B：C 风格的全局变量 + 初始化函数（对照）
// ==================================================================
struct LegacyConfig {
    int loopHz;
    int inited;
};
LegacyConfig g_legacy;  // 零初始化：loopHz = 0，直到有人调用 init

void legacy_init() {
    g_legacy.loopHz = 1000;
    g_legacy.inited = 1;
}

// ==================================================================
// 写法 C：只读单例（编译期常量，连函数调用都省了）
// ==================================================================
struct Limits {
    static constexpr int kMaxThrottle = 100;
    static constexpr int kMinThrottle = 0;
    static constexpr double kPi = 3.14159265358979323846;
};

int main() {
    std::printf("legacy 未初始化时 loopHz = %d（危险的 0）\n", g_legacy.loopHz);
    legacy_init();
    std::printf("legacy 初始化后 loopHz = %d\n", g_legacy.loopHz);

    std::printf("\nMeyers 单例：\n");
    std::printf("  Config::instance().board = %s\n", Config::instance().board.c_str());
    std::printf("  Config::instance().loopHz = %d\n", Config::instance().loopHz);
    Config::instance().loopHz = 500;  // 改一处，处处生效
    std::printf("  改完之后再读 = %d\n", Config::instance().loopHz);
    std::printf("  两次 instance() 是同一个对象: %d\n",
                (int)(&Config::instance() == &Config::instance()));

    std::printf("\n只读单例（编译期常量）：\n");
    std::printf("  Limits::kMaxThrottle = %d（连函数调用都没有，直接是立即数）\n",
                Limits::kMaxThrottle);
    static_assert(Limits::kMaxThrottle == 100, "");

    std::printf("\n为什么单例经常被批评（三条）：\n");
    std::printf("  1) 它是**伪装的全局变量**：任何函数都能悄悄依赖它，\n");
    std::printf("     依赖关系不会出现在函数签名里，改起来牵一发动全身。\n");
    std::printf("  2) 难测试：单元测试想给一个假配置，却没法替换单例内部的对象。\n");
    std::printf("     （对比：构造注入就很容易换，见例 24-10）\n");
    std::printf("  3) 生命周期含糊：析构顺序、多线程下的初始化时机都要想清楚。\n");
    std::printf("\n结论：\n");
    std::printf("  - **只读的、真正的全局常量** -> 放心用单例或 constexpr\n");
    std::printf("  - **可变的、会被替换的依赖** -> 用依赖注入（构造函数传进去）\n");
    std::printf("  - 嵌入式上还有一种更好的做法：模块内部用文件级 static，\n");
    std::printf("    通过一组 init()/get() 函数暴露，别做成全局可访问的类。\n");
    std::printf("\n坑：不要写 new 出来的单例（getInstance 里 new 一个），\n");
    std::printf("   它永远不会析构，而且离开作用域后指针还在，是多线程的雷。\n");
    return 0;
}
