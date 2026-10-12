// ch21_ex07_heap_fragmentation.cpp —— 裸机上的 new/delete：堆碎片是怎么来的
// 编译: check.bat ch21_ex07_heap_fragmentation.cpp c++latest
// 说明：PC 上有虚拟内存和操作系统，堆很难真的碎掉。
// 这里用一个"模拟裸机堆"的模型演示碎片的产生，结论对着 PY32F030（8KB RAM）说。
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

// ============================================================
// 模拟一个固定大小的裸机堆（比如 256 字节），带简单的首次适配分配器
// ============================================================
constexpr std::size_t kHeapSize = 256;

struct Block {
    std::size_t offset;      // 在堆中的起始偏移
    std::size_t size;        // 块大小（含头部）
    bool        used;
};

class SimHeap {
public:
    SimHeap()
    {
        buf_.fill(0);
        blocks_.push_back({0, kHeapSize, false});
    }

        // 首次适配（first fit）：找第一个够大的空闲块
    void* allocate(std::size_t n, const char* who)
    {
        const std::size_t need = n + 8;              // 8 字节头部（简化）
        for (std::size_t i = 0; i < blocks_.size(); ++i) {
            if (!blocks_[i].used && blocks_[i].size >= need) {
                                // 切成"已用 + 剩余空闲"
                if (blocks_[i].size > need) {
                    Block rest{blocks_[i].offset + need, blocks_[i].size - need, false};
                    blocks_[i].size = need;
                    blocks_.insert(blocks_.begin() + i + 1, rest);
                }
                blocks_[i].used = true;
                log("分配 " + std::to_string(n) + " 字节 (" + who + ")");
                return buf_.data() + blocks_[i].offset + 8;
            }
        }
        log("分配 " + std::to_string(n) + " 字节 (" + who + ") -> 失败：没有足够大的连续块");
        return nullptr;
    }

    void deallocate(void* p, const char* who)
    {
        if (!p) return;
        const std::size_t off = static_cast<std::size_t>(
            static_cast<std::uint8_t*>(p) - buf_.data()) - 8;
        for (auto& b : blocks_) {
            if (b.offset == off) {
                b.used = false;
                log("释放 (" + std::string(who) + ")");
                coalesce();
                return;
            }
        }
    }

    void dump() const
    {
        std::cout << "    堆布局: [";
        for (const auto& b : blocks_) {
            std::cout << (b.used ? "#" : ".") << b.size;
            if (&b != &blocks_.back()) std::cout << "|";
        }
        std::cout << "]   # = 已用, . = 空闲\n";
    }

    std::size_t largestFree() const
    {
        std::size_t m = 0;
        for (const auto& b : blocks_) if (!b.used) m = std::max(m, b.size);
        return m;
    }

    std::size_t totalFree() const
    {
        std::size_t m = 0;
        for (const auto& b : blocks_) if (!b.used) m += b.size;
        return m;
    }

private:
    void coalesce()                          // 合并相邻空闲块（很多嵌入式分配器不做或做不全）
    {
        for (std::size_t i = 0; i + 1 < blocks_.size();) {
            if (!blocks_[i].used && !blocks_[i + 1].used) {
                blocks_[i].size += blocks_[i + 1].size;
                blocks_.erase(blocks_.begin() + i + 1);
            }
            else ++i;
        }
    }

    void log(const std::string& what) const
    {
        std::cout << "    " << what << "\n";
    }

    std::array<std::uint8_t, kHeapSize> buf_{};
    std::vector<Block> blocks_;
};

