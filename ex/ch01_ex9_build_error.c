/**
 * ch01_ex9_build_error.c —— 四类编译/链接错误的"症状"总览
 */

#include <stdio.h>

/**
 * 声明了但故意不定义（定义在注释里）——演示链接错误用
 */
int declared_but_not_defined(void);

int main(void)
{
    printf("---- 四类错误的分辨方法 ----\n");

    printf("\n[A] 语法错误（编译期，error C 开头）\n");
    printf("    忘了分号：\n");
    printf("        int a = 1\n");
    printf("        int b = 2;\n");
    printf("    实测报错：error C2143: 语法错误: 缺少\";\"（在\"int\"的前面）\n");
    printf("    注意：报错说的是**下一行**，所以看到\"缺少分号\"就看上一行。\n");

    printf("\n[B] 头文件没找到（预处理期，fatal error C1083）\n");
    printf("        #include <stdoi.h>     （把 stdio 拼错了）\n");
    printf("    实测报错：fatal error C1083: 无法打开包括文件: \"stdoi.h\"\n");
    printf("    看到 C1083：先查拼写，再查包含路径（cl 的 /I 选项）。\n");

    printf("\n[B2] 源码编码不对（warning C4819 + 输出变成乱码/字面 \\n）\n");
    printf("    本机实测：源文件存成 UTF-8，但编译时**不加 /utf-8**，\n");
    printf("    含中文的源码会先报：\n");
    printf("      warning C4819: 该文件包含不能在当前代码页(936)中表示的字符。\n");
    printf("    然后字符串里的 \\n 不再被当成换行，而是被**原样打印出来**，\n");
    printf("    中文也会掉字（实测 温度 变成了 温?）。\n");
    printf("    原因：cl 默认按本地代码页(GBK/936)解码源文件，UTF-8 的中文字节\n");
    printf("    被两两错配，正好把反斜杠和字母 n 吃成了一个汉字。\n");
    printf("    对策：编译时加 /utf-8（本教程的所有示例都加了）。\n");

    printf("\n[C] 链接错误（找不到符号，error LNK2019）\n");
    printf("    调用了一个只声明、没有定义的函数，或者忘了把那个 .c 一起编译：\n");
    printf("        cl ch01_ex9_build_error.c        （漏了提供定义的源文件）\n");
    printf("    实测报错：error LNK2019: 无法解析的外部符号 declared_but_not_defined\n");
    printf("    看到 LNK2019：查\"是不是少编译了一个 .c\"\"名字是不是拼错了\""
           "\"库是不是没加\"。\n");

    printf("\n[D] 重复定义（error LNK2005）\n");
    printf("    同一个东西被定义了两遍（最常见：在头文件里写了变量定义或函数体）。\n");
    printf("    实测报错：error LNK2005: g_counter 已经在 xxx.obj 中定义\n");
    printf("    看到 LNK2005：去头文件里找有没有定义（而不是声明）。\n");

    printf("\n---- 一句话总结 ----\n");
    printf("C 开头 = 语法/类型问题，在**一个** .c 文件里就能定位；\n");
    printf("LNK 开头 = 符号找不到或重复，答案通常在**多个文件之间**的关系里。\n");
    printf("真正调用上面那个未定义函数会链接失败，所以这里注释掉了：\n");
    printf("    printf(\"%%d\\n\", declared_but_not_defined());\n");
    return 0;
}
