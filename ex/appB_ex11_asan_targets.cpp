// 附录B 探针 11：专门给 AddressSanitizer 抓的三类错误（一次只触发一个）
// 用参数选择：无参 = use-after-free；带参数 = double free
#include <cstdio>
#include <cstdlib>

int main(int argc, char**) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    int* p = (int*)std::malloc(sizeof(int));
    *p = 0x1234;
    std::printf("分配并写入: *p = 0x%X\n", *p);
    std::free(p);
    std::printf("已 free，接下来");
    std::fflush(stdout);

    if (argc > 1) {
        std::printf("再做一次 free（double free）...\n");
        std::free(p);                       // double free
        std::printf("double free 没被拦住\n");
    } else {
        std::printf("读 *p（use-after-free）...\n");
        std::printf("*p = 0x%X\n", *p);      // use-after-free
    }
    return 0;
}
