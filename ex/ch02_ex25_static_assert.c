/**
 * ch02_ex25_static_assert.c —— _Static_assert：把"假设"变成编译期错误
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>     /**
 * offsetof 在这里
 */
#include <assert.h>     /**
 * C11~C17 里 _Static_assert 由这个头提供 static_assert 宏
 */

/**
 * ---- 用 _Static_assert 把"平台假设"写死 ----
 */
_Static_assert(sizeof(int) == 4, "本教程假定 int 是 32 位");
_Static_assert(sizeof(void *) == 8, "64 位环境");

/* ---- 检查结构体布局：这是嵌入式里最有价值的用法 ---- */
typedef struct {
    uint8_t  id;
    uint8_t  flags;
    uint16_t value;
    uint32_t timestamp;
} frame_t;

/* 协议规定这一帧必须是 8 字节。如果编译器插了填充字节，这里就会报错。 */
_Static_assert(sizeof(frame_t) == 8, "frame_t 必须是 8 字节");

/* ---- 检查数组大小和枚举范围 ---- */
#define MAX_SAMPLES 16
_Static_assert(MAX_SAMPLES % 4 == 0, "样本数必须是 4 的倍数，方便按 4 个一组处理");

int main(void)
{
    printf("---- 1. 编译期检查通过了 ----\n");
    printf("sizeof(int)   = %zu\n", sizeof(int));
    printf("sizeof(void*) = %zu\n", sizeof(void *));
    printf("sizeof(frame_t) = %zu   （编译期已断言 == 8）\n", sizeof(frame_t));

    printf("---- 2. 运行期也确认一遍 ----\n");
    frame_t f = { 1, 0x02, 1234, 0x11223344 };
    printf("id=%u flags=0x%02X value=%u timestamp=0x%08X\n",
           (unsigned)f.id, (unsigned)f.flags,
           (unsigned)f.value, (unsigned)f.timestamp);
    printf("各成员偏移：id=%zu flags=%zu value=%zu timestamp=%zu\n",
           offsetof(frame_t, id), offsetof(frame_t, flags),
           offsetof(frame_t, value), offsetof(frame_t, timestamp));
    printf("  注意 value 的偏移是 2、timestamp 是 4：正好没有填充字节，\n");
    printf("  因为 uint8_t/uint8_t/uint16_t/uint32_t 天然就对齐好了。\n");
    printf("  如果顺序改成 uint8_t / uint32_t / uint16_t，就会插 3 个填充字节，\n");
    printf("  那时 sizeof 变成 12，上面的 _Static_assert 就会**编译失败**。\n");

    printf("---- 3. 它和 assert 的区别 ----\n");
    printf("  _Static_assert：编译期检查，条件必须是编译期常量，零运行开销；\n");
    printf("  assert        ：运行期检查，条件可以是变量，Release 版会被去掉。\n");
    assert(f.id == 1);
    printf("  assert(f.id == 1) 通过了（运行期）。\n");

    printf("---- 4. 在嵌入式项目里该断言什么 ----\n");
    printf("  a) 结构体大小与硬件手册/协议文档一致；\n");
    printf("  b) 各成员偏移与寄存器布局一致（用 offsetof 做常量表达式）；\n");
    printf("  c) 缓冲区大小是 2 的幂（如果代码里用 & (N-1) 代替取模）；\n");
    printf("  d) 枚举的取值范围能装进目标类型。\n");
    printf("  C23 起 static_assert 是关键字，写法和上面一致。\n");
    return 0;
}
