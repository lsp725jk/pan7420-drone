// 附录A 探针 6：容器对照（C 数组/手写链表 vs vector/list/map/unordered_map/array/span）
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <array>
#include <deque>
#include <string>
#include <algorithm>

struct Node { int v; Node* next; };   // C 风格手写链表

int main() {
    std::puts("=== 1. 动态数组：malloc/realloc vs vector ===");
    int* raw = (int*)std::malloc(3 * sizeof(int));
    for (int i = 0; i < 3; ++i) raw[i] = i * 10;
    raw = (int*)std::realloc(raw, 5 * sizeof(int));      // 扩容要自己来
    for (int i = 3; i < 5; ++i) raw[i] = i * 10;
    std::printf("C   : ");
    for (int i = 0; i < 5; ++i) std::printf("%d ", raw[i]);
    std::printf(" (容量靠人记)\n");
    std::free(raw);

    std::vector<int> v;
    for (int i = 0; i < 5; ++i) v.push_back(i * 10);
    std::printf("C++ : ");
    for (int x : v) std::printf("%d ", x);
    std::printf(" size=%zu capacity=%zu\n", v.size(), v.capacity());

    std::puts("=== 2. vector 的真实扩容规律（本机实测）===");
    std::vector<int> g;
    std::size_t lastCap = 0;
    for (int i = 0; i < 40; ++i) {
        g.push_back(i);
        if (g.capacity() != lastCap) {
            std::printf("  size=%2zu -> capacity=%2zu (data=%p)\n", g.size(), g.capacity(), (void*)g.data());
            lastCap = g.capacity();
        }
    }

    std::puts("=== 3. C 数组 vs std::array：都零开销，但 array 知道自己多大 ===");
    int carr[4] = {1, 2, 3, 4};
    std::array<int, 4> sarr = {1, 2, 3, 4};
    std::printf("sizeof(carr)=%zu sizeof(sarr)=%zu sarr.size()=%zu sarr.at(2)=%d\n",
                sizeof(carr), sizeof(sarr), sarr.size(), sarr.at(2));
        // carr.size() 不存在；sizeof(carr)/sizeof(carr[0]) 是 C 的老写法
    std::printf("C 求长度老写法 sizeof/sizeof = %zu\n", sizeof(carr) / sizeof(carr[0]));

    std::puts("=== 4. 链表：手写 vs std::list vs std::forward_list ===");
    Node* head = nullptr;
    for (int i = 3; i >= 1; --i) { Node* n = (Node*)std::malloc(sizeof(Node)); n->v = i; n->next = head; head = n; }
    std::printf("C   手写链表: ");
    for (Node* p = head; p; p = p->next) std::printf("%d ", p->v);
    std::printf(" （每个结点要自己 free）\n");
    while (head) { Node* n = head->next; std::free(head); head = n; }

    std::list<int> lst = {1, 2, 3};
    lst.push_front(0);
    std::printf("C++ std::list : ");
    for (int x : lst) std::printf("%d ", x);
    std::printf(" size=%zu\n", lst.size());

    std::puts("=== 5. 关联容器：map（有序，红黑树）vs unordered_map（哈希）===");
    std::map<std::string, int> m;
    m["pan"] = 1; m["fc"] = 2; m["zts"] = 3;
    std::printf("std::map 遍历（按 key 升序）: ");
    for (auto& kv : m) std::printf("%s=%d ", kv.first.c_str(), kv.second);
    std::printf("\n");

    std::unordered_map<std::string, int> um;
    um["pan"] = 1; um["fc"] = 2; um["zts"] = 3;
    std::printf("std::unordered_map 遍历（顺序不保证）: ");
    for (auto& kv : um) std::printf("%s=%d ", kv.first.c_str(), kv.second);
    std::printf("\n");

    std::puts("=== 6. 各容器单元素大小（本机实测，含调试迭代器开销）===");
    std::printf("  sizeof(vector<int>)=%zu  sizeof(list<int>)=%zu  sizeof(map<int,int>)=%zu\n",
                sizeof(std::vector<int>), sizeof(std::list<int>), sizeof(std::map<int, int>));
    std::printf("  sizeof(unordered_map<int,int>)=%zu  sizeof(set<int>)=%zu  sizeof(deque<int>)=%zu\n",
                sizeof(std::unordered_map<int, int>), sizeof(std::set<int>), sizeof(std::deque<int>));

    std::puts("=== 7. 迭代器失效：push_back 触发扩容后，旧指针/迭代器全废 ===");
    std::vector<int> iv = {1, 2, 3};
    const int* before = iv.data();
    iv.push_back(4); iv.push_back(5);
    std::printf("  扩容前 data()=%p 扩容后 data()=%p  %s\n",
                (void*)before, (void*)iv.data(),
                before == iv.data() ? "没搬（巧合）" : "搬走了 -> 旧指针已失效");

    std::puts("=== 8. 删除元素的写法对照 ===");
    std::vector<int> dv = {1, 2, 3, 4, 5, 6};
    dv.erase(std::remove_if(dv.begin(), dv.end(), [](int x) { return x % 2 == 0; }), dv.end());
    std::printf("  erase-remove 删除偶数后: ");
    for (int x : dv) std::printf("%d ", x);
    std::printf(" size=%zu\n", dv.size());
    return 0;
}
