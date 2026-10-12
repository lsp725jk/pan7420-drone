// ch11_ex19_pure_c.c —— 一个纯 C 源文件（模拟单片机工程里的 C 模块）
// 它由 C 编译器编译，符号名不会被 C++ 修饰。
int c_module_version(void) { return 42; }
int c_module_scale(int v)  { return v * 2; }
