/**
 * 附录A 探针 3F-c：C 侧的引用者（与 p_a03_neg_a.cpp 的内容等价，只是按 C 编译）
 */
#include <stdio.h>
extern const int gA;
extern       int gS;
extern       int gE;
int main(void) { printf("%d %d %d\n", gA, gS, gE); return 0; }
