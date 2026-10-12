// ch05_ex19.cpp —— 函数指针：把"函数"当参数传（嵌入式里做回调）
// 编译：check.bat ch05_ex19.cpp c++latest
#include <cstdio>

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

// 参数类型写成函数指针：int (*op)(int, int)
int calc(int(*op)(int, int), int a, int b) { return op(a, b); }

// 起别名的写法（可读性好很多）
typedef int (*BinOp)(int, int);
int calc2(BinOp op, int a, int b) { return op(a, b); }

// 用 using 的写法（C++11 起，等价于 typedef，但更好读）
using BinOp2 = int (*)(int, int);

int main()
{
    printf("sizeof(&add) = %zu  （函数指针也是指针）\n", sizeof(&add));
    printf("add 的地址    = %p\n", (void*)&add);

        // 三种取函数地址/调用的写法
    int(*p1)(int, int) = add;        // 函数名自动转成函数指针
    int(*p2)(int, int) = &add;       // 显式取地址，完全等价
    printf("\nadd(3,4)=%d   p1(3,4)=%d   (*p2)(3,4)=%d   p2(3,4)=%d\n",
           add(3, 4), p1(3, 4), (*p2)(3, 4), p2(3, 4));

        // 用函数指针表来选择操作（嵌入式里常见的"命令分派"）
    BinOp ops[3] = { add, sub, mul };
    const char* names[3] = { "add", "sub", "mul" };
    printf("\n用函数指针数组遍历：\n");
    for (int i = 0; i < 3; ++i)
        printf("  %s(10,3) = %d\n", names[i], calc2(ops[i], 10, 3));

    printf("\n同一个 calc 传不同的函数：\n");
    printf("  calc(add,7,2) = %d\n", calc(add, 7, 2));
    printf("  calc(sub,7,2) = %d\n", calc(sub, 7, 2));
    printf("  calc(mul,7,2) = %d\n", calc(mul, 7, 2));

        // 函数指针也能为空，也能放在结构体里（回调）
    BinOp2 nothing = nullptr;
    printf("\n调用前判空（嵌入式回调必须做）: %s\n",
           nothing == nullptr ? "空指针，跳过调用" : "非空，可以调用");
    return 0;
}
