// ch22_ex24.cpp —— 例 22-24：标签分发（tag dispatch）
// 场景：std::advance(it, n) 对不同容器要用不同办法：
// vector 的迭代器能直接 it += n；
// list 只能一步一步走；
// 前向链表连 --it 都没有。
// 这些差别是"类型的能力"，运行期判断不了，只能靠编译期选择实现。
#include <cstdio>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

// -------- 三个实现，最后那个参数就是"标签" --------
template <typename It, typename Dist>
void my_advance_impl(It& it, Dist n, std::random_access_iterator_tag) {
    it += n;  // 一步到位
}

template <typename It, typename Dist>
void my_advance_impl(It& it, Dist n, std::bidirectional_iterator_tag) {
    if (n >= 0) {
        while (n-- > 0) {
            ++it;
        }
    } else {
        while (n++ < 0) {
            --it;  // 只有双向迭代器才有 --
        }
    }
}

template <typename It, typename Dist>
void my_advance_impl(It& it, Dist n, std::input_iterator_tag) {
    while (n-- > 0) {
        ++it;  // 只能前进
    }
}

// -------- 分发器：标签由 iterator_traits 在编译期算出来 --------
template <typename It, typename Dist>
void my_advance(It& it, Dist n) {
        // iterator_category 就是那三种标签类型之一；空花括号造一个临时对象
    my_advance_impl(it, n, typename std::iterator_traits<It>::iterator_category{});
}

int main() {
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::list<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::forward_list<int> f{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto iv = v.begin();
    my_advance(iv, 7);
    std::printf("vector  前进 7 -> %d\n", *iv);

    auto il = l.begin();
    my_advance(il, 3);
    std::printf("list    前进 3 -> %d\n", *il);
    my_advance(il, -2);  // 双向迭代器支持负距离
    std::printf("list    再退 2 -> %d\n", *il);

    auto iw = f.begin();
    my_advance(iw, 5);
    std::printf("forward_list 前进 5 -> %d\n", *iw);

        // 三个容器被分到了三个不同的实现：
    std::printf("vector       的标签是 random_access_iterator_tag\n");
    std::printf("list         的标签是 bidirectional_iterator_tag\n");
    std::printf("forward_list 的标签是 forward_iterator_tag（继承自 input_iterator_tag）\n");

        // 为什么 forward_list 能匹配 input 版本？
        // forward_iterator_tag 派生自 input_iterator_tag，
        // 重载解析时"派生类到基类的转换"代价相同，只有唯一最佳匹配才会被选中。
    std::printf("分发在编译期完成，运行期没有任何 if/switch 开销\n");
    return 0;
}
