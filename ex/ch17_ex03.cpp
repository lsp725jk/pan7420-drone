// ch17_ex03.cpp —— map（红黑树）/ set / unordered_map（哈希）/ unordered_set
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// 一个自定义类型，想放进 map/set 需要提供比较（或哈希+相等）
struct Point {
    int x, y;
    bool operator<(const Point& o) const {            // map/set 需要
        if (x != o.x) return x < o.x;
        return y < o.y;
    }
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }   // unordered_* 需要
};

// unordered_* 还需要一个哈希函数
struct PointHash {
    std::size_t operator()(const Point& p) const
    {
        return std::hash<int>{}(p.x) * 31u + std::hash<int>{}(p.y);
    }
};

int main()
{
    std::printf("--- 1) map：按 key 自动排序（红黑树，查找 O(log n)）---\n");
    {
        std::map<std::string, int> m;
        m["banana"] = 2;
        m["apple"]  = 5;
        m["cherry"] = 1;
        m["apple"]  = 7;                 // 覆盖
        std::printf("    插入顺序：banana, apple, cherry —— 遍历顺序却是排好序的：\n");
        for (const auto& kv : m) {
            std::printf("      %-8s = %d\n", kv.first.c_str(), kv.second);
        }
        std::printf("    size=%zu，m[\"apple\"]=%d\n", m.size(), m["apple"]);
    }

    std::printf("--- 2) unordered_map：不排序（哈希表，平均查找 O(1)）---\n");
    {
        std::unordered_map<std::string, int> um;
        um["banana"] = 2;
        um["apple"]  = 5;
        um["cherry"] = 1;
        std::printf("    遍历顺序（不保证）：\n");
        for (const auto& kv : um) {
            std::printf("      %-8s = %d\n", kv.first.c_str(), kv.second);
        }
        std::printf("    size=%zu，bucket_count=%zu，load_factor=%.2f\n",
                    um.size(), um.bucket_count(), um.load_factor());
        std::printf("    注意：遍历顺序取决于哈希函数和实现，不要依赖它\n");
    }

    std::printf("--- 3) set / unordered_set：只有 key 没有 value ---\n");
    {
        std::set<int> s{ 5, 1, 3, 3, 2 };            // 重复的 3 只保留一个
        std::printf("    set 从 {5,1,3,3,2} 构造：");
        for (int x : s) std::printf("%d ", x);
        std::printf("（自动排序去重）\n");
        std::printf("    s.count(3)=%zu, s.count(9)=%zu\n", s.count(3), s.count(9));

        std::unordered_set<std::string> us{ "b", "a", "c", "a" };
        std::printf("    unordered_set 大小=%zu（也去重）\n", us.size());
    }

    std::printf("--- 4) 查找的三种写法（map 和 unordered_map 都适用）---\n");
    {
        std::map<std::string, int> m{ { "a", 1 }, { "b", 2 } };

                // 写法 1：operator[] —— 找不到会**插入**一个默认值！
        std::printf("    写法 1: m[\"zzz\"] = %d，插入后 size=%zu（危险：查询变成了插入）\n",
                    m["zzz"], m.size());

                // 写法 2：find + 迭代器（推荐，只查不改）
        auto it = m.find("a");
        if (it != m.end()) {
            std::printf("    写法 2: find(\"a\") 找到 -> %d\n", it->second);
        }
        if (m.find("nope") == m.end()) {
            std::printf("    写法 2: find(\"nope\") 返回 end()，什么都没插入，size=%zu\n", m.size());
        }

                // 写法 3：C++17 起的 contains（C++20 起还有 map::contains）
        std::printf("    写法 3: m.contains(\"b\") = %d（C++20 起可用）\n", m.contains("b") ? 1 : 0);
        std::printf("    写法 3: m.contains(\"nope\") = %d\n", m.contains("nope") ? 1 : 0);
    }

    std::printf("--- 5) 自定义类型放进 map / unordered_map ---\n");
    {
        std::map<Point, std::string> pm;
        pm[{1, 2}] = "p12";
        pm[{0, 5}] = "p05";
        std::printf("    map<Point,string>（要 operator<）：\n");
        for (const auto& kv : pm) {
            std::printf("      (%d,%d) -> %s\n", kv.first.x, kv.first.y, kv.second.c_str());
        }

        std::unordered_map<Point, std::string, PointHash> upm;
        upm[{1, 2}] = "p12";
        upm[{0, 5}] = "p05";
        std::printf("    unordered_map<Point,string,PointHash>（要哈希 + operator==）：大小=%zu\n",
                    upm.size());
    }

    std::printf("--- 6) map 有序带来的独特能力：范围查询 ---\n");
    {
        std::map<int, std::string> m{ {1,"a"}, {3,"c"}, {5,"e"}, {7,"g"}, {9,"i"} };
        auto lo = m.lower_bound(3);        // 第一个 >= 3
        auto hi = m.upper_bound(7);        // 第一个 > 7
        std::printf("    [3, 7] 区间内的元素：");
        for (auto it = lo; it != hi; ++it) {
            std::printf("%d=%s ", it->first, it->second.c_str());
        }
        std::printf("\n");
        std::printf("    unordered_map 做不到这个（无序就没有\"区间\"的概念）\n");
    }

    std::printf("--- 7) 选型建议 ---\n");
    std::printf("    需要按 key 有序遍历 / 范围查询 / 稳定迭代顺序 -> map / set\n");
    std::printf("    只要快速查找、不在乎顺序                                -> unordered_map / set\n");
    std::printf("    元素很少（< 10 个左右）                                  -> 直接线性查找的 vector 更快\n");
    std::printf("    想按插入顺序遍历                                        -> 需要额外维护一个顺序表\n");

    return 0;
}
