// 附录B 探针 1：数组越界（栈上）
// 关键点：越界不一定会当场崩。小越界可能只读到/写到相邻变量，
// 大越界会撞上编译器插的栈保护字（/GS 的 cookie），
// 这时进程会被直接终止，退出码 0xC0000409。
#include <cstdio>

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);   // 关缓冲：崩溃前的输出也留得下

    int  canary = 0x11223344;    // 放在数组前面，观察是否被踩坏
    int  a[4]  = {1, 2, 3, 4};
    int  guard = 0x55667788;     // 放在数组后面

    std::printf("canary=0x%08X guard=0x%08X\n", canary, guard);

    std::printf("--- 1. 越界读 ---\n");
    std::printf("a[0..3] = %d %d %d %d\n", a[0], a[1], a[2], a[3]);
    std::printf("a[4] = %d  a[5] = %d  （越界读：UB，读到的是相邻栈数据）\n", a[4], a[5]);
    std::printf("越界读通常不会崩，只会拿到垃圾值 —— 所以最难发现\n");

    std::printf("--- 2. 小越界写（写 1 格）---\n");
    a[4] = 999;
    std::printf("a[4] = 999 写入完成；canary=0x%08X guard=0x%08X\n", canary, guard);
    std::printf("如果 canary/guard 变了，说明已经踩到别的变量\n");

    std::printf("--- 3. 大越界写（写 100 格）---\n");
    std::printf("即将执行 a[100] = 1 ...\n");
    a[100] = 1;                                  // 撞栈保护字，通常直接终止进程
    std::printf("这一行通常打印不出来（进程已被 /GS 检查终止）\n");
    return 0;
}
