// ch05_ex16.cpp —— 字符串函数的安全问题：越界写和忘补 '\0'
// 编译：check.bat ch05_ex16.cpp c++latest
// 说明：本程序的后半段故意制造缓冲区溢出，运行时会崩溃或输出异常，这是预期结果。
#include <cstdio>
#include <cstring>

// 典型漏洞写法：把外部输入直接拷进固定缓冲区
void vulnerable(const char* input)
{
    char buf[8];                 // 只有 8 字节
    strcpy(buf, input);          // 完全不管 input 有多长！
    printf("  vulnerable 里 buf = \"%s\"\n", buf);
}

// 正确写法 1：限长 + 手动补 '\0'
void fixed_strncpy(const char* input)
{
    char buf[8];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    printf("  fixed_strncpy 后 buf = \"%s\" (strlen=%zu)\n", buf, strlen(buf));
}

// 正确写法 2：snprintf 一步到位（推荐）
void fixed_snprintf(const char* input)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "%s", input);
    printf("  fixed_snprintf 后 buf = \"%s\" (strlen=%zu)\n", buf, strlen(buf));
}

int main()
{
    const char* short_input = "abc";              // 3 字符，安全
    const char* long_input = "0123456789ABCDEF";  // 16 字符，超过 8 字节

    printf("[1] 短输入，两种写法都正常：\n");
    vulnerable(short_input);
    fixed_strncpy(short_input);
    fixed_snprintf(short_input);

    printf("\n[2] 长输入，安全写法会「截断」而不是溢出：\n");
    fixed_strncpy(long_input);
    fixed_snprintf(long_input);

    printf("\n[3] 长度算错的经典 bug：strlen 和 sizeof 混用\n");
    char a[10] = "abcdefgh";   // 8 字符 + '\0' = 9 字节，刚好放得下
    printf("  strlen(a)=%zu  sizeof(a)=%zu\n", strlen(a), sizeof(a));
    char b[10];
        // 错：strlen 没算结尾的 '\0'，所以 b 最后没有结束符
    strncpy(b, a, strlen(a));
    b[strlen(a)] = '\0';       // 这一行救了它；去掉就是"没有结尾的字符串"
    printf("  补了 '\\0' 之后 b = \"%s\" (strlen=%zu)\n", b, strlen(b));

    printf("\n[4] 现在故意溢出：16 个字符拷进 8 字节的 buf\n");
    fflush(stdout);
    vulnerable(long_input);    // 越界写：破坏相邻的栈内存，可能崩也可能不崩
    printf("  如果看到这一行，说明这次越界写「没被发现」，但内存已经坏了\n");
    return 0;
}
