# main.c 函数全表

> 这份文档回答：**`user/main.c` 里有哪些函数、各自负责什么、牵扯到哪些文件、
> 与现有资料怎么对上。**

生成方式：`build-gcc/tools/gen_main_func_table.py` 从源码提取
（先识别并跳过注释行，避免把注释掉的代码算成真调用）。**行号可在编辑器里直接跳转。**

---

## 一、main.c 自己定义的函数

`main.c` 一共 **4 个函数**，其中 `assert_failed()` 只在 `USE_FULL_ASSERT` 打开时才编译。

| 函数 | 行号 | 职责 | 被谁调用 |
| --- | --- | --- | --- |
| `SystemClock_Config()` | `main.c:120` | 配置系统时钟：HSI 24MHz → PLL×2 → SYSCLK **48MHz**；AHB/PCLK1 均不分频。失败则进 `Error_Handler()` | `main()`（`main.c:223`） |
| `main()` | `main.c:214` | 程序入口：时钟 → 全部外设初始化 → **对码循环** → 读 Flash 参数 → **主循环调度** | 启动文件 `Reset_Handler` |
| `Error_Handler()` | `main.c:512` | 错误陷阱：死循环。**副作用才是关键** —— 主循环不再喂狗，约 0.5s 后看门狗复位，相当于持续重试 | 本文件的 `SystemClock_Config()`；HAL 库内部也引用此函数名 |
| `assert_failed()` | `main.c:527` | `assert_param()` 失败时的钩子（本工程未实现内容，仅占位） | HAL 的 `assert_param()` 宏（未启用时不编译） |

---

## 二、main.c 调用的外部函数（33 个）

这才是 `main.c` 真正「负责的功能」的体现 —— 它自己不实现功能，只**按时序编排**这些函数。

> 表里的「main.c 行」是该函数在 main.c 中**所有出现位置**；
> 有些函数（如 `decode_pack`）在对码循环和主循环里各出现一次。

### 2.1 初始化阶段（`main.c:220~261`）

| 函数 | main.c 行 | 定义位置 | 一句话功能 |
| --- | --- | --- | --- |
| `HAL_Init()` | 220 | **HAL 库 / 标准库** | 初始化 HAL 本身：Flash 预取、**SysTick 1ms**、NVIC 优先级分组 |
| `HAL_RCC_OscConfig()` | 157 | **HAL 库 / 标准库** | 按结构体配置振荡器（HSI / LSI / PLL） |
| `HAL_RCC_ClockConfig()` | 170 | **HAL 库 / 标准库** | 配置 AHB/APB 分频与 Flash 等待周期 |
| `bsp_init()` | 227 | `drivers\bsp\bsp.c:92` | 板级外设初始化总入口（上电后按顺序把要用的外设都点一遍） |
| `Led_Init()` | 229 | `user\led.c:401` | 初始化 LED 硬件（只配 LED4 的 GPIO；RGB 由 TIM1 的 PWM 初始化负责） |
| `Led_SetState()` | 232、315 | `user\led.c:457` | 设置某盏灯的状态（灭/常亮/闪烁）并立即更新硬件 |
| `Timer17_UpdateIT_Init()` | 235 | `drivers\bsp\TIM16.c:289` | 配置并启动 TIM17 的 1ms 周期中断 —— 整个飞控的系统节拍源 |
| `Advanced_Timer1_PWM_Init()` | 239 | `user\led.c:275` | 初始化 TIM1 的三路 PWM，驱动 RGB 三色灯 |
| `Advanced_Timer3_PWM_Init()` | 244 | `user\bsp_pwm.c:85` | 初始化 TIM3 的四路 PWM 输出（PWM 模式 1），用于驱动四个电机 |
| `ADC_Init()` | 248 | `user\adc.c:125` | 初始化 ADC1（12bit、软件触发、连续转换），并校准 |
| `HAL_ADC_Start()` | 251 | **HAL 库 / 标准库** | 启动 ADC 转换（之后由 `get_voltage()` 轮询读结果） |
| `accgyro_init()` | 255 | `imu\accgyro.c:46` | 初始化 ICM42688：配 SPI、量程（±8g / ±2000dps）、ODR（200Hz）、FIFO，并做一次零漂校准；填充全局 accgyro |
| `imu_init()` | 258 | `imu\imu.c:452` | IMU初始化 |
| `BMP390_init()` | 261 | `user\common.c:379` | 初始化 BMP390 并配置为 FORCED（单次强制测量）模式 |

### 2.2 对码阶段（`main.c:302` 的 `while(1)` 内）

