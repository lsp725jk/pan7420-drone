#include <cstdio>
#include <vector>
int main() {
    unsigned u = 1;
    int i = -1;
    if (i < u) { std::puts("i < u 为真（-1 被转成无符号大数）"); }
    std::vector<int> v(3);
    for (int k = 0; k < v.size(); ++k) {}
    std::printf("%zu\n", v.size() + 0);
    return 0;
}