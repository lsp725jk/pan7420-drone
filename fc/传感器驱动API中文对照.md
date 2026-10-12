# 传感器驱动 API 中文对照手册

> **为什么单独做这份文档**：工程里的 ICM42688（ARM/InvenSense 原厂）和
> BMP390（Bosch 原厂）驱动带 **11,000 多行英文注释**。
> 逐行翻译既不现实、也会把你自己的代码淹没（光 `Icm426xxDefs.h` 一个文件
> 就 2425 行，全是寄存器定义，日常根本不会去翻）。
>
> 所以这里换一个角度：**只翻译你工程真正调用的那二十几个接口**，
> 按"中文叫法 → 干什么 → 参数怎么填 → 你的代码在哪用它"组织。
> 这样你读自己的代码时，遇到的英文函数名都能在这里查到。
>
> 配套文档：`术语解析手册.md`（概念）、`C_KEYWORDS_GUIDE.md`（语言）。

---

## 一、两张对照总表（先看这个）

### 1.1 BMP390 气压计（Bosch）—— 在 `user/common.c` 里被调用

| 英文函数名 | 中文叫法 | 一句话作用 | 调用位置 |
| --- | --- | --- | --- |
| `bmp3_interface_init` | 初始化接口 | 把读写函数指针挂到驱动上（抽象的"总线接口"） | `common.c:292` |
| `bmp3_i2c_read` / `bmp3_i2c_write` | I2C 读 / 写 | 项目自己实现的 I2C 收发（本工程实际走 SPI） | `common.c:112/126` |
| `bmp3_spi_read` / `bmp3_spi_write` | SPI 读 / 写 | 项目自己实现的 SPI 收发（真正在用的） | `common.c:153/185` |
| `bmp3_delay_us` | 微秒延时 | 驱动内部需要的短延时 | `common.c:215` |
| `bmp3_init` | 初始化 | 读芯片 ID、读出厂校准系数、软复位 | `common.c:388` |
| `bmp3_check_rslt` | 检查返回值 | 包装"出错就打印"，省掉到处写 if | `common.c:233` |
| `bmp3_set_sensor_settings` | 设置传感器参数 | 一次配好过采样、滤波、ODR | `common.c:410` |
| `bmp3_set_op_mode` | 设置工作模式 | 切睡眠 / 强制单次 / 连续测量 | `common.c:415`（切 FORCED） |
| `bmp3_get_status` | 读状态 | 查"数据准备好了没"（drdy 标志） | `common.c:506` |
| `bmp3_get_sensor_data` | 读测量数据 | 读出气压与温度，**驱动内部已做二阶补偿** | `common.c:527` |
| `bmp3_soft_reset` | 软复位 | 恢复出厂寄存器状态 | （备用） |
| `bmp3_get_sensor_settings` | 读回参数 | 反查当前配置（调试用） | （备用） |
| `bmp3_fifo_*` 系列 | FIFO 相关 | 批处理模式相关，**本工程没用** | — |

**你工程用到的最小组合（读一次气压的完整流程）**：
```c
bmp3_set_op_mode(FORCED)      /* ① 命令它测一次 */
   ↓ 等一会儿
bmp3_get_status()             /* ② 问：好了没？看 drdy 标志 */
   ↓ 好了
bmp3_get_sensor_data()        /* ③ 取数（驱动已做温度补偿、二阶补偿） */
```
这个三步走正是 `术语解析手册.md` 第 11 节讲的气压计状态机。

### 1.2 ICM42688 姿态传感器（ARM/InvenSense）—— 在 `bsp.c` / `helperClockCalib.c` 里被调用

