// ch05_ex8.cpp —— 数组名不是指针，但大多数场合会"退化"成指针
// 编译：check.bat ch05_ex8.cpp c++latest
#include <cstdio>

void f_takes_ptr(int* p)          { printf("  f_takes_ptr: sizeof(p)=%zu  ← 形参已退化为指针\n", sizeof(p)); }
void f_takes_arr(int arr[5])      { printf("  f_takes_arr: sizeof(arr)=%zu ← 形参里的 [5] 被忽略！\n", sizeof(arr)); }
void f_takes_arr2(int arr[])      { printf("  f_takes_arr2: sizeof(arr)=%zu\n", sizeof(arr)); }

int main()
{
    int a[5] = { 1, 2, 3, 4, 5 };

        // ---- 不退化：数组名仍代表"整个数组" ----
    printf("在定义所在的作用域里：\n");
    printf("  sizeof(a)      = %zu   （整个数组 5*4=20 字节）\n", sizeof(a));
    printf("  &a 的类型大小  = %zu   （int(*)[5]，指向数组的指针）\n", sizeof(&a));

        // ---- 退化：三种最常见的场合 ----
    printf("\n退化的场合：\n");
    printf("  传给函数: ");
    f_takes_ptr(a);                     // 1) 函数实参
    f_takes_arr(a);
    f_takes_arr2(a);
    int* p = a;                         // 2) 赋给指针变量
    printf("  赋给指针: p==&a[0] ? %s   p==a ? %s\n",
           (p == &a[0]) ? "是" : "否", (p == a) ? "是" : "否");
    printf("  a+0 的值: %p  （3) 参与算术运算时 a 退化成 &a[0]）\n", (void*)(a + 0));

        // ---- &a 与 a 数值相同，类型不同 ----
    printf("\n&a=%p   a=%p   （数值一样，但 &a+1 跳 20 字节，a+1 跳 4 字节）\n",
           (void*)&a, (void*)a);
    printf("  (char*)a     +1 前进 %td 字节\n", (char*)(a + 1) - (char*)a);
    printf("  (char*)&a    +1 前进 %td 字节\n", (char*)(&a + 1) - (char*)&a);
    return 0;
}
