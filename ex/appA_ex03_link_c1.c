/**
 * 附录A 探针 3C：C 语言里的 const 是"外部链接"——与 C++ 的关键差异
 */
const int gCConst = 777;   /**
 * C：文件作用域 const 默认外部链接
 */

int get_c_const(void) { return gCConst; }
