// ch17_ex05.cpp —— 迭代器：分类与失效规则（重点：失效）
#include <cstdio>
#include <deque>
#include <iterator>
#include <list>
#include <map>
#include <string>
#include <vector>

int main()
{
    std::printf("--- 1) 迭代器的四种典型用法 ---\n");
    {
        std::vector<int> v{ 10, 20, 30, 40 };

                // 用法 1：显式迭代器
        std::printf("    1) 显式迭代器：");
        for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
            std::printf("%d ", *it);
        }
        std::printf("\n");

                // 用法 2：auto 简化
        std::printf("    2) auto          ：");
        for (auto it = v.begin(); it != v.end(); ++it) {
            std::printf("%d ", *it);
        }
        std::printf("\n");

                // 用法 3：范围 for（C++11）—— 最推荐
        std::printf("    3) 范围 for      ：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

                // 用法 4：范围 for + 引用（想改元素时必须用引用）
        std::printf("    4) 范围 for + 引用：");
        for (int& x : v) x *= 2;
        for (int x : v) std::printf("%d ", x);
        std::printf("（元素被改了）\n");
    }

    std::printf("--- 2) 迭代器分类（决定了哪些算法能用）---\n");
    {
        std::vector<int> v{ 1, 2, 3 };
        std::list<int>   l{ 1, 2, 3 };

                // 随机访问迭代器：能做算术运算
        auto vit = v.begin();
        std::printf("    vector 迭代器支持 it + 2：*(v.begin()+2) = %d\n", *(vit + 2));
        std::printf("    也支持 it[1] = %d，以及 it2 - it1 = %td\n", vit[1], v.end() - v.begin());

                // 双向迭代器：只能 ++ / --
        auto lit = l.begin();
        ++lit;
        std::printf("    list 迭代器只能 ++/--：*(++l.begin()) = %d\n", *lit);
        std::printf("    list 迭代器**不支持** it + 2（编译错误），要用 std::advance(l.begin(), 2)\n");
        auto lit2 = l.begin();
        std::advance(lit2, 2);
        std::printf("    std::advance(l.begin(), 2) 之后 = %d\n", *lit2);

                // 反向迭代器
        std::printf("    vector 反向遍历：");
        for (auto rit = v.rbegin(); rit != v.rend(); ++rit) {
            std::printf("%d ", *rit);
        }
        std::printf("\n");
    }

    std::printf("--- 3) 【坑】vector 扩容会让所有迭代器/指针/引用失效 ---\n");
    {
        std::vector<int> v{ 1, 2, 3 };
        int* p = v.data();
        auto it = v.begin();
        std::printf("    容量 %zu，data()=%p，*it=%d\n", v.capacity(),
                    static_cast<void*>(p), *it);

        v.push_back(4);        // 触发扩容（1->2->3 已经满了）
        std::printf("    push_back(4) 之后：容量 %zu，data()=%p\n", v.capacity(),
                    static_cast<void*>(v.data()));
        std::printf("    旧指针 p=%p 现在**悬空**（内存已被释放）\n", static_cast<void*>(p));
        std::printf("    旧迭代器 it 也失效了，继续用就是 UB —— 本示例不再碰它们\n");

        std::printf("    修法一：重新获取迭代器/指针\n");
        std::printf("    修法二：先 reserve 足够大，避免扩容\n");
        std::printf("    修法三：用下标 v[i]（每次重新算地址，不会悬空）\n");
        std::printf("    验证一下：v[0] = %d，v.size() = %zu\n", v[0], v.size());
    }

    std::printf("--- 4) list 插入/删除不会让其他迭代器失效 ---\n");
    {
        std::list<int> l{ 1, 2, 3, 4 };
        auto it2 = l.begin();
        ++it2;                            // 指向 2
        std::printf("    it2 指向 %d\n", *it2);
        l.insert(l.end(), 99);            // 插入新元素
        std::printf("    在尾部插入 99 之后，it2 仍然指向 %d（list 不会搬移元素）\n", *it2);
        l.erase(l.begin());               // 删掉第一个
        std::printf("    删掉第一个元素之后，it2 仍然指向 %d\n", *it2);
        std::printf("    只有\"被删除的那个元素\"的迭代器才失效\n");
    }

    std::printf("--- 5) map 的迭代器非常稳定（除了被删的元素）---\n");
    {
        std::map<int, std::string> m{ {1,"a"}, {3,"c"} };
        auto it1 = m.find(1);
        std::printf("    it1 -> %d=%s\n", it1->first, it1->second.c_str());
        m[5] = "e";
        m[7] = "g";
        std::printf("    插入 5,7 之后，it1 仍然 -> %d=%s（红黑树不搬移已有节点）\n",
                    it1->first, it1->second.c_str());
        m.erase(3);
        std::printf("    删掉 key=3 之后，it1 仍然 -> %d=%s\n",
                    it1->first, it1->second.c_str());
    }

    std::printf("--- 6) deque 的失效规则比较特殊 ---\n");
    {
        std::deque<int> d{ 1, 2, 3 };
        auto dit = d.begin();
        std::printf("    *dit = %d\n", *dit);
        d.push_back(4);                   // 尾部插入：迭代器通常没事
        std::printf("    push_back 之后 *dit = %d（MSVC 上通常仍然有效）\n", *dit);
        d.push_front(0);                  // 头部插入：**所有**迭代器都失效（实现相关）
        std::printf("    push_front 之后，按照标准：所有迭代器失效。\n");
        std::printf("    本示例不再使用 dit（避免 UB）\n");
        std::printf("    修法：插入后重新 begin()/end()\n");
    }

    std::printf("--- 7) 失效规则速查表（这张表值得抄下来）---\n");
    std::printf("    容器            插入                                 删除\n");
    std::printf("    vector          扩容则全部失效；否则插入点之后失效   插入点及之后失效\n");
    std::printf("    deque           两端插入：迭代器失效；中间插入：全部  两端删除：只被删的失效\n");
    std::printf("    list/forward_   全部保持有效                          只被删的失效\n");
    std::printf("    map/set         全部保持有效                          只被删的失效\n");
    std::printf("    unordered_*     可能 rehash -> 全部失效；否则有效      只被删的失效\n");
    std::printf("    经验：在循环里插入/删除时，用 erase 的返回值接住新迭代器：\n");
    std::printf("      it = v.erase(it);   // 不要写 v.erase(it); ++it;\n");

    return 0;
}
