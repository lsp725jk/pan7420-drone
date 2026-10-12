// ch22_ex01.cpp —— 例 22-1：C 风格"通用代码"的痛
// 需求：写一个"从数组里找最大值"的函数，要求对 int 和 double 都能用。
// C 里只有一招：void* + 元素大小 + 比较回调（qsort/bsearch 就是这个套路）。
#include <cstddef>
#include <cstdio>
#include <cstring>

// 通用实现：它不知道元素是什么类型，
// 只被告知"每个元素占多少字节"和"两个元素怎么比大小"。
const void* max_elem(const void* base, std::size_t n, std::size_t elem_size,
                     int (*cmp)(const void*, const void*)) {
    const unsigned char* p = static_cast<const unsigned char*>(base);
    const void* best = p;
    for (std::size_t i = 1; i < n; ++i) {
        const void* cur = p + i * elem_size;
        if (cmp(cur, best) > 0) {
            best = cur;
        }
    }
    return best;
}

int cmp_int(const void* a, const void* b) {
    int x = 0, y = 0;
    std::memcpy(&x, a, sizeof x);
    std::memcpy(&y, b, sizeof y);
    return (x > y) - (x < y);
}

int cmp_double(const void* a, const void* b) {
    double x = 0, y = 0;
    std::memcpy(&x, a, sizeof x);
    std::memcpy(&y, b, sizeof y);
    return (x > y) - (x < y);
}

int main() {
    int ia[16] = {3, 9, 4, 1, 7};  // 只用了前 5 个，后面补 0
    double da[5] = {3.5, 9.25, 4.0, 1.5, 7.75};

        // ---- 正确用法：三样东西（指针、元素大小、比较器）都得给对 ----
    int bi = 0;
    std::memcpy(&bi, max_elem(ia, 5, sizeof(int), cmp_int), sizeof bi);
    double bd = 0.0;
    std::memcpy(&bd, max_elem(da, 5, sizeof(double), cmp_double), sizeof bd);
    std::printf("正确用法  : max(int) = %d, max(double) = %g\n", bi, bd);

        // ---- 错误用法：元素大小写成 8（int 是 4）----
        // 编译器不会说一个字，程序照跑，只是答案错了。
    int wrong = 0;
    std::memcpy(&wrong, max_elem(ia, 5, sizeof(double), cmp_int), sizeof wrong);
    std::printf("大小写错了: max(int) = %d   <-- 应该是 9\n", wrong);

        // 为什么是 7：大小按 8 算，于是它只看下标 0、2、4、6、8 这五个位置，
        // 也就是 3、4、7、0、0，最大值自然成了 7。
    return 0;
}