int main()
{
    std::cout << "=== A. 碎片是怎么产生的：一个\"存活的小块\"把空闲空间切成两半 ===\n";
    {
        SimHeap h;
        std::cout << "  初始 256 字节全空:\n";
        h.dump();

        auto* a = h.allocate(64, "A");
        auto* b = h.allocate(64, "B");
        std::cout << "  分配 A(64) 和 B(64) 之后:\n";
        h.dump();

        h.deallocate(a, "A");
        std::cout << "  释放 A（B 还活着，它把左边那块和右边那块隔开了）:\n";
        h.dump();
        std::cout << "    空闲总量 = " << h.totalFree()
                  << " 字节，最大连续空闲块 = " << h.largestFree() << " 字节\n";

        std::cout << "  现在申请 120 字节:\n";
        void* big = h.allocate(120, "C");
        std::cout << "    -> " << (big ? "成功" : "**失败**")
                  << "   （空闲 184 字节却要不到 120 字节！）\n";
        std::cout << "  原因：最大连续空闲块只有 112 字节，不够 120；\n";
        std::cout << "        而 malloc 只会切分**单独**一个块，不会把不相邻的空闲块拼起来。\n";
        std::cout << "  这就是**堆碎片（heap fragmentation）**：\n";
        std::cout << "    空闲内存总量足够，但没有一块连续空间能满足请求。\n";
        std::cout << "  注意：本例的分配器**会**合并相邻空闲块（coalesce），\n";
        std::cout << "        但仍然救不了\"被存活块隔开\"的两块内存。\n";
        h.deallocate(b, "B");
        std::cout << "  再把 B 也释放（现在相邻了，合并发生）:\n";
        h.dump();
        std::cout << "    空闲总量 = " << h.totalFree()
                  << "，最大连续 = " << h.largestFree() << "  -> 又能分配大块了\n";
        std::cout << "  关键：碎片不是\"内存泄漏\"，只要那块还活着，碎片就消不掉。\n";
    }

    std::cout << "\n=== B. 在 8KB RAM 的 PY32F030 上，这件事有多严重 ===\n";
    std::cout << "  假设堆只有 2KB（8KB 里还要放栈、全局变量、DMA 缓冲）：\n";
    std::cout << "    - 每次 new/delete 都要在链表里找块、切块、合并；\n";
    std::cout << "    - 分配失败时 new 抛 std::bad_alloc —— 而我们已经 -fno-exceptions，\n";
    std::cout << "      那就直接 terminate/重启；\n";
    std::cout << "    - 长期运行（飞控要连续跑几小时）后碎片逐渐累积；\n";
    std::cout << "    - 症状是**偶发**的：今天飞 5 分钟没事，明天飞 3 分钟就死机。\n";
    std::cout << "  这就是为什么嵌入式规范通常写：\"禁止动态内存分配\"。\n";
    std::cout << "  注意 MISRA C++ / JSF AV C++ 等航空规范也是明确禁用 new/delete 的。\n";

    std::cout << "\n=== C. 禁用 new/delete 的几种做法 ===\n";
    std::cout << "  做法 1：把 operator new 删掉，让它在编译期就报错\n";
    std::cout << "      void* operator new(std::size_t) = delete;\n";
    std::cout << "      void  operator delete(void*) noexcept = delete;\n";
    std::cout << "    这样任何一处 new 都是编译错误，比靠 code review 靠谱。\n";
    std::cout << "  做法 2：链接一个会 HardFault 的实现（防止第三方库偷偷 new）\n";
    std::cout << "      void* operator new(std::size_t) { while(1) {} }   // 或触发断点\n";
    std::cout << "  做法 3：只用静态容器\n";
    std::cout << "      std::array<T, N>  代替 std::vector<T>\n";
    std::cout << "      自己写的环形缓冲 代替 std::queue<T>\n";
    std::cout << "      固定数组 + 下标   代替 std::map/std::unordered_map\n";

    std::cout << "\n=== D. 静态分配的样子（零碎片、零运行时开销）===\n";
    {
                // 全部在编译期确定大小，放在 .bss 段，运行时不分配任何内存
        std::array<float, 64>  adcBuffer{};       // ADC 采样缓冲
        std::array<std::uint8_t, 128> uartRx{};   // 串口接收环形缓冲
        std::array<float, 3>   pidOutput{};       // 三轴 PID 输出

        std::cout << "    adcBuffer  : " << sizeof(adcBuffer) << " 字节\n";
        std::cout << "    uartRx     : " << sizeof(uartRx) << " 字节\n";
        std::cout << "    pidOutput  : " << sizeof(pidOutput) << " 字节\n";
        std::cout << "    合计 " << sizeof(adcBuffer) + sizeof(uartRx) + sizeof(pidOutput)
                  << " 字节，全部在 .bss 段，编译期就定好。\n";
        std::cout << "    好处：不会失败、不会变慢、不会有碎片、可以用 memcpy 整体清空。\n";
        std::cout << "    代价：要提前想清楚最大需要多少（比如串口最高波特率下的最大帧长）。\n";
    }

    std::cout << "\n=== E. 如果实在需要动态分配，怎么降风险 ===\n";
    std::cout << "  1) 只在启动阶段分配，运行期不再 new/delete：\n";
    std::cout << "     把要用的大小全部在 init() 里一次性分配好（\"启动后冻结\"）。\n";
    std::cout << "  2) 用**内存池**（memory pool / fixed-size allocator）：\n";
    std::cout << "     只提供几种固定大小（如 16/32/64 字节），同类块可以互换，\n";
    std::cout << "     于是永远不会产生\"总量够但没有连续块\"的情况。\n";
    std::cout << "  3) 预留一个\"最后手段\"的应急块：\n";
    std::cout << "     出错时先释放它，保证还能把故障信息写进 Flash 或发出去。\n";
    std::cout << "  4) 定期监控堆的高水位（watermark）：\n";
    std::cout << "     记录历史最大使用量，留 2 倍余量再发版。\n";
    std::cout << "  5) 用工具而不是猜：\n";
    std::cout << "     GNU ld 的 --wrap=malloc 可以统计分配次数和峰值。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
