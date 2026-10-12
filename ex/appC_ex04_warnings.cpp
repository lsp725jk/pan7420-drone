// 附录C 例 4：一份"故意写满常见毛病"的代码，用来对比不同警告级别的收获
// 用 /W3 编一次、/W4 编一次、/Wall 编一次，看各能抓到几条。
#include <cstdio>
#include <vector>
#include <string>

int addOne(int value, int unusedParam) {     // 未引用的形参
    return value + 1;
}

int maybeNoReturn(int flag) {                // 不是所有路径都有返回值
    if (flag) return 1;
}

int main() {
    int untouched = 5;                       // 初始化了但没用到
    double d = 3.99;
    int narrowed = d;                        // 浮点转整数，可能丢数据
    int x = 1;
    {
        int x = 2;                           // 内层 x 遮蔽外层 x
        std::printf("inner x = %d\n", x);
    }
    std::printf("outer x = %d\n", x);

    std::vector<int> v(3);
    for (int i = 0; i < v.size(); ++i) {     // 有符号/无符号比较
        v[i] = i;
    }

    unsigned u = 1;
    int      neg = -1;
    if (neg < u) {                           // 有符号/无符号比较
        std::puts("neg < u 为真（-1 被转成了无符号大数）");
    }

    std::printf("addOne(1, 2) = %d\n", addOne(1, 2));
    std::printf("maybeNoReturn(0) = %d\n", maybeNoReturn(0));
    std::printf("NARROWED = %d\n", narrowed);

    char small[4];
    std::snprintf(small, sizeof small, "%s", "abcdefg");   // 可能被截断
    std::printf("small = %s\n", small);
    return 0;
    std::puts("这行永远执行不到");            // 无法访问的代码
}
