// 附录A 探针 3F-a：链接性对照的最小单元（无 main）
// 与 p_a03_neg_b.cpp（C++）或 p_a03_neg_c2.c（C）配对编译。
const     int gA = 1;    // C++：内部链接；  C：外部链接
static    int gS = 2;    // C++ 与 C 都是内部链接
int           gE = 3;    // C++ 与 C 都是外部链接