| 函数 | main.c 行 | 定义位置 | 一句话功能 |
| --- | --- | --- | --- |
| `Led_SetAlert()` | 274、313、472、494 | `user\led.c:113` | 登记或清除一条报警，然后重算灯语 |
| `Led_Process()` | 305、427 | `user\led.c:554` | LED 灯语心跳：选颜色 + 非阻塞翻转（必须定期在主循环里调用） |
| `decode_pack()` | 309、384 | `user\deal_pack.c:138` | 收一个射频包并解析（非阻塞：没收到就立刻返回） |
| `bsp_pn298_write_reg()` | 317、318 | `drivers\bsp\bsp_pn298.c:133` | 向 PN298 的一个寄存器写入 1 个字节（最底层的“写寄存器”原语） |
| `bsp_pn298_set_channel()` | 329 | `drivers\bsp\bsp_pn298.c:643` | 设置射频通信信道（频点） |
| `HAL_GetTick()` | 323、330、470 | **HAL 库 / 标准库** | 返回 SysTick 毫秒计数（软定时器与失联判据都用它） |
| `printf()` | 316、344 | **HAL 库 / 标准库** | 串口打印（重定向到 UART，见 `build-gcc/common/syscalls_stub.c` 的 `_write`） |

### 2.3 对码成功后（读参数、切地址、起定时器、起狗）

| 函数 | main.c 行 | 定义位置 | 一句话功能 |
| --- | --- | --- | --- |
| `flash_read()` | 336 | `user\flash.c:238` | 开机时从 Flash 读回参数到 pair_data |
| `get_now_addr()` | 347 | `user\deal_pack.c:316` | 把 pair_data.paircode 的 5 字节拆成独立的地址数组 pair_addr_arr |
| `bsp_pn298_change_addr()` | 349 | `drivers\bsp\bsp_pn298.c:1575` | 运行中切换通信地址与信道（对码成功后由 main.c 调用） |
| `tim16_init()` | 352 | `drivers\bsp\TIM16.c:107` | 配置并启动 TIM16：一个 1 微秒一步的自由计数器（不开中断） |
| `WDG_init()` | 355 | `user\WDG.c:63` | 初始化独立看门狗（IWDG），并立即开始计时 |

### 2.4 主循环阶段（`main.c:379` 的 `while(1)` 内）

| 函数 | main.c 行 | 定义位置 | 一句话功能 |
| --- | --- | --- | --- |
| `decode_pack()` | 309、384 | `user\deal_pack.c:138` | 收一个射频包并解析（非阻塞：没收到就立刻返回） |
| `annexCode()` | 392 | `imu\decode.c:20` | 把 fcCmd[]（-500~+500 的摇杆量）换算成 rcCommand[]（油门 1000~2000、目标角速度） |
| `Mode_Controler()` | 402 | `user\flip.c:164` | 一键空翻状态机主函数（main.c 每约 4ms 调一次） |
| `altitude_ctrl()` | 407 | `imu\altitude.c:412` | 功能：协调高度更新和高度控制处理 |
| `flight_ctrl()` | 410 | `imu\fc.c:76` | 飞控主控制链路：**在主循环里被调用**（main.c:410），每次跑姿态/位置 PID + 混控 |
| `HAL_IWDG_Refresh()` | 422 | **HAL 库 / 标准库** | **喂狗** —— 全工程唯一的喂狗点（`main.c:422`） |
| `Led_Process()` | 305、427 | `user\led.c:554` | LED 灯语心跳：选颜色 + 非阻塞翻转（必须定期在主循环里调用） |
| `get_voltage()` | 457 | `user\adc.c:215` | 读一次电池电压并做低电保护（轮询方式，约每 1 秒被调用一次） |
| `bsp_pn298_rx_mode()` | 478 | `drivers\bsp\bsp_pn298.c:479` | 把芯片切成“接收模式”（飞控绝大部分时间都在这个状态） |
| `HAL_GetTick()` | 323、330、470 | **HAL 库 / 标准库** | 返回 SysTick 毫秒计数（软定时器与失联判据都用它） |
| `Led_SetAlert()` | 274、313、472、494 | `user\led.c:113` | 登记或清除一条报警，然后重算灯语 |

---

## 三、按「周期」看 main.c 的调度（理解它的关键）

`main.c` 本体只是个**调度器**：TIM17 每 1ms 给几个计数器 +1，主循环比对阈值来决定该谁干活。

