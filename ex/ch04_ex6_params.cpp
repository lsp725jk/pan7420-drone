/**
 * ch04_ex6_params.c —— 参数传递：C 只有值传递，以及怎么"模拟"引用传递
 */
#include <stdio.h>

/**
 * ---- 1. 值传递：改不动外面的变量 ----
 */
static void try_change_value(int x)
{
    x = 999;
    printf("  函数内 x = %d（改的是副本）\n", x);
}

/**
 * ---- 2. 指针参数：能改到外面 ----
 */
static void change_by_pointer(int *p)
{
    if (p == NULL) {                        /* 指针参数第一件事永远是判空 */
        printf("  收到空指针，什么都不做\n");
        return;
    }
    *p = 999;
    printf("  函数内 *p = %d\n", *p);
}

/* ---- 3. 用指针"返回"多个值 ---- */
static int divide_with_remainder(int a, int b, int *quotient, int *remainder)
{
    if (b == 0 || quotient == NULL || remainder == NULL) {
        return -1;                                  /* 返回值专门用来报错 */
    }
    *quotient  = a / b;                     /* 通过指针"带回"结果 */
    *remainder = a % b;
    return 0;
}

/* ---- 4. 数组参数：长度必须单独传 ---- */
static int sum_array(int arr[], int count)      /* int arr[] 等价于 int *arr */
{
    printf("  函数内 sizeof(arr) = %zu —— 只是指针大小，不是数组大小！\n",
           sizeof(arr));
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += arr[i];
    }
    return total;
}

/* 想"带回"修改，就传数组，数组元素本身是可以改的 */
static void double_all(int *arr, int count)
{
    for (int i = 0; i < count; i++) {
        arr[i] *= 2;
    }
}

/* ---- 5. const 指针参数：承诺不改 ---- */
static int sum_readonly(const int *arr, int count)
{
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += arr[i];
                                /* arr[i] = 0;   <-- 取消注释会编译失败，这就是 const 的价值 */
    }
    return total;
}

/* ---- 6. 结构体传参：值传递会整体复制，小结构体无所谓，大的要传指针 ---- */
typedef struct {
    int   x;
    int   y;
    int   z;
} vec3_t;

static void vec3_by_value(vec3_t v)             /* 复制整个结构体 */
{
    v.x = 100;
    printf("  按值传：函数内 v.x = %d（外面没变）\n", v.x);
}

static void vec3_by_pointer(vec3_t *v)          /* 只复制一个指针 */
{
    v->x = 100;
    printf("  按指针传：函数内 v->x = %d（外面变了）\n", v->x);
}

int main(void)
{
    printf("---- 1. 值传递 ----\n");
    int a = 1;
    printf("  调用前 a = %d\n", a);
    try_change_value(a);
    printf("  调用后 a = %d   <-- 没变，这就是值传递\n", a);

    printf("---- 2. 指针参数 ----\n");
    int b = 1;
    printf("  调用前 b = %d\n", b);
    change_by_pointer(&b);                  /* 传地址进去 */
    printf("  调用后 b = %d   <-- 变了\n", b);
    change_by_pointer(NULL);                /* 传空指针也要能安全处理 */

    printf("---- 3. 用指针返回多个值 ----\n");
    int q = 0, r = 0;
    int rc = divide_with_remainder(17, 5, &q, &r);
    printf("  17 / 5：rc=%d 商=%d 余=%d（一次调用拿回两个结果）\n", rc, q, r);
    printf("  这是一种 C 里非常常见的接口风格：返回值报错，结果用指针带回。\n");
    printf("  另一种风格：返回值直接给结果，出错返回一个特殊值（如 -1）。\n");

    printf("---- 4. 数组参数 ----\n");
    int arr[5] = { 1, 2, 3, 4, 5 };
    printf("  main 里 sizeof(arr) = %zu（真正的数组大小）\n", sizeof(arr));
    printf("  求和 = %d\n", sum_array(arr, 5));
    printf("  【症状】如果在函数里写 sizeof(arr)/sizeof(arr[0]) 想求长度，\n");
    printf("         会得到 8/4 = 2（64 位下指针 8 字节），完全错。\n");
    double_all(arr, 5);
    printf("  double_all 之后: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf(" <-- 数组内容能被改，因为传的是首元素地址\n");
    printf("  只读求和（const 参数） = %d\n", sum_readonly(arr, 5));

    printf("---- 5. 结构体传参 ----\n");
    vec3_t v = { 1, 2, 3 };
    printf("  调用前 v.x = %d\n", v.x);
    vec3_by_value(v);
    printf("  按值传之后 v.x = %d\n", v.x);
    vec3_by_pointer(&v);
    printf("  按指针传之后 v.x = %d\n", v.x);
    printf("  【选择建议】结构体很大时按值传会复制很多字节，浪费时间也吃栈；\n");
    printf("             只读就传 const 指针，要改就传指针。\n");

    printf("---- 6. 参数顺序与默认值 ----\n");
    printf("C 的参数没有默认值（C++ 才有），所以每个实参都得写；\n");
    printf("参数多于 4~5 个时，建议打包成结构体传，可读性会好很多。\n");
    return 0;
}
