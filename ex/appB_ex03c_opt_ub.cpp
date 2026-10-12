// 附录B 探针 3c：找出「MSVC 真的会利用 UB」的场景
// 用 /Od 与 /O2 各编译一次对比输出。
#include <cstdio>
#include <cstdlib>

const int kConstGlobal = 42;          // 真正的 const 对象：MSVC 把它放进只读段

// 先解引用再判空：解引用已经让"p 非空"成为编译器的前提，判空可以被删掉
int nullCheckAfterDeref(int* p) {
    int r = *p;                        // 若 p 为空，这里已经是 UB
    if (p == nullptr) return -1;       // 编译器可以判定这一支永不执行
    return r;
}

int uninitUse() {
    int x;                             // 未初始化
    if (x > 0) return 1;
    return 0;
}

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    int  real = 7;
    std::printf("先解引用再判空（真实指针）= %d\n", nullCheckAfterDeref(&real));
    std::printf("未初始化变量参与判断        = %d\n", uninitUse());

    {
        int* q = (int*)std::malloc(sizeof(int));
        *q = 0x11223344;
        int before = *q;
        std::free(q);
        int after = *q;                // 释放后读：UB
        std::printf("free 前 *q=0x%X, free 后 *q=0x%X （相同不代表安全）\n", before, after);
    }

    std::printf("--- 最后演示改 const 对象：本机应当直接访问违例 ---\n");
    std::printf("kConstGlobal = %d（在只读段）\n", kConstGlobal);
    std::printf("即将通过 const_cast 写它...\n");
    int* p = const_cast<int*>(&kConstGlobal);
    *p = 100;                          // UB：写只读内存，几乎必然 0xC0000005
    std::printf("写成功了? 读回 = %d\n", kConstGlobal);
    return 0;
}
