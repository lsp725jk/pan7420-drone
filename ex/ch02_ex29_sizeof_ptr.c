/**
 * ch02_ex29_sizeof_ptr.c —— sizeof 与指针：数组退化的量化证据
 */
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))   /**
 * 只对真数组有效
 */

/**
 * 这个函数拿到的一定是指针 —— sizeof 只有 8
 */
static void show_in_func(int *arr, size_t len)
{
    printf("    函数内 sizeof(arr)      = %zu  （指针大小）\n", sizeof(arr));
    printf("    函数内 ARRAY_LEN(arr)   = %zu  （错！8/4 = 2）\n", ARRAY_LEN(arr));
    printf("    调用者传进来的 len      = %zu  （正确）\n", len);
}

/* 形参写成数组形式也没用：编译器仍然当指针 */
static void show_in_func2(int arr[10])
{
    printf("    形参写 int arr[10] 时 sizeof(arr) = %zu  <-- 那个 10 被忽略\n",
           sizeof(arr));
}

int main(void)
{
    int arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    printf("---- 1. 数组本身：sizeof 是整个数组 ----\n");
    printf("  sizeof(arr)     = %zu  （10 * 4 = 40）\n", sizeof(arr));
    printf("  sizeof(arr[0])  = %zu\n", sizeof(arr[0]));
    printf("  ARRAY_LEN(arr)  = %zu  （正确）\n", ARRAY_LEN(arr));

    printf("---- 2. 传进函数：退化成指针 ----\n");
    show_in_func(arr, ARRAY_LEN(arr));
    show_in_func2(arr);

    printf("---- 3. 指针的 sizeof ----\n");
    int *p = arr;
    printf("  sizeof(p)       = %zu  （指针本身的大小）\n", sizeof(p));
    printf("  sizeof(*p)      = %zu  （解引用后是 int）\n", sizeof(*p));
    printf("  sizeof(p + 1)   = %zu  （指针算术的结果还是指针）\n", sizeof(p + 1));
    printf("  【关键】sizeof 看的是**类型**，不是指向多少个元素。\n");

    printf("---- 4. 字符串的两种长度 ----\n");
    char buf[32] = "hello";
    const char *str = "hello";
    printf("  char buf[32] = \"hello\";\n");
    printf("    sizeof(buf) = %zu  （整个数组 32 字节）\n", sizeof(buf));
    printf("    strlen(buf) = %zu  （实际内容 5 个字符）\n", strlen(buf));
    printf("  const char *str = \"hello\";\n");
    printf("    sizeof(str) = %zu  （指针大小！不是 6）\n", sizeof(str));
    printf("  【坑】把 sizeof 当字符串长度用，在指针上必然错。\n");

    printf("---- 5. 安全模式：在定义处算好长度再传 ----\n");
    printf("  ARRAY_LEN 只能在**数组还没退化**的作用域里用，\n");
    printf("  所以标准做法是：在 main（或定义它的文件）里算好，作为参数传下去。\n");
    return 0;
}
