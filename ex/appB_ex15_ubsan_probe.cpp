#include <stdio.h>
int main() {
    int a = 2147483647;
    printf("before overflow\n");
    int b = a + 1;                      /**
     * signed integer overflow = UB
     */
    printf("b = %d\n", b);
    return 0;
}