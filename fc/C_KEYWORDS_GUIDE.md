# C 语言关键字：用法、变式与本工程实例

> 本文档专为**飞控工程（PY32F030 / Cortex-M0+）**而写。
> 每个关键字按 **「作什么用 → 变式 → 本工程实例 → 坑」** 四段式讲解，
> 实例全部取自 `飞控/` 下的真实代码，可对照查看。
>
> 阅读建议：先看 [第 0 节](#0-先建立一张心智地图) 建立整体印象，
> 遇到看不懂的写法再回来查对应小节。

---

## 0. 先建立一张心智地图

C 的关键字不是零散的，它们回答四个不同的问题：

| 问题 | 相关关键字 | 一句话 |
| --- | --- | --- |
| **这个变量/函数谁能看见？** | `static` `extern` `inline` | 链接属性（作用域的第二层含义） |
| **编译器能不能优化它？** | `volatile` `const` `register` | 类型限定符 |
| **这是什么类型？** | `typedef` `struct` `union` `enum` | 自定义类型 |
| **怎么组织控制流？** | `if` `switch` `for` `while` `do-while` `break` `continue` `goto` `return` | 语句 |

**飞控代码最容易被误解的正是前两类**：`static` 和 `volatile`。
因为飞控里同时存在"中断"和"主循环"两个执行流，还有"多个 .c 文件共享变量"的需求，
这两个关键字的用法比普通 PC 程序更关键、也更容易写错。

---

## 1. `static` —— 一个关键字，三种完全不同的含义

这是 C 里最容易混淆的关键字：**位置不同，含义完全不同**。

### 1.1 三种用法对照

| 写法 | 含义 | 生存期 | 可见范围 |
| --- | --- | --- | --- |
| 文件内 `static int x;`（函数外） | 全局变量，但**只本文件可见** | 整个程序运行期 | 本 .c 文件 |
| `static void f(void){}` | 函数**只本文件可见** | — | 本 .c 文件 |
| 函数内 `static int x;` | **跨调用保持**的局部变量 | 整个程序运行期 | 仅该函数内 |

**记忆口诀**：`static` = "限制可见性" 或 "延长生存期"，
在函数**外**用是前者，在函数**内**用是后者。

### 1.2 变式一：文件内 static 全局变量（限制可见性）

```c
/* 来源：imu/pid.c —— 这三个是"模块私有"的调试观察量 */
static int32_t last_rate[3] = {0,0,0};
static float   rate_i[3]    = {0.0f, 0.0f, 0.0f};
static float   delta1[3] = {0.0f}, delta2[3] = {0.0f};
```

**为什么加 static**：PID 的中间状态只应该由 `pid()` 自己读写。
若不加 `static`，它们就成了全局符号，别的文件只要写 `extern float rate_i[3];`
就能偷偷改掉——飞控里这种"被别处误改"的 bug 极难查。
`static` 相当于文件级的"私有"。

**变式**：`static` 全局变量**未显式初始化时自动清零**（在 .bss 段），
而局部变量不初始化是随机值。这一点在飞控初始化时常被依赖。

### 1.3 变式二：static 函数（内部链接）

```c
/* 来源：imu/pid.c —— 电机混控只在本文件用 */
static inline int PIDMIX(int X, int Y, int Z, int16_t axisPID[]) { ... }

/* 来源：imu/filter.c —— 双二阶系数的实际计算函数，对外只暴露包装版 */
static void biquadFilterInit(biquadFilter_t *filter, float filterFreq,
                             uint32_t sampleDeltaUs, float Q, biquadFilterType_e filterType)
```

**为什么**：`biquadFilterInit` 需要传 Q 值和滤波器类型，是通用实现；
对外只提供 `biquadFilterInitLPF()`（固定 Q=巴特沃斯、类型=低通）。
用 `static` 把通用版藏起来，接口就只剩一个不容易用错的入口。

**变式（重要）**：`static` 函数**会被编译器当作内联候选**。
因为编译器确定"没有人从外部调用它"，可以放心决定内联或删除。
本项目 Flash 已用 95%，这个优化很有价值。

### 1.4 变式三：函数内 static 局部变量（跨调用保持状态）

这是飞控里**最常用**的一种：

```c
/* 来源：imu/fc.c —— 记录上次执行时刻，用来算 deltaT */
void flight_ctrl(void)
{
  static uint32_t previousIMUUpdateTime;   /* 只在第一次进入时清零 */
  ...
  deltaT = (uint16_t)(currentTime - previousIMUUpdateTime) * 1e-6;
  previousIMUUpdateTime = currentTime;
}

/* 来源：imu/pid.c —— PID 的积分与微分历史必须跨调用保持 */
static float rate_i[3] = {0.0f, 0.0f, 0.0f};
static int32_t last_rate[3] = {0,0,0};
```

**为什么必须 static**：`pid()` 每 1ms 被调用一次，每次返回后栈帧销毁。
积分项要"累加"、微分项要"记住上次误差"，这些状态必须活得比函数调用更久。
不加 `static` 的话每次进来都是初值，PID 会退化成纯比例控制。

### 1.5 `static` 的坑

1. **函数内 static 不是线程安全的**，也不是可重入的。
   飞控里如果有两个执行流同时改它（例如中断和主循环都调同一个函数），会出问题。
   本项目 `pid()` 只在中断里调，所以安全 —— 但这个前提要记住。
2. **`static` 局部变量的初始化只执行一次**。
   `static int x = f();` 里的 `f()` 只在第一次调用时执行 ——
   嵌入式里通常不允许这种动态初始化（启动顺序问题），本工程也没这么用。
3. **忘记加 static 造成的符号冲突**：
   两个文件都定义了 `int count;`，链接时报 "multiple definition"。

---

## 2. `volatile` —— 告诉编译器"这个值会自己变，别优化"

### 2.1 作什么用

编译器默认假设：**一个变量如果没在本段代码里被改，那它的值就不会变**，
于是把它的值缓存到寄存器里反复使用。
但嵌入式里有三种情况会打破这个假设：

1. **中断服务程序**改了它（编译器不知道中断何时发生）；
2. **硬件寄存器**的值会自己变（例如定时器计数、ADC 数据寄存器）；
3. **多执行流**（DMA、另一个内核）改了它。

`volatile` 就是告诉编译器：**每次都老老实实从内存重新读，别缓存、别删**。

### 2.2 变式一：中断与主循环共享的标志

```c
/* 来源：drivers/bsp/TIM16.c —— 计数器会被硬件自动改写 */
uint32_t micros32(void)
{
  uint16_t current_count = TIM16->CNT;   /* HAL 库已把 CNT 声明为 volatile */
  ...
}
```

**变式（写法）**：`volatile` 修饰的是**变量本身**，不是指向它的指针：

| 写法 | 含义 |
| --- | --- |
| `volatile uint32_t flag;` | flag 会自己变，每次都重新读 |
| `volatile uint32_t *p;` | p 指向的对象是 volatile（指针本身不是） |
| `uint32_t * volatile p;` | p 这个指针本身是 volatile（它指向的对象不是） |

**怎么记**：`volatile` 靠近谁就修饰谁。`*` 左边修饰指针，右边修饰所指对象。

### 2.3 变式二：避免"编译器把循环优化掉"

```c
/* 典型的延时空循环（本工程用汇编实现，这里是示意） */
volatile uint32_t i;
for (i = 0; i < 100000; i++) { }   /* 没有 volatile 时，编译器可能直接删掉整个循环 */
```

因为循环体是空的、`i` 后面又没被使用，编译器会认为"这循环没意义"而删除，
延时效果就没了。

### 2.4 `volatile` 的坑（重点）

1. **`volatile` 不保证原子性**。
   `volatile uint32_t x; x++;` 在 M0+ 上是"读-改-写"三条指令，
   中断若在中间插入就会丢更新。**共享计数器要么关中断，要么用原子操作。**
2. **`volatile` 不解决竞态，只解决"编译器优化"**。
   很多人以为加上它就线程安全了 —— 不是。
3. **别滥用**：给所有变量加 `volatile` 会让编译器无法优化，代码变慢变大。
   本项目 Flash 已 95%，滥用会有实际代价。
4. **本工程实际只有 22 处 `volatile`** —— 这个数量级是对的。
   如果你发现自己想给到处都加，通常说明设计上有共享状态没理清。

---

## 3. `const` —— "只读"的两种含义

### 3.1 变式一：修饰普通变量（只读数据）

```c
/* 来源：imu/pid.c —— 积分限幅阈值，运行期不变 */
const float _imaxYaw = 228.0f;
const float _imaxPR  = 350.0f;
```

**在 MCU 上的实际好处**：`const` 修饰的**全局**数组/常量会被放进 Flash（.rodata），
**不占 RAM**。本项目 RAM 只有 8KB，把查表数据、字符串都标 `const` 是基本习惯。

> 注意：`const` 局部变量（如上面这两个）在函数内可能仍在栈上，
> 是否进 Flash 取决于编译器优化。

### 3.2 变式二：修饰指针参数（"我只读你的数据，不会改"）

用**大数组当函数参数**时的标准写法：

```c
/* 来源：imu/mlib.c 的声明形式 */
double quickMedianFilter3(double * v);        /* 可能会改 v 的内容 */
```

如果要承诺"不改"，写成 `const double *v`。判断规则与 `volatile` 一样：

| 写法 | 含义 |
| --- | --- |
| `const char *p` | 不能通过 p 改所指内容（p 自己能改） |
| `char * const p` | p 自己不能改（所指内容能改） |
| `const char * const p` | 两者都不能改 |

### 3.3 `const` 的坑

1. **`const` 只是编译期约束**，通过强制类型转换仍能改（但这是未定义行为，
   若对象真在 Flash 里，写它会触发硬件错误）。
2. **用 `#define` 还是 `const` 定常量？**
   - `#define`：纯文本替换，无类型、不占存储，但调试时看不到、且没有作用域；
   - `const`：有类型、可被调试器看到、有作用域，但可能占存储。
   - 本工程两者都用：数组大小、寄存器地址用 `#define`；
     参与运算的阈值用 `const float`。

---

## 4. `extern` —— 声明"这个变量在别的文件里"

### 4.1 核心区分：**定义** vs **声明**

```c
int  x = 5;        /* 定义：分配存储空间，只能有一处 */
extern int x;      /* 声明：告诉编译器"有这么个东西"，可以有很多处 */
```

**一句话**：定义是"建房"，声明是"贴门牌"。门牌可以贴很多张，房子只能盖一栋。

### 4.2 本工程的标准做法

**在 .h 里声明，在 .c 里定义**：

```c
/* fc.h —— 只有声明，任何 include 它的文件都能用 fc */
extern fc_ctrl_s fc;

/* fc.c —— 定义，全项目唯一一份 */
fc_ctrl_s fc;
```

```c
/* decode.h */
extern int16_t rcCommand[4], rcCommandTrim[3];
/* decode.c */
int16_t rcCommand[4] = {0}, rcCommandTrim[3] = {0};
```

**为什么数组的 extern 声明不用写长度？**
`extern int16_t rcCommand[];` 也合法，但本工程写全长度 `[4]`，
让读代码的人一眼看到大小 —— 这是更好的习惯。

### 4.3 变式：局部 extern（在函数内声明外部变量）

本工程有多处这种写法，它**不是好习惯，但要看得懂**：

```c
/* 来源：imu/pid.c —— 在函数外集中声明来自其他模块的东西 */
extern int32_t BaroPID;          /* 来自 imu/altitude.c */
extern uint8_t wait_up;          /* 来自 user/flip.c */
extern uint32_t target_rate_qh;  /* 来自 user/flip.c */
```

**问题**：这些声明散落在各个 .c 里，而不是集中在对应的 .h 中。
一旦某个变量的类型改了（例如 `int32_t` → `int16_t`），
必须找到所有地方改，漏一处就是难查的 bug。

**正确做法**（本工程部分地方已经这么做）：把它放进对应模块的 .h，用 `#include` 引入。

> 读代码时看到 `extern`，第一件事是**去定义处确认类型是否一致**。

---

## 5. `typedef` —— 给类型起别名

### 5.1 变式一：`typedef struct`（本工程最常用）

```c
/* 来源：imu/fc.h */
typedef struct {
    uint8_t uartCmdNew:1;
    uint8_t motorArmed:1;
    ...
} fc_ctrl_flags_s;      /* 别名后缀 _s = struct，是常见命名约定 */

typedef struct {
    fc_ctrl_flags_s flags;   /* 嵌套结构体 */
    uint8_t takeofflandTimer;
    int32_t takeoffAlt;
} fc_ctrl_s;
```

**为什么用**：不写 typedef 的话，每次声明变量都要写 `struct fc_ctrl_s fc;`。
起别名后就是 `fc_ctrl_s fc;`，而且别名可以像内建类型一样直接传参、返回值。

**变式对比**：

```c
struct Foo { int a; };        /* 类型名是 struct Foo */
typedef struct { int a; } Bar; /* 类型名是 Bar，更简洁 */
typedef struct Foo { int a; } Foo;  /* 两者都可用（C 里常见，但 C++ 不需要） */
```

**命名约定**：本工程用 `_s` 后缀标结构体、`_e` 标枚举、`_t` 标句柄类型
（如 `pt1Filter_t`、`biquadFilter_t`，这是 STM32/HAL 风格的惯例）。

### 5.2 变式二：`typedef enum`

```c
/* 来源：imu/mlib.h */
typedef enum {
    X = 0,
    Y,      /* 自动 = 1 */
    Z       /* 自动 = 2 */
} axis_e;
```

**变式（显式赋值 vs 自动递增）**：

```c
typedef enum { A, B, C } e1;        /* A=0 B=1 C=2 */
typedef enum { A = 1, B, C } e2;    /* A=1 B=2 C=3 */
typedef enum { A = 0x10, B = 0x20 } e3;  /* 显式值，常用于寄存器位掩码 */
```

**枚举 vs `#define`**：枚举有类型、能在调试器里显示出**名字**而不是数字，
这是排查飞控状态机问题时的实际便利。本工程用了 144 处枚举。

### 5.3 变式三：`typedef` 函数指针（读法要点）

```c
/* 来源：imu/filter.h（示意形式） */
typedef float (*filterApplyFnPtr)(void *filter, float input);
```

**函数指针声明的读法**：从变量名出发，先右后左、由内向外。

```c
float (*fp)(void *, float);
      ↑  ↑
      |  指向一个函数，该函数接受 (void*, float) 并返回 float
      fp 是一个指针
```

对比易混的两种：

```c
float  *fp(void *, float);   /* fp 是函数，返回 float* */
float (*fp)(void *, float);  /* fp 是指针，指向返回 float 的函数 */
```

**记法**：`(*名字)` 加了括号 = 指针；没有括号 = 函数。

---

## 6. `struct` —— 把相关数据打包

### 6.1 变式一：嵌套结构体（本工程的组织方式）

```c
/* 来源：imu/fc.h —— 标志位单独成结构体，再嵌进主结构体 */
typedef struct { uint8_t motorArmed:1; ... } fc_ctrl_flags_s;
typedef struct { fc_ctrl_flags_s flags; ... } fc_ctrl_s;
```

**好处**：`fc.flags.motorArmed` 比 `fc_motorArmed` 更有层次，
且相关标志集中在一处便于整体清零。

### 6.2 变式二：位域（紧凑存布尔标志）

```c
typedef struct {
    uint8_t uartCmdNew:1;
    uint8_t motorArmed:1;
    uint8_t motorIdle:1;
    uint8_t bVelocityControl:1;
    uint8_t bAccGyroCalibrationEnb:1;
    uint8_t bTakeoff:1;
    uint8_t bLanding:1;
} fc_ctrl_flags_s;    /* 7 个标志只占 1 个字节 */
```

**语法**：`类型 成员名 : 位数;`
**为什么用**：7 个标志用 7 个 `uint8_t` 要 7 字节，位域只要 1 字节。
本工程 RAM 只有 8KB，这类节省有意义。

**位域的坑（务必知道）**：
1. **不能取地址**：`&fc.flags.motorArmed` 是**非法**的
   （位不是字节，没有地址）。需要传地址时只能用普通 `uint8_t`。
2. **布局由编译器决定**：不同编译器对位域的排列顺序（从低位还是高位开始）、
   是否跨字节，标准没有强制规定。**不要用它去对应硬件寄存器或通信协议的字节布局** ——
   那种场合要用显式位运算（见第 12 节）。
3. **位宽不能超过类型宽度**：`uint8_t x:9;` 是错的。
4. 本工程用位域只做**内部标志**，不用于协议解析 —— 这个用法是安全的。

---

## 7. `union` —— 同一块内存的多种解释

### 7.1 作什么用

`union` 的所有成员**共用同一块内存**，改一个就是改全部。
典型用途：**把多字节数据拆成字节**，或反之。

### 7.2 本工程的典型用途（协议/数据拆解）

```c
/* 典型形式：把一个 float 拆成 4 个字节发送，或把 2 个字节拼成 int16 */
typedef union {
    float    f;
    uint8_t  b[4];
    uint32_t u;
} float_bytes_u;
```

**对比 struct**：

| | struct | union |
| --- | --- | --- |
| 内存 | 成员**依次排列**，总大小 ≈ 各成员之和 | 成员**重叠**，总大小 = 最大成员 |
| 同时有效 | 所有成员 | 同一时刻只有一个成员有意义 |

### 7.3 `union` 的坑

1. **写一个成员、读另一个成员** 在 C 里是合法的（这也是它的用途），
   但结果依赖**字节序**（大小端）。PY32/Cortex-M0+ 是**小端**：
   低字节在低地址。跨平台通信时必须明确字节序。
2. 与位域配合时同样有编译器布局差异，不要用来对应协议。

---

## 8. `inline` —— 建议编译器"展开而不是调用"

### 8.1 作什么用

函数调用有开销（压栈、跳转、返回）。对**很小又很热**的函数，
把它直接在调用处展开能省下这些开销。

```c
/* 来源：imu/pid.c —— 混控函数每次 PID 要调 4 次 */
static inline int PIDMIX(int X, int Y, int Z, int16_t axisPID[]) { ... }
```

**为什么这里值得内联**：`pid()` 跑在 1ms 定时中断里，混控要调 4 次。
而且整段 PID 必须在 1ms 内跑完，省下的调用开销是实打实的。

### 8.2 关键认知：`inline` 只是**建议**

编译器可以拒绝。而且：
- **`static inline`**：最实用。函数只本文件可见 + 建议内联，
  编译器可以放心内联，**不会产生多余符号**。
- **单独的 `inline`**（不带 static）：C99 语义微妙，容易出现
  "undefined reference" 链接错误 —— 本工程没有这么用，是对的。
- **`__attribute__((always_inline))`**：强制内联（GCC 扩展），本工程未使用。

### 8.3 `inline` 与 `#define` 宏的取舍

| | `static inline` 函数 | `#define` 宏 |
| --- | --- | --- |
| 类型检查 | ✅ 有 | ❌ 无 |
| 副作用（如 `i++`） | ✅ 安全 | ⚠️ 参数出现两次就自增两次 |
| 调试 | ✅ 能打断点 | ❌ 看不到 |
| 能否取地址 | ✅ 可以 | ❌ 不行 |

**结论**：能用 `static inline` 就别用宏。本工程两者都用，各取所长。

---

## 9. `__weak` —— 弱符号（库给默认实现，工程可覆盖）

### 9.1 作什么用

**弱符号**：链接器允许同名强符号覆盖它。
嵌入式里最常见的场景是**中断服务函数**：

```c
/* 来源：drivers/PY32F0xx_HAL_Driver/.../py32f0xx_hal_tim.c（HAL 库） */
__weak void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* 默认什么都不做 */
}

/* 来源：drivers/bsp/bsp_timer.c（本工程覆盖它） */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM17) {
        /* 这里才是真正要执行的中断逻辑 */
    }
}
```

**工作机制**：链接器看到同名符号时，**强符号赢**，弱定义被丢弃。
所以库不需要知道谁会实现它，工程也不需要改库代码。

**变式（GCC 里怎么写）**：本工程为了支持 GCC 编译，在启动文件里用汇编实现同样的效果：

```asm
.weak  TIM16_IRQHandler
.thumb_set  TIM16_IRQHandler, Default_Handler
```

含义：如果工程里定义了 `TIM16_IRQHandler` 就用工程的，否则指向 `Default_Handler`（死循环）。
这跟 `__weak` 是同一个概念，只是语法不同。

---

## 10. `__attribute__((...))` —— GCC 的编译指示扩展

### 10.1 本工程用到的几种

**① `naked`（裸函数）** —— 用于需要自己控制栈帧的场合：

```c
/* 来源：drivers/bsp/bsp_delay.c —— GCC 分支下的延时函数 */
__attribute__((naked)) void __delay_pan742(uint32_t cycle)
{
    (void)cycle;
    __asm__ volatile(
        ".short 0x3801\n\t"   /* subs r0, #1  */
        ".short 0xD8FC\n\t"   /* bcs  .-4     */
        ".short 0x4770\n\t"); /* bx   lr      */
}
```

**`naked` 的含义**：编译器**不生成**函数入口/出口代码（不保存寄存器、不建栈帧）。
必须自己写完整的汇编，包括返回指令。

**为什么这里需要**：这是一个精确延时的汇编循环，
编译器如果插入了自己的寄存器保存代码，会破坏时序精度。

**② `packed`（取消对齐填充）** —— 用于对应外部数据布局：

```c
/* 典型形式：解析通信包时保证结构体紧凑、无填充 */
typedef struct __attribute__((packed)) {
    uint8_t  header;
    uint16_t value;     /* 不加 packed 时，这里会被填充 1 字节对齐 */
    uint8_t  checksum;
} packet_t;
```

**为什么需要**：结构体默认会插入填充字节以满足对齐。
若用 `sizeof` 计算长度去读通信包，填充会让长度与实际字节数不符，
解析就错位了。

**坑**：`packed` 后成员的地址可能未对齐，**在 M0+ 上访问未对齐的
多字节数据会触发硬件异常（HardFault）**。所以 `packed` 结构体里的
多字节成员要逐字节读取再拼装，不要直接取地址解引用。

### 10.2 语法变式

```c
__attribute__((naked))              /* 用在函数前 */
struct __attribute__((packed)) S {}; /* 用在 struct 关键字后 */
typedef struct {} __attribute__((packed)) T;  /* 也可以放在末尾 */
```

---

## 11. 宏（`#define`）—— 用法与四个必知的坑

### 11.1 变式一：常量宏与类型宏

```c
/* 来源：imu/decode.h */
#define FC_CMD_LENGTH     10
#define FC_CMD_THROTTLE   2
```

**优点**：无类型、零开销、能用于数组长度和 `#if` 判断。
**缺点**：没有作用域、调试器里看不到、出错时报错信息是展开后的文本。

### 11.2 变式二：带参数的宏函数（括号是生死线）

```c
/* 来源：imu/mlib.h */
#define SQ(x)    ((x)*(x))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(x)   ((x) > 0 ? (x) : -(x))
```

**这些宏名的英文缩写含义**（读代码撞到时对照）：

| 宏名 | 英文全称 | 中文意思 |
| --- | --- | --- |
| `SQ` | SQuare | 平方 |
| `MIN` / `MAX` | MINimum / MAXimum | 取最小 / 最大值 |
| `ABS` | ABSolute value | 取绝对值 |
| `PIDMIX` | PID MIXing | PID 混控（把三个轴的 PID 修正量分配到四个电机，见 `imu/pid.c`） |
| `QMF_*` | Quick Median Filter | 快速中值滤波（`imu/mlib.c` 的 `QMF_SORT` / `QMF_SWAP` / `QMF_COPY`） |
| `BAD_*` / `GOOD_*` | （故意取的示例名） | 本文档里用来说明"错误写法 vs 正确写法"的对照名，不是工程里的宏 |

**规则：每个参数加括号、整个表达式再加一层括号。**
反例对比：

```c
#define BAD_SQ(x)  x*x
BAD_SQ(a+b)   →  a+b*a+b        /* 完全错了 */
#define SQ(x) ((x)*(x))
SQ(a+b)       →  ((a+b)*(a+b))  /* 正确 */
```

**还有副作用陷阱**：`SQ(i++)` 展开成 `((i++)*(i++))`，`i` 自增两次。
带自增、赋值、函数调用的实参不要传给这类宏。

> **本工程里最好用的"一对一错"对照**（同一文件、相邻两行，来自 `imu/imu.h`）：
> ```c
> /* ✗ 参数没加括号：DEGREES_TO_DECIDEGREES(a+b) 会展开成 (a+b*10)，错 */
> #define DEGREES_TO_DECIDEGREES(angle) (angle * 10)
> #define DECIDEGREES_TO_DEGREES(angle) (angle / 10)
>
> /* ✓ 参数加了括号 */
> #define DEGREES_TO_RADIANS(angle)     ((angle) * 0.0174532925f)
> ```
> 上面那三个宏目前只被传简单变量，所以**没有暴露问题**；
> 但只要有人传 `x+1` 这类表达式就会出错。读代码时看到这种宏要留个心。
>
> 另一个变形（`icm426/helperClockCalib.h`）：用反斜杠续行写多行宏，
> 且**整个宏体没有外层括号** —— 调用处一旦参与更大的表达式就会算错：
> ```c
> #define NUMBER_OF_SAMPLES_FOR_ODR(odr_us)                    \
>     TIME_US_FOR_CLOCK_CALIBRATION / (odr_us) + TIME_US_FOR_CLOCK_CALIBRATION % (odr_us);
> ```

### 11.3 变式三：多语句宏 → 必须用 `do { } while(0)` 包起来

```c
/* 来源：imu/mlib.c —— 中值滤波的辅助宏 */
#define QMF_SORT(a,b) { if ((a)>(b)) QMF_SWAP((a),(b)); }

/* 来源：drivers/bsp/bsp_delay.h —— 本工程最规范的两个例子 */
#define delay_us(us)  do{if((us)){__delay_pan742((((int32_t)(us))<<4) + 270);}}while(0)
#define delay_ms(ms)  do{uint32_t cnt=(ms);while(cnt--){__delay_pan742(7980);}}while(0)
```

`delay_ms` 里还有两个额外技巧值得学：
1. **`uint32_t cnt=(ms);` 先拷贝一份参数** —— 避免实参被多次求值
   （若调用处传 `i++`，不拷贝就会按自增后的值算错次数）；
2. 注释里提醒的坑：`cnt` 是**无符号**，传 `0` 会回绕成 `0xFFFFFFFF`，
   跑 40 多亿次 —— 这是 `while(cnt--)` 这种写法的经典陷阱。

**为什么标准写法是 `do { ... } while(0)`**：

```c
#define BAD()   stmt1; stmt2
if (cond) BAD();     /* 展开后：if (cond) stmt1; stmt2;  → stmt2 总被执行！ */

#define GOOD()  do { stmt1; stmt2 } while(0)
if (cond) GOOD();    /* 展开后是一个完整语句块，正确 */
```

**`do{}while(0)` 的三个好处**：
1. 宏体是一个**单条语句**，能安全放进 `if/else` 而不加大括号；
2. 结尾需要分号，用法与函数一致，读起来自然；
3. 编译器会把 `while(0)` 优化掉，**零运行时开销**。

> 本项目 `QMF_SORT` 用的是 `{ }` 形式而非 `do-while`。
> 在当前调用方式（单独一行、后面跟分号、不在 `if` 里裸用）下是安全的，
> 但**如果以后要放进 `if` 里，必须改成 `do{}while(0)`**。

### 11.4 变式四：字符串化 `#` 与拼接 `##`

```c
#define STR(x)   #x          /* STR(abc) → "abc"    把参数变成字符串 */
#define CAT(a,b) a##b        /* CAT(foo,1) → foo1   把两个记号粘成一个 */
```

**用途**：批量生成函数名、寄存器名（例如 `CAT(GPIOA_, pin)`）。
本工程用得少，主要出现在 HAL 库里。

### 11.5 条件编译（`#if` / `#ifdef`）—— 本工程的重要用法

```c
/* 来源：drivers/bsp/bsp_delay.c —— 同一份源码同时支持两个编译器 */
#if defined(__CC_ARM)
__ASM void __delay_pan742(uint32_t cycle) { ... }   /* Keil ARMCC 语法 */
#elif defined(__GNUC__)
__attribute__((naked)) void __delay_pan742(uint32_t cycle) { ... }  /* GCC 语法 */
#else
#error "__delay_pan742: unsupported compiler (need ARMCC or GCC)"
#endif
```

**变式对比**：

| 写法 | 含义 | 注意 |
| --- | --- | --- |
| `#ifdef X` | 定义了 X（哪怕定义为 0） | 不要用 `#ifdef` 判断布尔值 |
| `#if defined(X)` | 同上，可组合：`#if defined(A) && !defined(B)` | 推荐写法 |
| `#if X` | X 的值非 0 | 适合 `#define DEBUG 1` 这种 |

**`#error` 的价值**：遇到不认识的编译器**立刻报错**，
而不是静默编出一个行为不对的固件 —— 这是"失败要响亮"的工程原则。

### 11.6 头文件保护（include guard）

```c
#ifndef __DECODE_H
#define __DECODE_H
... 内容 ...
#endif
```

**作用**：防止同一个头文件被重复 include 导致重复定义。
**变式**：也可以写 `#pragma once`，但 `#ifndef` 形式兼容性最好。
本工程统一用 `#ifndef`。

> 注意：以 `__` 开头的标识符在 C 标准里是**保留给编译器**的，
> 自己用属于灰色地带（本工程沿用了厂商风格，暂不改动）。

---

## 12. 位运算 —— 嵌入式里最重要的一组运算符

单片机编程离不开位操作。本工程里 `|=` 出现 634 次、`&=` 96 次。

### 12.1 五种基本操作（务必形成肌肉记忆）

```c
x |=  (1 << n);     /* 置位：把第 n 位置 1，其余不变 */
x &= ~(1 << n);     /* 清位：把第 n 位置 0，其余不变 */
x ^=  (1 << n);     /* 翻转：第 n 位取反 */
y  = (x >> n) & 1;  /* 读位：取出第 n 位的值（0 或 1） */
x &=  ~(0x3 << n);  /* 清多位：一次清掉两个位 */
```

**为什么用 `|=` 而不是 `=`**：寄存器里往往有多个位段，
`REG = 0x08` 会**把其他位全清零**（写坏别人的配置）；
`REG |= 0x08` 才是"只动我要动的那一位"。这是新手最常见的硬件 bug。

> **本工程实测频次**：`|=` 出现 **634 次**、`&=` **96 次**、`<<` **108 次**，
> 但 **`^=` 一次都没用**。"翻转某位"的需求在本工程是用
> "读出来 → 判断 → 分别 `|=` 或 `&=`"实现的。
> 所以 `^=` 属于"要认识、但本工程用不上"的写法。
>
> **本工程真实实例**：
> - 置位：`bsp_pn298.c` → `data[2] |= (1 << 5);`
> - 清位：`bsp_pn298.c` → `temp &= ~(0x01);`
> - 读位：`helperClockCalib.c` → `if (sensor_mask & (1 << INV_ICM426XX_SENSOR_FSYNC_EVENT))`
> - 取最低位：`bsp_pn298.c` → `bsp_pn298_read_reg(DEM_CAL) & 0x01`
> - 取第 j 位：`bsp_pn298.c` → `((data1 >> j) & 0x01)`（先右移再屏蔽）
> - **`|=` 的另一种用途（注意不是位操作）**：`icm42688.c` 里
>   `rc |= inv_icm426xx_configure_fifo(...)` 是把多步操作的**返回码累积**起来
>   一起判断（只要有一个出错，最终 rc 就非 0），而不是置位。同一文件有连续 8 行这种写法。
> - 拼字节（拓宽后移位）：`icm42688.c` → `((uint16_t)buffer[0] << 8) | buffer[1]`

### 12.2 变式：一次操作多位 / 位段

```c
/* 设置某个位段（连续 3 位） */
#define MASK   (0x7 << 4)      /* 第 4~6 位 */
REG = (REG & ~MASK) | (value << 4);   /* 先清后设，是位段操作的标准套路 */
```

**本工程的标准三步写法**（来源：`bsp_pn298.c` 的 `set_mode`）：

```c
temp &= ~(0x03 << 6);                             /* ① 先把 bit7:6 一起清掉 */
temp |= (((uint8_t)mode & PN298_MODE_MSK) << 2);  /* ② 掩码取合法位 ③ 移位 ④ 写入 */
```
注意 `0x03 << 6` 比直接写 `0xC0` 更好读 —— 一眼看出是"第 7、6 两位"。

**"先清后设"很重要**：如果只写 `REG |= (value << 4)`，
当 value 的某些位是 0 时，旧值不会被打扫掉，结果就错了。

### 12.3 变式：移位做乘除（性能技巧）

```c
x >> 1     /* 等价 x / 2 */
x >> 3     /* 等价 x / 8 */
x << 4     /* 等价 x * 16 */
x & 0x0F   /* 等价 x % 16（仅当 x 非负） */
```

**为什么在飞控里重要**：M0+ 没有硬件除法指令，除法是软件模拟，很慢。
本项目在定标换算（Q30 定点、>> 30）里大量使用移位代替除法。

**坑**：**有符号数右移**是"算术移位还是逻辑移位"由实现定义
（GCC 是有符号右移 = 算术右移，保留符号），负数右移不要依赖具体行为。
**移位量不要超过类型宽度**：`uint8_t x = 1 << 8;` 是未定义行为
（`1` 是 int，这里其实是 `int` 的移位，赋值时才截断 —— 要写 `1u << 8`）。

---

## 13. 控制流关键字

### 13.1 `switch/case` —— 三个必知要点

```c
/* 来源：imu/mlib.c 的双二阶系数计算 */
switch (filterType) {
    default:
    case FILTER_LPF:
        b0 = (1 - cs) / 2;
        ...
        break;
    case FILTER_NOTCH:
        ...
        break;
}
```

1. **`break` 不能漏**：漏了会"贯穿"（fall through）到下一个 case。
   有时这是有意为之（多个 case 共用一段代码，如上面的 `default:` 与 `FILTER_LPF`），
   但**有意的贯穿应加注释**说明，否则读代码的人会以为是 bug。
2. **`default` 不是必须的，但强烈建议写**：本工程将其与第一个 case 合并，
   保证 `filterType` 传了非法值时也有确定行为（不会什么都不做）。
3. **只能用于整数类型**：`switch` 的表达式必须是整型（`float` 不行），
   `case` 必须是**编译期常量**。

### 13.2 `do { } while(...)` —— 至少执行一次

```c
/* 典型形式：先做再判断 */
do {
    val = read_hardware();
} while (val == BUSY);
```

**与 `while` 的区别**：`while` 可能一次都不执行，`do-while` 至少执行一次。
读取硬件状态、重试逻辑常用它。

**本工程实测（和直觉相反，值得注意）**：
本项目**没有任何"活的" `do-while` 循环**。`do` 只出现在两种地方：
1. **宏定义里**的 `do{...}while(0)` —— 这不是循环，是"把多语句宏包成一条语句"的技巧
   （见第 11.3 节的 `delay_us` / `delay_ms`）；
2. 一段**被注释停用**的代码（`user/main.c`，原意是失联后进入降落循环）。

工程里所有真循环都用 `while(...)` 或 `for(...)`。
`do-while` 的活样本只存在于厂商驱动里（如 `Icm426xxDriver_HL_apex.c`）。
⇒ 结论：**`do{...}while(0)` 这个"宏技巧"在本工程用得很频繁，但 `do-while` 作为循环几乎不用。**
这两个 `do` 长得像、含义完全不同，是读代码时容易看错的地方。

### 13.3 `goto` —— 本工程只用 2 处（这个数量是健康的）

`goto` 在嵌入式里的**正当用途**是"统一错误清理出口"：

```c
int init_all(void)
{
    if (init_a() != OK) goto fail;
    if (init_b() != OK) goto fail;
    return OK;
fail:
    cleanup();
    return ERR;
}
```

**为什么嵌入式里可以接受**：比嵌套 if 更清晰，且能集中释放资源。
**禁止用途**：往前跳、跳进循环体、用来替代正常循环 —— 那会毁掉可读性。

**本工程实测**：非厂商代码里 `goto` **恰好只有 2 处**，都在
`icm426/Icm426xxDriver_HL_apex.c` 的 `load_dmp_sram` 函数里，
两个失败分支都跳向函数末尾的 `end_load_dmp_sram:` 标签做统一收尾：

```c
	if (cur_idx != size) {
		status = INV_ERROR_UNEXPECTED;
		goto end_load_dmp_sram;      /* ← 统一错误出口 */
	}
	...
	if (data != dmp_prog[cur_idx]) {
		status = INV_ERROR_MEM;
		goto end_load_dmp_sram;
	}
	...
end_load_dmp_sram:
	status |= inv_icm426xx_write_reg(s, MPUREG_PWR_MGMT_0, 1, &pwr_mgmt);
	return status;
```
标签名以 `end_` 开头、直接表达意图 —— 这是 `goto` 唯一被公认合理的用法。

### 13.4 三元运算符 `?:`

```c
/* 来源：imu/mlib.h 的 ABS 宏 */
#define ABS(x) ((x) > 0 ? (x) : -(x))

/* 来源：user/led.c —— 单边限幅，比 if/else 紧凑得多 */
rgbLedControl.red = (red > 1000) ? 1000 : red;

/* 来源：imu/mlib.c —— 连空格都省了的紧凑写法 */
return v>0?v:-v;
```

**规则**：`条件 ? 真值 : 假值`，本质是"有返回值的 if"。
**坑**：
1. 优先级很低，**嵌套时一定要加括号**；
2. 两个分支的类型会做隐式转换，混用有符号/无符号容易出错；
3. 上面的 `v>0?v:-v` 就是 `mlib_abs()` 的实现 —— 注意 `INT32_MIN` 取绝对值会溢出（本工程未处理）。

---

## 14. 嵌入式特有的写法（关键字之外，但同属"语法要点"）

### 14.1 类型宽度要与平台匹配

```c
uint8_t  /* 无符号 8 位  —— 0~255 */
uint16_t /* 无符号 16 位 —— 0~65535 */
int16_t  /* 有符号 16 位 —— -32768~32767 */
int32_t  /* 有符号 32 位 */
```

**为什么不用 `int`/`short`**：它们的宽度由平台决定（PC 上 `int` 是 32 位，
某些 8 位单片机上 `int` 是 16 位）。**跨平台可移植的代码必须用 `stdint.h` 的固定宽度类型。**
本工程全部使用固定宽度类型 —— 这是正确的做法。

**窄化转换的坑**（本工程有大量 `(uint16_t)` 强转）：

```c
/* 来源：imu/pid.c */
motorPWMValue[0] = (uint16_t)motor[1] - MOTORSTOPVAL;
```

这里 `motor[1]` 是 `int16_t`。若它小于 1000（例如 PID 修正把它压到 900），
`900 - 1000 = -100`，强转成 `uint16_t` 会变成 **65436**（回绕），
PWM 输出就会完全失控。**看到强制类型转换，先问：这个值可能为负/溢出吗？**

### 14.2 `sizeof` 与数组退化

```c
int arr[10];
sizeof(arr)        /* = 40（整个数组） */
void f(int arr[])  /* 形参里 arr 退化成了指针 */
{
    sizeof(arr)    /* = 4（指针大小，不是 40！）—— 经典陷阱 */
}
```

**因此**：把数组传给函数时**必须同时传长度**。
本工程的做法就是显式传长度：

```c
/* 来源：imu/mlib.c */
double quickMedianFilter3(double * v);      /* 3 点 —— 长度写进函数名 */
double quickMedianFilter5(double * v);
double quickMedianFilter7(double * v);
```

**另一个惯用法**：用 `sizeof` 求数组元素个数

```c
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
```

### 14.3 中断服务函数（ISR）的写法约束

```c
/* 来源：user/py32f0xx_it.c —— 名字必须与向量表一致 */
void TIM17_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle_17);
}
```

**三条铁律**：
1. **函数名不能改** —— 它由启动文件里的向量表按名字引用；
2. **必须短、不能阻塞** —— ISR 里不能有延时、不能等硬件、不能 `printf` 长字符串；
3. **与主循环共享的变量要 `volatile`**，且注意原子性。

**本工程的做法**（推荐模式）：
ISR 里只做"置标志/加计数"，重量级处理放到主循环：

```c
/* 来源：drivers/bsp/bsp_timer.c —— 中断里只加各种计时计数 */
if (htim->Instance == TIM17) {
    baroUpdateTimer++;
    altitudeCtrlTimer++;
    _cnt10ms++;
    ...
}
/* 主循环里再根据这些计数决定何时做气压更新等重活 */
```

这叫**"中断打标记、主循环干活"**，能让中断保持极短，
是飞控稳定运行的关键设计。

### 14.4 字符串与转义字符

```c
printf("offset: %d, %d, %d\r\n", a, b, c);
```

| 转义 | 含义 | 为什么用 |
| --- | --- | --- |
| `\r\n` | 回车 + 换行 | 串口终端的标准换行（只写 `\n` 在部分终端不换行） |
| `\t` | 制表符 | 对齐打印 |
| `\0` | 空字符 | **C 字符串的结束标志** |
| `\\` | 反斜杠本身 | 路径、正则里要用 |
| `%%` | 百分号本身 | `printf` 里打印 `%` |

**坑**：`printf("...%d", float_var)` 用错格式符会导致**未定义行为**（不是简单显示错）。
本工程大量使用 `printf` 调试，改类型时记得同步改格式符。

---

## 15. 速查表：本工程关键字使用统计

统计范围：`飞控/` 下自有代码 + 项目驱动（排除 HAL/CMSIS 库）。

| 关键字/写法 | 出现次数 | 本工程主要用途 |
| --- | --- | --- |
| `struct` | 594 | 数据结构打包（fc、accgyro、滤波器句柄） |
| `|=` 位或赋值 | 634 | 硬件寄存器置位 |
| `typedef` | 157 | 结构体/枚举别名（`_s` `_e` `_t` 后缀） |
| `enum` | 144 | 轴索引、状态码、滤波器类型 |
| `extern` | 138 | 跨文件共享全局变量 |
| `const` | 184 | 只读常量、参数承诺 |
| `static` | 262 | 模块私有 + 跨调用保持状态 |
| `<<` 左移 | 108 | 位掩码、快速乘除 |
| `&=` 位与赋值 | 96 | 硬件寄存器清位 |
| `#define` 宏函数 | 98 | 数学宏、寄存器操作宏 |
| `volatile` | 22 | 中断共享变量、硬件寄存器 |
| `switch` | 19 | 状态机、类型分派 |
| `do-while` | 17 | 至少执行一次的重试/读取 |
| `union` | 13 | 多字节数据拆解 |
| 函数指针 | 12 | 滤波器类型抽象 |
| `sizeof` | 12 | 数组长度、结构体大小 |
| `__weak` | 9 | HAL 回调默认实现 |
| `__attribute__` | 6 | `naked`、`packed` |
| `goto` | 2 | 错误统一清理出口 |

**读代码时的优先级建议**：先弄清 `static`（哪些状态是持久的）、
再看 `volatile`（哪些变量有跨执行流的共享）、
最后看位运算（硬件到底被怎么配置的）。这三类是最容易出 bug 的地方。

---

## 附：与本工程其它文档的关系

| 文档 | 内容 |
| --- | --- |
| 本文档 | **语言层面**：关键字用法与变式 |
| `COMMENT_STANDARD.md` | **注释规范**：写注释的格式与纪律 |
| `VSCODE_使用说明.md` | **工具链**：如何在 VS Code 里编译/烧录/调试 |
