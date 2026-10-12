// ch14_ex14.cpp —— 生成 shared_ptr 循环引用的内存增长证据（用二进制大小做粗略估计）
// 这个例子的作用是：把"泄漏"从"看不见"变成"能观察到的数字"。
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <memory>
#include <vector>

struct Node {
    int   payload[16];                    // 64 字节负载，让泄漏更容易被观察到
    std::shared_ptr<Node> next;
    Node() { for (int& v : payload) v = 1; }
};

int main()
{
    const int kN = 20000;
    std::printf("创建 %d 个互相成环的 Node……\n", kN);
    {
        std::vector<std::shared_ptr<Node>> nodes;
        nodes.reserve(static_cast<std::size_t>(kN));
        for (int i = 0; i < kN; ++i) {
            nodes.push_back(std::make_shared<Node>());
        }
        for (int i = 0; i + 1 < kN; ++i) {
            nodes[i]->next = nodes[i + 1];     // 每个节点强引用下一个
        }
        nodes[kN - 1]->next = nodes[0];        // 首尾相连，形成一个大环
        std::printf("环已形成，nodes[0].use_count() = %ld\n", nodes[0].use_count());
        std::printf("现在把 vector 清掉，所有外部引用都消失……\n");
        nodes.clear();
        std::printf("nodes.clear() 之后，环上的节点互相持有，一个都释放不掉\n");
    }
    std::printf("程序结束 —— 这 %d 个 Node 在进程退出前都不会被析构\n", kN);
    return 0;
}
