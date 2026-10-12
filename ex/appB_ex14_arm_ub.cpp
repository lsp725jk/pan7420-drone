/**
 * 附录B 探针 14：用读者真正用的工具链（arm-none-eabi-g++ 14.2）看 UB 的优化后果
 */
#include <cstdint>

/**
 * 1. 先解引用再判空：GCC 有权认定 p 永不为空，把判空删掉
 */
int deref_then_check(int* p) {
    int r = *p;
    if (p == nullptr) return -1;           /**
     * 这一支永远不执行
     */
    return r;
}

/**
 * 2. 有符号溢出：编译器假设不回绕，于是 x + 1 > x 恒真
 */
bool add_wraps(int x) {
    return x + 1 > x;
}

/* 3. 循环边界依赖有符号溢出：GCC 的 -fstrict-overflow 会让它变成死循环 */
int loop_overflow(int start) {
    int n = 0;
    for (int i = start; i > 0; i += 100) {
        ++n;
        if (n > 1000) return -1;
    }
    return n;
}

/* 4. 常量下标的越界访问：编译器可以直接假定不发生 */
int const_index_oob(void) {
    int a[4] = {1, 2, 3, 4};
    return a[4];
}

/* 5. 未初始化变量：编译器可以用任意值替代 */
int uninit_read(int flag) {
    int x;
    if (flag) x = 1;
    return x;
}
