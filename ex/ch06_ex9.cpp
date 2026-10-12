// ch06_ex9.cpp —— memcpy / memmove / memset：按字节操作内存
// 编译：check.bat ch06_ex9.cpp c++latest
#include <cstdio>
#include <cstring>

int main()
{
        // 1) memcpy：从源拷 n 个字节到目标，两个区必须不重叠
    int src[5] = { 1, 2, 3, 4, 5 };
    int dst[5] = { 0 };
    memcpy(dst, src, sizeof(src));
    printf("[1] memcpy 之后 dst: ");
    for (int i = 0; i < 5; ++i) printf("%d ", dst[i]);
    printf("\n    第三个参数是「字节数」，不是「元素个数」（这里 sizeof(src)=%zu）\n", sizeof(src));

        // 2) memmove：允许重叠，内部会判断方向
    char buf[16] = "ABCDEFGH";
    printf("\n[2] memmove 处理重叠区:\n");
    printf("    原内容: %s\n", buf);
    memmove(buf + 2, buf, 6);          // 源和目标重叠
    printf("    memmove(buf+2, buf, 6) 之后: %s\n", buf);   // 期望 "ABABCDEF"

    char buf2[16] = "ABCDEFGH";
        // memcpy(buf2 + 2, buf2, 6);      // ← 同样参数用 memcpy 是 UB（本机实测会得到不同结果）
    printf("    同样参数换成 memcpy 就是 UB：标准不保证结果（本示例不实际执行）\n");

        // 3) memset：把 n 个字节设成同一个值（注意：按字节设，不是按元素设）
    int a[4];
    memset(a, 0, sizeof(a));
    printf("\n[3] memset(a, 0, sizeof(a)) 之后: ");
    for (int i = 0; i < 4; ++i) printf("%d ", a[i]);
    printf("\n");

    int b[4];
    memset(b, 1, sizeof(b));            // 想设成 1，实际得到 0x01010101 = 16843009
    printf("    memset(b, 1, sizeof(b)) 之后（经典坑）: ");
    for (int i = 0; i < 4; ++i) printf("%d ", b[i]);
    printf("\n    ← 因为 memset 按字节填，每个 int 变成 0x01010101\n");
    printf("    想按元素填只能用循环；想填 0 或 -1（0xFF）才适合 memset\n");

    int c[4];
    memset(c, 0xFF, sizeof(c));
    printf("    memset(c, 0xFF, ...) 之后: ");
    for (int i = 0; i < 4; ++i) printf("%d ", c[i]);
    printf("  （补码解释成 -1）\n");

        // 4) memcmp：按字节比较（不是字符串比较）
    const char* s1 = "abc\0def";
    const char* s2 = "abc\0xyz";
    printf("\n[4] memcmp(s1,s2,7) = %d  （strcmp 会在第 4 字节的 \\0 处停下）\n",
           memcmp(s1, s2, 7));
    printf("    strcmp(s1,s2)   = %d  （只比较到 '\\0'，所以认为相等）\n", strcmp(s1, s2));

        // 5) 拷贝结构体：memcpy 比逐字段赋值更直接（但仅限于"扁平"结构体）
    struct Flat { int id; double val; char tag[4]; };
    struct Flat f1 = { 7, 3.5, "AB" };
    struct Flat f2;
    memcpy(&f2, &f1, sizeof(f1));
    printf("\n[5] memcpy 拷贝结构体: f2.id=%d f2.val=%.1f f2.tag=\"%s\"\n", f2.id, f2.val, f2.tag);
    printf("    如果结构体里有指针成员，memcpy 只是「浅拷贝」——两个结构体指向同一块内存\n");
    return 0;
}
