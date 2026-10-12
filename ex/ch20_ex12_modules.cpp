// ch20_ex12_modules.cpp —— 模块（modules）简介：用 import std; 代替一堆 #include

// ！！本文件**不能**用普通的 check.bat 编译！！实测会报：
// error C2230: 无法找到模块"std"
// error C2039: "cout": 不是 "std" 的成员
// 原因：check.bat 只调用 cl 一次，而 import std; 需要先有一个预编译好的
// 标准库模块接口文件 std.ifc。MSVC 不会自动帮你做这一步。

// 本机实测可用的完整步骤（MSVC 14.51 / VS 2026，三条命令）：
// :: 第 0 步：进 MSVC 环境，并切到 ASCII 路径（模块对路径比较敏感）
// call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
// cd /d C:\cccwork\ex

// :: 第 1 步：构造标准库模块（约 1~3 分钟，只需一次）
// cl /nologo /utf-8 /std:c++latest /EHsc /W0 /c ^
// "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231\modules\std.ixx" ^
// /ifcOutput std.ifc
// :: 这一步会生成 std.ifc（约 36 MB）和 std.obj（约 130 KB）两个文件

// :: 第 2 步：编译本文件，必须带上 /reference 和 std.obj
// cl /nologo /utf-8 /std:c++latest /EHsc /W4 /reference std=std.ifc ^
// /Fe:mod.exe ch20_ex12_modules.cpp std.obj

// :: 第 3 步：运行
// mod.exe

// 两个容易卡住的点（本机都实际踩过）：
// 1) 只写 /headerUnit std=std.ifc 不行 —— 会报 C5058「找不到标头文件」。
// 标准库模块要用 /reference，不是 /headerUnit（/headerUnit 是给
// import <iostream>; 这种"头单元"用的）。
// 2) 只加 /reference 不链接 std.obj 也不行 —— 会报 LNK2019 无法解析
// std::_General_precision_tables_2<double>::_Max_P 之类的符号。
// 因为 std.ixx 里既有声明（进 .ifc）也有实现（进 .obj），两个都要。

// 实测输出见本章正文（已逐字粘贴）。
import std;                          // 一行代替 iostream / vector / string / algorithm...

// ============================================================
// 导出自己的模块：module 声明必须放在文件最前面
// ============================================================
// 真实项目里会把它单独放在 pid.ixx 文件里，并加上 export module pid;
// 这里为了单文件演示，用普通函数说明概念。

int main()
{
    std::cout << "=== A. import std; 一行顶几十行 #include ===\n";
    std::cout << "  import std; 之后 std::cout / std::vector / std::string\n";
    std::cout << "  / std::ranges / std::format 全部可用，不需要任何 #include。\n";

    std::vector<int> v{5, 3, 8, 1};
    std::ranges::sort(v);
    std::cout << "  ranges::sort 之后: ";
    for (int x : v) std::cout << x << " ";
    std::cout << "\n";

    std::print("  std::print 也能用: {} + {} = {}\n", 2, 3, 5);
    std::cout << "  " << std::format("std::format: {:.3f}\n", 3.14159);

    std::cout << "\n=== B. 模块 vs #include 的区别 ===\n";
    std::cout << "  #include : 文本**复制粘贴**。每个 .cpp 都要重新解析一遍头文件，\n";
    std::cout << "             所以 iostream 拖进 3 万行代码，编译慢；\n";
    std::cout << "             宏会泄漏到你的代码里，include 顺序会影响行为。\n";
    std::cout << "  import   : 二进制接口（IFC 文件）。编译器直接读预处理好的结果，\n";
    std::cout << "             编译快很多；宏不跨模块泄漏；顺序无关。\n";
    std::cout << "  实测本机：构造 std.ifc 需要一次 1~3 分钟，之后每次编译都很快。\n";

    std::cout << "\n=== C. 模块的三个关键字（了解即可）===\n";
    std::cout << "  export module pid;        声明\"我这是一个模块，叫 pid\"\n";
    std::cout << "  export int compute();     导出这个符号，别的模块能 import 到\n";
    std::cout << "  import pid;               使用别的模块\n";
    std::cout << "  import <iostream>;        头单元（header unit）：把某个头文件当模块导入\n";
    std::cout << "  import std.compat;        额外把 C 标准库放进全局命名空间\n";

    std::cout << "\n=== D. 一个真实模块长什么样（多文件结构）===\n";
    std::cout << "  pid.ixx:\n";
    std::cout << "      export module pid;\n";
    std::cout << "      export struct PidConfig { float kp, ki, kd; };\n";
    std::cout << "      export float pidStep(PidConfig&, float target, float actual);\n";
    std::cout << "  main.cpp:\n";
    std::cout << "      import pid;\n";
    std::cout << "      int main() { PidConfig c{1,0,0}; ... }\n";
    std::cout << "  编译（MSVC）：\n";
    std::cout << "      cl /std:c++latest /interface /c pid.ixx        // 生成 pid.ifc\n";
    std::cout << "      cl /std:c++latest /reference pid=pid.ifc main.cpp\n";

    std::cout << "\n=== E. 现实建议（尤其是嵌入式）===\n";
    std::cout << "  1) 模块在 MSVC 14.51 上**可用**，但需要手工准备标准库模块：\n";
    std::cout << "     本机实测要先生成 std.ifc(约36MB) + std.obj(约130KB)，\n";
    std::cout << "     编译时写 /reference std=std.ifc 并一起链接 std.obj。\n";
    std::cout << "     少任何一步都会失败（C2230 / C5058 / LNK2019，本机都踩过）。\n";
    std::cout << "  2) 标准库模块要先用命令手工生成，而且每个工具链版本都要重做一次。\n";
    std::cout << "  3) 构建系统（CMake 等）对模块的支持仍在演进，\n";
    std::cout << "     团队协作时容易出\"我这儿能编你那儿不能\"的问题。\n";
    std::cout << "  4) 嵌入式工具链（arm-none-eabi-gcc / Keil / IAR）对模块支持很有限，\n";
    std::cout << "     PY32F030 这种项目**现在不要用模块**，继续用头文件。\n";
    std::cout << "  结论：知道模块是什么、能看懂别人写的就行；新项目可以试，\n";
    std::cout << "        但嵌入式飞控项目暂时保持 #include 更稳妥。\n";

    std::cout << "\nmain 正常结束\n";
    return 0;
}
