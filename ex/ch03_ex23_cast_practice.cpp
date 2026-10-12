/**
 * ch03_ex23_cast_practice.c —— 显式转换的四种实用场景与两个陷阱
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main(void)
{
    printf("---- 1. 场景一：整数除法想要小数 ----\n");
    int sum = 7, cnt = 2;
    printf("  sum / cnt           = %d   （整数除法）\n", sum / cnt);
    printf("  (double)sum / cnt   = %f   （先转再除）\n", (double)sum / cnt);
    printf("  sum / (double)cnt   = %f\n", sum / (double)cnt);
    printf("  (double)(sum / cnt) = %f   （先除再转 —— 已经丢了小数）\n",
           (double)(sum / cnt));
    printf("  【关键】转换的位置决定了结果，括号的位置就是语义。\n");

    printf("---- 2. 场景二：窄化取字节 ----\n");
    uint32_t word = 0x12345678u;
    printf("  word = 0x%08X\n", word);
    printf("  取最低字节   (uint8_t)(word & 0xFF)      = 0x%02X\n",
           (unsigned)(uint8_t)(word & 0xFFu));
    printf("  取次低字节   (uint8_t)((word >> 8) & 0xFF) = 0x%02X\n",
           (unsigned)(uint8_t)((word >> 8) & 0xFFu));
    printf("  直接截断     (uint8_t)word                = 0x%02X（同上，但意图不明）\n",
           (unsigned)(uint8_t)word);
    printf("  【建议】用「先移位再掩码再转」三步，比直接强转更清楚地表达意图。\n");

    printf("---- 3. 场景三：void* 往返 ----\n");
    int value = 0x11223344;
    void *vp = &value;                          /* 任何对象指针 -> void*：隐式 */
    int  *ip = (int *)vp;                       /* void* -> 对象指针：需要显式（C 里是习惯） */
    printf("  通过 void* 往返：0x%X\n", (unsigned)*ip);
    printf("  【注意】把 void* 转成**错误的**类型再解引用是 UB：\n");
    printf("          float *fp = (float *)&value; *fp 就是未定义行为（严格别名）。\n");

    printf("---- 4. 场景四：消除有符号/无符号歧义 ----\n");
    int          a = -1;
    unsigned int b = 1u;
#pragma warning(push)
#pragma warning(disable : 4018)
    printf("  直接比较   a < b        = %d（错的，见 02.3）\n", a < b);
#pragma warning(pop)
    printf("  显式统一   (long)a < (long)b = %d\n", (long)a < (long)b);
    printf("  显式统一   (unsigned)a < b   = %d（把 -1 当成 4294967295）\n",
           (unsigned)a < b);
    printf("  【建议】比较前把两边转成同一类型，并在这行代码里把原因写进注释。\n");

    printf("---- 5. 陷阱一：用 (int) 硬压 double ----\n");
    double vals[4] = { 2.4, 2.5, 2.6, -2.6 };
    for (int i = 0; i < 4; i++) {
        printf("  (int)%+.1f = %+d    用 (int)(x + 0.5) 得到 %+d\n",
               vals[i], (int)vals[i], (int)(vals[i] + 0.5));
    }
    printf("  【症状】(int)2.6 得到 2（不是 3）—— 向零截断，不是四舍五入。\n");
    printf("  【提醒】用 (int)(x + 0.5) 做四舍五入只对**非负数**正确，\n");
    printf("          负数要写成 (int)(x - 0.5) 或者直接用 round()。\n");

    printf("---- 6. 陷阱二：指针类型乱转 ----\n");
    uint32_t bits = 0x3F800000u;                /* 这是 1.0f 的位模式 */
    float f;
    memcpy(&f, &bits, sizeof(f));               /* 正确：用 memcpy 做"位重解释" */
    printf("  用 memcpy 把 0x%08X 解释成 float = %f（正确做法）\n", bits, (double)f);
    printf("  错误做法：float *p = (float *)&bits; 然后读 *p\n");
    printf("    -> 违反严格别名规则（strict aliasing），是 UB；\n");
    printf("    -> 在 -O2 下可能被优化成你不期望的结果。\n");
    printf("  【正确工具】memcpy（可移植）、C20/C23 的 bit_cast（C++）/ 联合体（实现定义）。\n");
    return 0;
}
