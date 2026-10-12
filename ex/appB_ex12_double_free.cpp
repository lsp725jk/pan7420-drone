// 附录B 探针 12：重复释放（double free），用于演示 ASan 的报错
#include <cstdio>
#include <cstdlib>

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    int* p = (int*)std::malloc(sizeof(int));
    *p = 7;
    std::printf("第一次 free...\n");
    std::free(p);
    std::printf("第二次 free 同一个指针...\n");
    std::free(p);                      // double free
    std::printf("居然没被拦住\n");
    return 0;
}
