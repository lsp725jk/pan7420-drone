/**
 * 文件：imu/mlib.h                   【角色】数学工具库（全项目的"计算器"）
 */

#ifndef __MLIB_H
#define __MLIB_H
// ------------------------------------------------------------------------------------
#include "bsp.h" 



// ------------------------------------------------------------------------------------

#define M_PI      (3.14159265358979323846f)



/* ===== 常用宏 ===== 【语法讲解：这些是"带参数的宏"，本质是文本替换，不是函数】 为什么每个参数和整体都要加括号？因为宏不遵守运算优先级： #define BAD_SQ(x)  x*x */
#define SQ(x) ((x)*(x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(x) ((x) > 0 ? (x) : -(x))



/* #ifndef 条件编译：避免重复定义（同一个宏若被定义两次，编译器会警告/报错）。 变式：`#ifdef X` 只要"定义过"就成立（哪怕定义成 0）； `#if defined(X) && !defined(Y)` 可组合多个条件，推荐这种写法。 */
#ifndef TRUE
  #define TRUE  1
  #define FALSE 0
#endif

/* 三轴枚举。 【关键字：typedef enum —— 给枚举起别名】 不写 typedef 的话，使用时要写 `enum axis_e x;`；起别名后就是 `axis_e x;`。 【变式】枚举值可以显式赋值，也可以自动递增（下面 Y=1、Z=2 是自动的）： */
typedef enum {
    X = 0,
    Y,
    Z
} axis_e;

// ------------------------------------------------------------------------------------
/* 限幅（整数版）：把 a 夹在 [x, y] 区间内。 用法：constrain(1500+BaroPID, 1000, 2000) —— 保证电调信号不越界。 */
int16_t constrain(int16_t a,int16_t x,int16_t y);
/* 限幅（浮点版） */
float constrain_float(float a,float x,float y);
/* 取绝对值。 【为什么叫 mlib_abs 而不是 abs】 标准库 stdlib.h 里有 int abs(int)。本工程的 int32_t 在 ARM 上就是 long， 所以自定义的 long abs(long) 与标准库的 int abs(int) 是**签名冲突**， */
int32_t mlib_abs(int32_t v);
/* 取较大值。命名理由同 mlib_abs（避开标准库/宏同名） */
int16_t mlib_max(int16_t x,int16_t y);
/* 死区处理：见 mlib.c 的详细说明。遥控摇杆中位抖动全靠它消除。 */
int32_t applyDeadband(int32_t value, int32_t deadband);



/* 快速中值滤波（3/5/7 点）。 【为什么需要中值滤波而不是平均滤波】 平均滤波遇到"突发野值"（比如电机干扰导致 ADC 跳变）会被拉偏； 中值滤波取排序后的中间值，单个野值直接被丢弃，抗脉冲干扰更强。 */
double quickMedianFilter3(double * v);
double quickMedianFilter5(double * v);
double quickMedianFilter7(double * v);

/* 快速平方根倒数：算 1/√x。 姿态解算里要把向量归一化，本质就是反复算 1/√(x²+y²+z²)， 所以这个函数被调用得非常频繁，用"魔数 + 牛顿迭代"的近似算法实现。 */
float invSqrt(float x);



float sin_approx(float x);
float cos_approx(float x);
float atan2_approx(float y, float x);
float acos_approx(float x);

#endif



