// 附录B 探针 5：堆内存的三类经典错误
// 1) 堆缓冲区溢出
// 2) free / delete 不匹配
// 3) 释放后使用（use-after-free）
// 用 MSVC 的 AddressSanitizer 编译可以精确抓到，见附录 B 的工具一节。
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct Pod { int a; int b; };

int main(int argc, char** argv) {
    int which = argc;             // 用参数决定走哪条路，方便分别演示

    std::printf("=== 1. 堆缓冲区溢出 ===\n");
    {
        char* p = (char*)std::malloc(8);
        std::memcpy(p, "1234567", 8);            // 合法：8 字节含结尾 '\0'
        std::printf("  合法写入: %s\n", p);

        std::printf("  现在写第 9 个字节（越界）...\n");
        p[8] = 'X';                              // 越界写：UB
        std::printf("  越界写「成功」了，程序没报错 —— 这正是它危险的地方\n");
        std::fflush(stdout);
        std::free(p);
    }

    std::printf("=== 2. malloc/free 与 new/delete 混用 ===\n");
    if (which >= 1) {
        int* q = new int(5);
        std::printf("  new 出来的指针用 free 释放（UB）...\n");
        std::fflush(stdout);
        std::free(q);                            // 与 new 不匹配：UB
        std::printf("  这次没崩，但堆管理器的元数据可能已经被破坏\n");
    }
    if (which >= 2) {
        Pod* r = (Pod*)std::malloc(sizeof(Pod));
        r->a = 1; r->b = 2;
        std::printf("  malloc 出来的内存用 delete 释放（UB）...\n");
        std::fflush(stdout);
        delete r;                                // 不匹配：UB（析构函数会在未构造对象上跑）
        std::printf("  同样可能看起来没事\n");
    }

    std::printf("=== 3. 释放后使用（use-after-free）===\n");
    {
        int* u = (int*)std::malloc(sizeof(int));
        *u = 0xABCD;
        std::printf("  释放前 *u = 0x%X\n", *u);
        std::free(u);
        std::printf("  释放后继续读 *u = 0x%X （UB：值可能是旧的，也可能是别的数据）\n", *u);
        *u = 0x1234;                             // 释放后写：UB，可能踩坏别人
        std::printf("  释放后写也「成功」了\n");
    }

    std::printf("=== 4. 重复释放（double free）===\n");
    {
        int* d = (int*)std::malloc(sizeof(int));
        std::free(d);
        std::printf("  第一次 free 完成，第二次 free 是 UB（可能直接崩）\n");
        std::fflush(stdout);
        std::free(d);
        std::printf("  第二次 free 居然没崩\n");
    }

    std::printf("全部执行完毕，退出码 0 —— 上面每一步都是 UB，只是这次没被抓住\n");
    return 0;
}
