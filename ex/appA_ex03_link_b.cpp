// 附录A 探针 3B：编译单元 2
// 这里尝试引用编译单元 1 里的东西，用来证明链接性差异。
int readPlain();    // 对应 int gCppPlain（外部链接）—— OK
int readInline();   // 对应 inline int gCppInline（外部链接）—— OK

extern int gCppPlain;
extern int gCppInline;

// 下面两行如果取消注释，会在链接阶段失败（LNK2001），
// 因为 C++ 里 const/constexpr 命名空间作用域变量是内部链接：
// extern const int     gCppConst;
// extern constexpr int gCppConstexpr;
// 同理 static 的也链接不到：
// extern int gCppStatic;

int readPlain()  { return gCppPlain; }
int readInline() { return gCppInline; }
