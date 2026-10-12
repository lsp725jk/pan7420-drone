// 附录A 探针 3F-b：C++ 侧的引用者
#include <cstdio>
extern const int gA;
extern       int gS;
extern       int gE;
int main() { std::printf("%d %d %d\n", gA, gS, gE); return 0; }
