/**
 * ch08_ex23.c —— 【进阶变式】用 # 和 ## 自动生成「名字表」和"唯一变量名「
 */
#include <stdio.h>
#include <stdint.h>

/**
 * ---------- 模式 1：错误码 -> 名字 ----------
 */
typedef enum {
    ERR_OK = 0,
    ERR_TIMEOUT,
    ERR_CRC,
    ERR_NO_DEVICE,
    ERR_COUNT
} err_t;

#define ERR_NAME_LIST(X)     \
    X(ERR_OK,        "成功")  \
    X(ERR_TIMEOUT,   "超时")  \
    X(ERR_CRC,       "校验错") \
    X(ERR_NO_DEVICE, "设备不在")

/* 展开 1：生成「名字表」数组（下标 = 枚举值），靠 #e 字符串化 */
#define X_TO_NAME(e, t)  [e] = #e,
static const char *const g_err_name[ERR_COUNT] = { ERR_NAME_LIST(X_TO_NAME) };
#undef X_TO_NAME

/* 展开 2：生成「中文说明表」 */
#define X_TO_TEXT(e, t)  [e] = t,
static const char *const g_err_text[ERR_COUNT] = { ERR_NAME_LIST(X_TO_TEXT) };
#undef X_TO_TEXT

/* ---------- 模式 2：批量生成函数 ---------- */
#define DEF_TEST(name, expect)                          \
    static int test_##name(void) {                      \
        int got = (expect);                             \
        printf("  test_%-6s : got = %d\n", #name, got);  \
        return got;                                     \
    }

DEF_TEST(zero,  0)
DEF_TEST(five,  5)
DEF_TEST(neg,  -3)

/* ---------- 模式 3：造不重名的局部变量（含一个经典坑） ---------- */

/* 关键工具：两级 CAT。第一级负责让参数【先展开】，第二级才做粘贴。 */
#define CAT_(a, b)  a##b
#define CAT(a, b)   CAT_(a, b)

/* 错：## 直接作用在 __LINE__ 上，它会阻止 __LINE__ 展开，三次调用都得到 lock___LINE__ -> error C2374 重定义 #define UNIQUE_NAME_BAD(prefix)  prefix##__LINE__ */

/* 对：先经过 CAT，__LINE__ 先展开成数字，再粘贴 */
#define UNIQUE_NAME(prefix)  CAT(prefix, __LINE__)

#define MAKE_LOCK()   int UNIQUE_NAME(lock_) = 0; (void)UNIQUE_NAME(lock_)

/* 变式：__COUNTER__ 每次出现就 +1，同一行用两次也能得到两个不同的名字 */
#define UNIQ2(p)  CAT(p, __COUNTER__)
static int UNIQ2(gv_) = 1; static int UNIQ2(gv_) = 2;   /* 同一行，两个全局变量 */

int main(void)
{
    printf("模式 1：错误码 -> 名字 + 中文说明\n");
    for (int k = 0; k < ERR_COUNT; k++)
        printf("  %d = %-14s (%s)\n", k, g_err_name[k], g_err_text[k]);
    printf("  注意 g_err_name[] 里存的是源码文本，靠的是 #e 字符串化\n\n");

    printf("模式 2：一个宏生成三个函数\n");
    printf("  test_zero() 返回 %d\n", test_zero());
    printf("  test_five() 返回 %d\n", test_five());
    printf("  test_neg()  返回 %d\n", test_neg());
    printf("  （函数名 test_zero 由 ## 拼出来，打印的名字由 # 字符串化）\n\n");

    printf("模式 3：靠 __LINE__ 造不重名的变量\n");
    MAKE_LOCK();                  /* 展开成 int lock_60 = 0; （行号由编译器填） */
    MAKE_LOCK();                  /* 展开成 int lock_61 = 0; */
    MAKE_LOCK();                  /* 展开成 int lock_62 = 0; */
    printf("  三次 MAKE_LOCK() 在同一作用域里也不冲突（名字里带行号）\n");
    printf("  坑：如果写成 #define UNIQUE_NAME(p) p##__LINE__ ，\n");
    printf("       ## 会阻止 __LINE__ 展开，三次都得到 lock___LINE__ -> error C2374 重定义。\n");
    printf("       正确写法是先经过两级 CAT：CAT_(a,b) a##b / CAT(a,b) CAT_(a,b)。\n");
    printf("       这和例 8-9 里 STR/STR_、ID/ID2 的两级中转是同一个道理。\n\n");

                /* 变式：__COUNTER__ 更可靠 —— 它在同一行用两次也不会撞名 （放在文件作用域，避免"未使用的局部变量"警告） */
    printf("变式：__COUNTER__ 可以在同一行造出两个不同名字（MSVC/GCC 扩展）\n");
    printf("      __LINE__ 做不到这一点。代价是 __COUNTER__ 不是标准 C。\n");
    return 0;
}