| 英文函数名 | 中文叫法 | 一句话作用 | 调用位置 |
| --- | --- | --- | --- |
| `inv_icm426xx_init` | 初始化 | 复位、校验 WHO_AM_I、配置接口与传感器 | `icm42688.c` |
| `inv_icm426xx_configure_serial_interface` | 配置串行接口 | 告诉驱动走 SPI 还是 I2C、几线模式 | `icm42688.c` |
| `inv_icm426xx_read_reg` / `write_reg` | 读 / 写寄存器 | 最底层操作，**最终会调到你的 SPI 函数** | `helperClockCalib.c:46` 等 |
| `inv_icm426xx_get_who_am_i` | 读芯片 ID | 确认"你确实是我以为的那颗芯片" | 初始化时 |
| `inv_icm426xx_device_reset` | 器件复位 | 软复位芯片 | 初始化时 |
| `inv_icm426xx_set_accel_fsr` / `set_gyro_fsr` | 设置量程 | 加速度计/陀螺仪的量程（如 ±4g、±2000dps） | 初始化时 |
| `inv_icm426xx_set_accel_frequency` / `set_gyro_frequency` | 设置采样率 | ODR（每秒采样多少次） | 初始化时 |
| `inv_icm426xx_enable_accel_low_noise_mode` | 使能加速度计（低噪声） | 开始以低噪声模式采集 | 初始化时 |
| `inv_icm426xx_enable_gyro_low_noise_mode` | 使能陀螺仪（低噪声） | 同上 | 初始化时 |
| `inv_icm426xx_get_data_from_registers` | 从寄存器读数据 | 读一次三轴加速度+角速度（非 FIFO 方式） | `accgyro.c` |
| `inv_icm426xx_get_data_from_fifo` | 从 FIFO 读数据 | 批量读取（FIFO 方式） | 备用 |
| `inv_icm426xx_get_time_us` | 取当前时间（微秒） | **回调你把系统微秒时间给它** | `bsp.c:192` |
| `inv_icm426xx_sleep_us` | 睡眠（微秒） | **回调你做微秒延时** | `bsp.c:168` |
| `inv_icm426xx_get_current_timestamp` | 读芯片时间戳 | 芯片内部计数器的当前值 | `helperClockCalib.c:248` |
| `inv_icm426xx_convert_odr_bitfield_to_us` | ODR 位域转微秒 | 把寄存器里的 ODR 编码换算成时间 | `helperClockCalib.c:193` |
| `inv_icm426xx_enable_timestamp_to_register` | 使能时间戳寄存器 | 让时间戳可以被读出 | `helperClockCalib.c:282` |
| `inv_icm426xx_get_fifo_timestamp_resolution_us_q24` | 取 FIFO 时间戳分辨率 | 时钟校准用，Q24 定点 | `helperClockCalib.c:100` |
| `inv_icm426xx_get_reg_timestamp_resolution_us_q24` | 取寄存器时间戳分辨率 | 同上 | `helperClockCalib.c:105` |
| `inv_icm426xx_get_clkin_rtc_status` | 查外部时钟状态 | 判断有没有外接 32kHz 时钟源 | `helperClockCalib.c:127` |
| `inv_icm426xx_force_clock_source` | 强制时钟源 | 时钟校准过程中用 | `helperClockCalib.c` |
| `inv_icm426xx_configure_timestamp_resolution` | 配置时间戳分辨率 | 同上 | `helperClockCalib.c` |
| **`*-apex-*` 和 `*-dmp-*` 一大票函数** | （计步、敲击、跌倒、自由落体、DMP 协处理器…） | **本工程完全没用**。它们能编译进来只是因为在同一个驱动文件里 | — |

> **⚠ 读代码时最该注意的一点**：上面那些 `inv_icm426xx_*` 函数**不是你写的**，
> 也不要试图修改它们。你真正需要看懂的是：
> - `bsp.c` 里**注册进去的两个回调**（`get_time_us` 和 `sleep_us`）——
>   这是驱动"反过来调用你的代码"的地方；
> - `user/spi.c` 里的 `inv_io_hal_read_reg` / `inv_io_hal_write_reg` ——
>   驱动的每一次寄存器读写最终都落到这两个函数上。

---

## 二、"回调"是什么：驱动反过来调用你的代码

这是读驱动代码最容易困惑的地方，单独讲清楚。

**问题**：驱动作者写 ICM42688 驱动时，**不知道**你用的是 SPI 还是 I2C、
也不知道你的芯片跑多快。他只在文档里写"请把读写函数给我"。

**解决方式**：他定义一组**函数指针**，让你在初始化时把自己写的函数"挂"上去。

```c
/* 你工程里实际做的（bsp.c:246-247，示意） */
icm_serif->read_reg  = inv_io_hal_read_reg;    /* ← 把你的读函数挂上去 */
icm_serif->write_reg = inv_io_hal_write_reg;   /* ← 把你的写函数挂上去 */
icm_serif->context   = 0;                      /* 附带一个上下文指针 */
```
之后驱动内部所有 `serif->read_reg(...)` 实际执行的都是**你的** `inv_io_hal_read_reg`。

**这叫"回调（callback）"或"钩子（hook）"**：
- 好处：驱动与硬件解耦，同一份驱动能跑在 SPI/I2C、任意 MCU 上；
- 代价：读代码时"跳转链条变长"，你看到 `read_reg` 不能立刻知道读到哪去了。

**本工程的完整调用链**（记住这一条，读传感器代码就不会迷路）：
```
imu/accgyro.c
  → icm42688.c（项目封装层）
    → Icm426xxDriver_HL.c（原厂驱动）
      → serif->read_reg（函数指针）
        → user/spi.c: inv_io_hal_read_reg（你的实现）
          → HAL_SPI_TransmitReceive()（HAL 库）
            → SPI1 硬件 → ICM42688 芯片
```

**同理，时间也是回调**：
```c
inv_icm426xx_sleep_us  ← 挂到 bsp.c 的实现（调你的 delay_us）
inv_icm426xx_get_time_us ← 挂到 bsp.c 的实现（调 micros32() 等）
```
驱动内部要"等 200ms"或"取当前时间戳"时，执行的是你的代码 ——
所以**你的延时函数不准，驱动的时序就不准**。
这也解释了为什么 `bsp_delay.h` 里那个 `delay_us` 实际偏 2 倍的问题
（见 `术语解析手册.md` 第 11 节）值得关注：它同时影响射频和传感器的时序。

---

## 三、术语对照：英文 → 中文（读驱动注释时最常撞到的词）

### 3.1 传感器相关

