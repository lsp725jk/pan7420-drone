// ch05_ex15.cpp —— 字符串函数族：strlen / strcpy / strncpy / strcat / strcmp
// 编译：check.bat ch05_ex15.cpp c++latest
#include <cstdio>
#include <cstring>

int main()
{
    char dst[32] = "abc";
    const char* src = "defg";

        // strlen：数到 '\0' 为止，不含 '\0'
    printf("strlen(\"abc\")       = %zu\n", strlen("abc"));
    printf("strlen(dst)         = %zu   sizeof(dst) = %zu\n", strlen(dst), sizeof(dst));
    printf("  ← strlen 是「运行期数长度」，sizeof 是「编译期算大小」\n");

        // strcpy：把 src 连 '\0' 一起拷过去，返回值是 dst
    char* ret = strcpy(dst, src);
    printf("\nstrcpy 后 dst = \"%s\"   返回值==dst? %s\n", dst, (ret == dst) ? "是" : "否");

        // strcat：从 dst 的 '\0' 处接着往后追加
    strcat(dst, "XYZ");
    printf("strcat 后 dst = \"%s\"  strlen=%zu\n", dst, strlen(dst));

        // strcmp：按字典序比较，返回 <0 / 0 / >0
    printf("\nstrcmp(\"abc\",\"abc\")  = %d\n", strcmp("abc", "abc"));
    printf("strcmp(\"abc\",\"abd\")  = %d\n", strcmp("abc", "abd"));
    printf("strcmp(\"abd\",\"abc\")  = %d\n", strcmp("abd", "abc"));
    printf("strcmp(\"abc\",\"ab\")   = %d   （前缀关系：长的大）\n", strcmp("abc", "ab"));
    printf("  ← 千万不要写 if (strcmp(a,b))，要写 if (strcmp(a,b) == 0)\n");

        // 带 n 的版本：最多处理 n 个字符
    char safe[8];
    strncpy(safe, "0123456789", sizeof(safe) - 1);
    safe[sizeof(safe) - 1] = '\0';       // strncpy 不保证补 '\0'，必须自己补
    printf("\nstrncpy 截断后 safe = \"%s\"  strlen=%zu\n", safe, strlen(safe));

        // 查找类函数
    const char* text = "hello world hello";
    printf("\nstrchr(text,'o')    -> \"%s\"\n", strchr(text, 'o'));
    printf("strrchr(text,'o')   -> \"%s\"   （最后一个）\n", strrchr(text, 'o'));
    printf("strstr(text,\"wor\") -> \"%s\"\n", strstr(text, "wor"));
    printf("strspn(\"abcXY\",\"abc\") = %zu  （开头连续属于集合的字符数）\n", strspn("abcXY", "abc"));
    return 0;
}