| 触发条件 | 实际周期 | 跑哪些函数 | 归属文件 |
| --- | --- | --- | --- |
| `_cnt10ms >= 10` | 约 11ms | `decode_pack()` + `annexCode()` | `user/deal_pack.c`、`imu/decode.c` |
| `_cnt4ms >= 4` | 约 5ms | `Mode_Controler()`（空翻状态机） | `user/flip.c` |
| 每一圈 | 约 4.6ms | `altitude_ctrl()`（2.1~3.9ms）+ `flight_ctrl()`（2.5~3.4ms） | `imu/altitude.c`、`imu/fc.c` |
| `_cnt100ms >= 100` | 约 101ms | **喂狗** `HAL_IWDG_Refresh()` + `Led_Process()` | HAL、`user/led.c` |
| `_cnt1s >= 1000` | 约 1.001s | `get_voltage()`（读电池电压） | `user/adc.c` |
| 每一圈 | — | 失联检测（`>5000ms`）→ 报警 + `bLanding=1` + `bsp_pn298_rx_mode()` | HAL、`user/led.c`、`drivers/bsp/bsp_pn298.c` |
| 每一圈 | — | 油门低位且 `accData[2]>6000` → 自动解锁（`motorArmed=1`） | `imu/fc.c` 的 `fc` 结构 |

> **两条最容易搞错的地方**（已在 `术语解析手册.md` 11.6 节记录）：
> 1. **控制环不在 1ms 中断里**，而在主循环；1ms 只是**时基**。实测主循环一圈约 **4.6ms**。
> 2. 周期实际是 **N+1 ms**（判断是「先比较再清零」，而中断在同一毫秒内还会 +1）。

---

## 四、全局变量：main.c 用了哪些、谁定义的

`extern` 声明的变量虽然不在本文件定义，但**理解 main.c 必须知道它们的归属**。

| 变量 | 类型 | 定义位置 | 在 main.c 里干什么 |
| --- | --- | --- | --- |
| `accgyro` | `accgyro_s` | `imu/accgyro.c` | ① 把 Flash 读出的零偏写进去；② 用 `accData[2]` 判飞机是否平放 |
| `pair_addr_arr[]` | `uint8_t[5]` | `user/deal_pack.c` | 对码地址，由 `get_now_addr()` 拆出 |
| `pair_over_flag` | `uint8_t` | `user/deal_pack.c` | 对码完成标志，`decode_pack()` 置 1，主循环据此跳出 |
| `pair_data` | `Pair_Code_Config` | `user/flash.c` | 对码地址/信道/传感器零偏，`flash_read()` 填充 |
| `_cnt10ms` / `_cnt4ms` / `_cnt100ms` | `uint8_t` | `drivers/bsp/bsp_timer.c` | 软定时器计数（TIM17 中断里 +1） |
| `_cnt1s` | `uint16_t` | `drivers/bsp/bsp_timer.c` | 1 秒节拍 |
| `fc` | 结构体 | `imu/fc.c` | 飞行状态：`flags.motorArmed` / `motorIdle` / `bLanding` / `uartCmdNew` |
| `rcCommand[]` | 数组 | `imu/decode.c` | 内部指令（油门 1000~2000、目标角速度） |
| `IwdgHandle` | `IWDG_HandleTypeDef` | `user/WDG.c` | 喂狗时传给它 |
| `AdcHandle` | `ADC_HandleTypeDef` | `user/adc.c` | `HAL_ADC_Start(&AdcHandle)` |
| `ch_change_arr[2]` | `uint8_t[2]` | **本文件**（`main.c:80`） | 对码候选信道。定义 `{36,36}`，但 `main.c:265` 会改成 `{36,0}`（见勘误汇总） |
| `youmen` | `uint8_t` | **本文件**（`main.c:212`） | 历史遗留：只在被注释掉的代码里出现，**当前无作用** |

---

## 五、与现有资料的对照

| 想深入了解哪一块 | 看哪份资料 |
| --- | --- |
| 术语含义（时钟 / 中断 / 寄存器 / 射频 / 看门狗 / 对码 / 姿态 / 滤波） | `术语解析手册.md` |
| 这些术语**为什么长这样**、有哪几种实现方法 | `术语伴读01-03` / `04-06` / `07-08`、`飞控技术演进总纲.md` |
| 已被纠正的错误说法（含 `main.c` 自己的注释） | `_术语伴读_勘误汇总.md` |
| `main.c` 里出现的 C 关键字（`volatile` / `static` / `extern` …） | `C_KEYWORDS_GUIDE.md` |
| 传感器接口（ICM42688 / BMP390） | `传感器驱动API中文对照.md` |
| 怎么编译、烧录、调试 | `VSCODE_使用说明.md` |
| 写注释的规范 | `COMMENT_STANDARD.md` |