| 英文 | 中文 | 说明 |
| --- | --- | --- |
| IMU | 惯性测量单元 | 加速度计 + 陀螺仪（有的含磁力计） |
| Accelerometer / Accel | 加速度计 | 测加速度（含重力） |
| Gyroscope / Gyro | 陀螺仪 | 测角速度 |
| Barometer / Baro | 气压计 | 测大气压 → 换算高度 |
| FSR (Full Scale Range) | 满量程 | 能测的最大值，如 ±4g / ±2000dps |
| ODR (Output Data Rate) | 输出数据速率 | 每秒采样多少次 |
| LSB (Least Significant Bit) | 最低有效位 | 分辨率单位，如 2048 LSB/g |
| Bias / Offset | 零偏 | 静止时输出不为 0 的那部分 |
| Calibration | 校准 | 测量并记住零偏 |
| Sensitivity | 灵敏度 | 每个数字量对应多少物理量 |
| Self-test | 自检 | 芯片自己验证传感器是否正常 |
| DMP | 数字运动处理器 | 芯片内部的协处理器（本工程未用） |
| APEX | （InvenSense 的功能集名） | 计步/敲击/跌倒检测等（本工程未用） |
| FIFO | 先进先出缓冲 | 芯片内部的数据队列，可批量读取 |
| WOM (Wake On Motion) | 运动唤醒 | 有运动时才唤醒（省电，本工程未用） |
| SMD (Significant Motion Detection) | 显著运动检测 | 同上 |
| Temp compensation | 温度补偿 | 用片上温度修正读数 |
| Power-on reset (POR) | 上电复位 | — |

### 3.2 通信与硬件相关

| 英文 | 中文 | 说明 |
| --- | --- | --- |
| Serial interface / Serif | 串行接口 | 指 SPI 或 I2C |
| Register / Reg | 寄存器 | 读写硬件的地方 |
| Bitfield | 位域 | 一个字节里划分的几段 |
| Mask | 掩码 | 用来只取某几位 |
| Read-modify-write | 读-改-写 | 改寄存器某几位而不动其它位 |
| Burst read/write | 连续读/写 | 一次传输多个字节 |
| Chip Select (CS / NSS) | 片选 | 拉低表示"我在跟你说话" |
| Clock polarity / phase (CPOL/CPHA) | 时钟极性/相位 | SPI 模式 0~3 |
| MISO / MOSI / SCK | 主入从出 / 主出从入 / 时钟 | SPI 的四根线（另加 CS） |
| Interrupt (INT1/INT2) | 中断引脚 | 芯片有事件时通知 MCU |
| IBI (Interrupt Based Interface?) | 中断方式接口 | 用中断而非轮询 |
| Timestamp | 时间戳 | 芯片内部计时值 |
| Clock source | 时钟源 | 内部 RC / 外部晶振 |
| PLL | 锁相环 | 倍频电路 |
| RC oscillator | RC 振荡器 | 精度差的内部时钟 |
| Roll-over | 回绕 | 计数器数满后归零 |

### 3.3 状态与返回值相关

| 英文 | 中文 | 说明 |
| --- | --- | --- |
| Status / Return code | 状态 / 返回码 | 函数返回的成败标识 |
| `INV_ERROR_*` | （原厂错误码） | 如 `INV_ERROR_UNEXPECTED` 意外错误 |
| `BMP3_OK` | 成功 | Bosch 的成功码 |
| `BMP3_E_*` | （Bosch 错误码） | 如 `BMP3_E_DEV_NOT_FOUND` 找不到器件 |
| drdy (data ready) | 数据就绪 | 测量完成的标志 |
| Self test result | 自检结果 | — |
| Timeout | 超时 | — |

---

## 四、如果你确实需要翻译某个原厂文件

说清楚实际情况，方便你决定：

| 文件 | 行数 | 内容性质 | 建议 |
| --- | --- | --- | --- |
| `Icm426xxDefs.h` | 2425 | 寄存器/位域定义大全 | **不建议翻译**。它是"字典"，需要时查具体寄存器即可 |
| `Icm426xxDriver_HL.c` | 2145 | 驱动实现 | 不建议逐行翻译。有需要就针对具体函数问 |
| `bmp3.c` | 2976 | Bosch 驱动实现 | 同上 |
| `bmp3_defs.h` / `bmp3.h` | 921 / 549 | 类型与宏定义 | 同上 |
| `Icm426xxDriver_HL_apex.c` | 789 | DMP/APEX 功能，**本工程未用** | 完全不用看 |
| `icm42688.c` / `helperClockCalib.c` | 546 / 501 | **项目自己的封装层** | ✅ 已有中文注释，重点看这个 |

**结论**：这五个"字典型"文件（合计约 7,000 行）翻译的价值很低，
因为里面 90% 的内容你一辈子不会用到。真正值得读的是
`icm42688.c`、`helperClockCalib.c`、`user/spi.c`、`user/common.c`
——**这四个都已经是中文注释了**。

如果你在某个具体场景下撞到看不懂的英文函数（比如想看
`inv_icm426xx_configure_fifo` 到底配了什么），
**直接把函数名告诉我，我针对那一个给你讲透**，比通篇翻译高效得多。
