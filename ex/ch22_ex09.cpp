// ch22_ex09.cpp —— 例 22-9：模板 + static_assert = 给类型上"契约"
// 场景：飞控里串口/DMA 收发的二进制帧，PC 端和 MCU 端必须对同一份布局有共识。
// 把"共识"写成 static_assert，谁改了结构体，编译立刻失败，而不是上天之后才发现。
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

// "契约模板"：任何一个协议结构体扔进来，都会接受三项体检。
// 注意它只能**在结构体定义完成之后**使用（类型不完整时 sizeof / type_traits 都不成立）。
template <typename T, std::size_t ExpectBytes>
struct WireLayout {
    static_assert(std::is_trivially_copyable<T>::value,
                  "协议结构体必须可平凡复制：不能有虚函数、不能有自定义拷贝构造");
    static_assert(std::is_standard_layout<T>::value,
                  "协议结构体必须是标准布局，否则字段顺序/偏移不可依赖");
    static_assert(sizeof(T) == ExpectBytes, "结构体大小与协议定义不一致");
    static constexpr bool checked = true;
};

#pragma pack(push, 1)  // 关掉填充，按 1 字节对齐
struct ImuFrame {
    std::uint8_t  head;      // 偏移 0，1 字节
    std::uint16_t seq;       // 偏移 1，2 字节
    std::int16_t  acc[3];    // 偏移 3，6 字节
    std::int16_t  gyro[3];   // 偏移 9，6 字节
    std::uint8_t  crc;       // 偏移 15，1 字节
};                           // 合计 16
#pragma pack(pop)

// 同一个协议，但忘了 pack：编译器会插入填充字节
struct ImuFrameNoPack {
    std::uint8_t  head;
    std::uint16_t seq;
    std::int16_t  acc[3];
    std::int16_t  gyro[3];
    std::uint8_t  crc;
};

#pragma pack(push, 1)
struct RcFrame {
    std::uint8_t  head;
    std::uint16_t ch[8];  // 8 通道，每通道 2 字节
    std::uint8_t  crc;
};  // 1 + 16 + 1 = 18
#pragma pack(pop)

// 契约在这里生效：谁改坏了上面任何一个结构体，这几行就编不过
static_assert(WireLayout<ImuFrame, 16>::checked);
static_assert(WireLayout<RcFrame, 18>::checked);

int main() {
    std::printf("sizeof(ImuFrame)        = %zu\n", sizeof(ImuFrame));
    std::printf("sizeof(ImuFrameNoPack)  = %zu  <-- 忘了 pack，多出填充字节\n",
                sizeof(ImuFrameNoPack));
    std::printf("sizeof(RcFrame)         = %zu\n", sizeof(RcFrame));
    std::printf("offsetof: head=%zu seq=%zu acc=%zu gyro=%zu crc=%zu\n",
                offsetof(ImuFrame, head), offsetof(ImuFrame, seq), offsetof(ImuFrame, acc),
                offsetof(ImuFrame, gyro), offsetof(ImuFrame, crc));
    std::printf("契约检查全部在编译期完成，运行期一个字节的代码都不产生\n");

        // 反面教材（取消注释就会编译失败，错误信息就是你自己写的那句话）：
        // static_assert(WireLayout<ImuFrameNoPack, 16>::checked);
        // -> error C2338: static assertion failed: '结构体大小与协议定义不一致'
    return 0;
}
