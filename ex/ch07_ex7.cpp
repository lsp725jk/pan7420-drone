// ch07_ex7.cpp —— enum：隐式/显式赋值、作用域、当数组下标
// 编译：check.bat ch07_ex7.cpp c++latest
#include <cstdio>

// 传统 enum（C 风格）：枚举名会"泄漏"到外层作用域
enum Color { RED, GREEN, BLUE };

// 显式赋值
enum ErrCode {
    ERR_OK        = 0,
    ERR_TIMEOUT   = 1,
    ERR_CHECKSUM  = 2,
    ERR_NO_MEMORY = 100,
    ERR_IO        = 101
};

// 十六进制/位掩码风格的枚举
enum Flags {
    FLAG_NONE  = 0x00,
    FLAG_A     = 0x01,
    FLAG_B     = 0x02,
    FLAG_C     = 0x04,
    FLAG_ALL   = FLAG_A | FLAG_B | FLAG_C
};

// C++11 起的 enum class：有作用域、不会隐式转成 int（更安全）
enum class State : unsigned char { IDLE = 0, RUN = 1, FAULT = 2 };

// 末尾加 _MAX / _COUNT 的技巧：可以直接拿来定数组大小
enum SensorId { SENSOR_TEMP = 0, SENSOR_PRESS, SENSOR_HUMI, SENSOR_COUNT };

int main()
{
    printf("[1] 隐式赋值：不写就是「前一个 + 1」\n");
    printf("    RED=%d GREEN=%d BLUE=%d\n", RED, GREEN, BLUE);
    printf("    sizeof(enum Color) = %zu （MSVC 用 int 存）\n", sizeof(enum Color));

    printf("\n[2] 显式赋值（可以跳号）:\n");
    printf("    ERR_OK=%d ERR_TIMEOUT=%d ERR_CHECKSUM=%d\n", ERR_OK, ERR_TIMEOUT, ERR_CHECKSUM);
    printf("    ERR_NO_MEMORY=%d ERR_IO=%d\n", ERR_NO_MEMORY, ERR_IO);

        // 做状态码：switch 比一串 if/else 清楚
    int code = ERR_CHECKSUM;
    printf("\n[3] 枚举做状态码（switch）:\n");
    printf("    code=%d → ", code);
    switch (code) {
    case ERR_OK:        printf("OK\n"); break;
    case ERR_TIMEOUT:   printf("超时\n"); break;
    case ERR_CHECKSUM:  printf("校验错\n"); break;
    case ERR_NO_MEMORY: printf("内存不足\n"); break;
    case ERR_IO:        printf("IO 错误\n"); break;
    default:            printf("未知错误\n"); break;
    }

    printf("\n[4] 枚举当数组下标（最实用的技巧）:\n");
        // 用 SENSOR_COUNT 定大小，增删枚举成员时数组自动跟着变
    const char* names[SENSOR_COUNT] = { "温度", "压力", "湿度" };
    double values[SENSOR_COUNT] = { 0 };
    values[SENSOR_TEMP] = 25.5;
    values[SENSOR_PRESS] = 1013.2;
    values[SENSOR_HUMI] = 48.0;
    for (int i = 0; i < SENSOR_COUNT; ++i)
        printf("    [%d] %s = %.1f\n", i, names[i], values[i]);
    printf("    sizeof(names)/sizeof(names[0]) = %zu == SENSOR_COUNT(%d)\n",
           sizeof(names) / sizeof(names[0]), (int)SENSOR_COUNT);
    printf("    ← 加一个 SENSOR_XXX 只要放在 SENSOR_COUNT 之前，数组自动扩容\n");

    printf("\n[5] 位掩码风格（可以按位组合）:\n");
    unsigned int f = FLAG_A | FLAG_C;
    printf("    FLAG_A|FLAG_C = 0x%02X\n", f);
    printf("    检查某位: (f & FLAG_A) ? 真 : 假  → %s\n", (f & FLAG_A) ? "真" : "假");
    printf("    检查某位: (f & FLAG_B) ? 真 : 假  → %s\n", (f & FLAG_B) ? "真" : "假");
    printf("    FLAG_ALL = 0x%02X\n", (unsigned)FLAG_ALL);

    printf("\n[6] enum class（C++11 起）：带作用域、不隐式转 int\n");
    State s = State::RUN;
    printf("    State s = State::RUN;  必须写 State:: 前缀，不会和别的枚举撞名\n");
    printf("    (int)s = %d  ← 要显式转才能当数字用\n", (int)s);
    printf("    sizeof(State) = %zu  （指定了 : unsigned char 就是 1 字节）\n", sizeof(State));
        // int x = State::RUN;      // ← 编译错误：enum class 不会隐式转换（这是好事）
    if (s == State::RUN) printf("    和整数比较也必须显式转换，减少了误比较\n");

    printf("\n[7] 枚举的三个坑:\n");
    printf("    (a) 传统 enum 的名字没有作用域：两个枚举都有 RED 就会冲突\n");
    printf("    (b) 枚举变量的取值范围可以超出枚举成员（C++ 里是 UB 边缘），别指望它自带校验\n");
    printf("    (c) 不要让枚举成员的值有空洞又拿它当数组下标，会浪费或越界\n");
    return 0;
}
