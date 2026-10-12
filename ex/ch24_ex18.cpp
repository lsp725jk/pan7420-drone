// ch24_ex18.cpp —— 例 24-18：Sanitizer（净化器）简介
// Sanitizer 是编译器插进去的"运行时检查器"：程序照常跑，但内存越界、
// 未定义行为、数据竞争会被当场抓住并打印调用栈。
// 代价是慢 2~20 倍，所以只在**测试**时开，发布不开。

// 本机（MSVC 19.51）实测支持的开关：
// cl /fsanitize=address  ...    AddressSanitizer（ASan）
// MSVC 目前**没有** UBSan 和 TSan（GCC/Clang 有），下面会说明。
#include <cstdio>
#include <cstring>
#include <vector>

// ==================================================================
// 各类 Sanitizer 各查什么
// ==================================================================
// ASan (AddressSanitizer)    —— 内存错误
// · 堆缓冲区越界读写（malloc 的块）
// · 栈缓冲区越界（局部数组）
// · 使用已释放的内存（use-after-free）
// · 重复释放（double-free）
// · 内存泄漏（LeakSanitizer，GCC/Clang 默认附带）

// UBSan (UndefinedBehaviorSanitizer) —— 未定义行为
// · 有符号整数溢出
// · 除以零
// · 空指针解引用
// · 移位超过位宽
// · 类型双关违规、对齐错误
// · 数组下标越界（仅静态数组）

// TSan (ThreadSanitizer)     —— 数据竞争
// · 两个线程无同步地访问同一内存且至少一个写
// · 死锁（部分检测）

// MSan (MemorySanitizer)     —— 未初始化内存读取（GCC/Clang）

int* make_leak() {
    int* p = new int[10];
    p[0] = 1;
    return p;  // 调用者如果忘了 delete[]，LeakSanitizer 会报
}

void demo_stack_ok() {
    int buf[4] = {1, 2, 3, 4};
    std::printf("  正常访问 buf[3] = %d\n", buf[3]);
}

// 下面这几个函数**故意**是错的，只在开了 ASan 时才会被抓住，
// 所以这里用宏包起来，默认不启用。
#ifdef ENABLE_ASAN_DEMO
void demo_heap_overflow() {
    int* p = new int[4];
    p[4] = 42;  // 越界写！ASan 会立刻报 heap-buffer-overflow
    delete[] p;
}

void demo_use_after_free() {
    int* p = new int(7);
    delete p;
    std::printf("  %d\n", *p);  // use-after-free
}
#endif

int main() {
    std::printf("=== 正常代码：Sanitizer 不会报任何东西 ===\n");
    demo_stack_ok();

    std::vector<int> v{1, 2, 3};
    v.push_back(4);
    std::printf("  vector 大小 %zu，最后一个 %d\n", v.size(), v.back());

    int* p = make_leak();
    std::printf("  make_leak() 返回的第一个值 = %d\n", p[0]);
    delete[] p;  // 如果注释掉这一行，ASan/LSan 会在退出时报泄漏

    std::printf("\n=== 各 Sanitizer 查什么（对照表）===\n");
    std::printf("  内存越界/释放后使用/重复释放 -> ASan  （MSVC: /fsanitize=address）\n");
    std::printf("  有符号溢出/除零/空指针/移位  -> UBSan （GCC/Clang: -fsanitize=undefined）\n");
    std::printf("  多线程数据竞争/死锁          -> TSan  （GCC/Clang: -fsanitize=thread）\n");
    std::printf("  读未初始化内存               -> MSan  （Clang: -fsanitize=memory）\n");

    std::printf("\n怎么用（本机实测）：\n");
    std::printf("  cl /nologo /utf-8 /std:c++latest /EHsc /fsanitize=address 例子.cpp\n");
    std::printf("  然后正常运行 exe，出错时会打印：\n");
    std::printf("    =================================================================\n");
    std::printf("    ==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address ...\n");
    std::printf("        #0 0x... in demo_heap_overflow ...\\ch24_ex18.cpp:52\n");
    std::printf("    （带完整的调用栈和越界位置，比崩溃后猜快一百倍）\n");

    std::printf("\n嵌入式上能用吗？\n");
    std::printf("  - 单测在 PC 上跑：能，而且非常推荐。把驱动逻辑抽出来，\n");
    std::printf("    用假硬件在 PC 上跑 ASan/UBSan 单测，比在板子上抓 HardFault 快得多。\n");
    std::printf("  - 直接在 MCU 上跑：基本不行（代码体积和 RAM 都吃不消）。\n");
    std::printf("    MCU 上的替代品：\n");
    std::printf("      · MPU（内存保护单元）把非法访问变成 HardFault\n");
    std::printf("      · 栈哨兵/栈填充法检查栈溢出\n");
    std::printf("      · 在自己的 malloc 包装里加边界校验\n");
    std::printf("      · 断言 + 日志，把现场存到 Flash 里事后分析\n");

    std::printf("\n什么时候开：\n");
    std::printf("  - 平时开发/CI 跑测试时：开着（/fsanitize=address）\n");
    std::printf("  - 发布版本：关掉（慢，而且需要额外的运行时库）\n");
    std::printf("  - 第一次跑通新模块：一定要开着跑一遍\n");
    std::printf("\n想现场看 ASan 报错长什么样：\n");
    std::printf("  编译时加 /DENABLE_ASAN_DEMO /fsanitize=address，\n");
    std::printf("  然后在 main 里调用 demo_heap_overflow() 或 demo_use_after_free()。\n");
    return 0;
}
