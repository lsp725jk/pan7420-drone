// 附录B 探针 13：迭代器/容器调试检查（MSVC 的 _ITERATOR_DEBUG_LEVEL）
// 用 /D_ITERATOR_DEBUG_LEVEL=2 编译会让 MSVC 的 STL 在运行期检查迭代器失效。
#include <cstdio>
#include <vector>
#include <stdexcept>

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    std::printf("=== 1. 扩容后继续用旧迭代器 ===\n");
    {
        std::vector<int> v{1, 2, 3};
        auto it = v.begin();
        std::printf("  capacity 从 %zu 开始\n", v.capacity());
        for (int k = 0; k < 10; ++k) v.push_back(k);   // 触发扩容
        std::printf("  capacity 变成 %zu，现在解引用旧迭代器...\n", v.capacity());
        std::printf("  *it = %d\n", *it);              // 已失效
    }

    std::printf("=== 2. 越界下标（operator[] 不检查，at() 检查）===\n");
    {
        std::vector<int> v{1, 2, 3};
        std::printf("  v.at(1) = %d（合法）\n", v.at(1));
        std::printf("  即将 v.at(10)...\n");
        try {
            std::printf("  v.at(10) = %d\n", v.at(10));
        } catch (const std::out_of_range& e) {
            std::printf("  at() 抛出 std::out_of_range: %s\n", e.what());
        }
        std::printf("  （v[10] 不抛异常，是 UB）\n");
    }

    std::printf("=== 3. 对 end() 解引用 ===\n");
    {
        std::vector<int> v{1, 2, 3};
        auto e = v.end();
        std::printf("  即将解引用 v.end()...\n");
        std::printf("  *end() = %d\n", *e);          // UB
    }
    std::printf("程序结束（没有调试检查时，上面每一步都会「安静地」跑过去）\n");
    return 0;
}
