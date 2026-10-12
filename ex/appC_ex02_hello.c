/**
 * 附录C 例 2：C 版 hello（按 C17 编译）
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

int main(void) {
    puts("你好，C。");
    printf("本机 sizeof(void*) = %zu\n", sizeof(void*));
    printf("sizeof(int32_t) = %zu, sizeof(intptr_t) = %zu\n",
           sizeof(int32_t), sizeof(intptr_t));
    return 0;
}
