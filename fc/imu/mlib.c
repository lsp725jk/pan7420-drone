/**
 * 文件：imu/mlib.c                   【角色】数学工具库的实现
 */



#include "mlib.h"
#include <math.h>

/**
 * @brief  整数限幅：把 a 限制在 [x, y] 区间 【用法】constrain(1500+BaroPID, 1000, 2000) —— 保证电调信号不越界 【关联】pid.c 每 1ms 调用多次；这是全项目最热的函数之一
 */
int16_t constrain(int16_t a,int16_t x,int16_t y)
{
	if(a < x) a = x;
	if(a > y) a = y;

	return a;
}

/* @brief  浮点限幅，语义同 constrain()，用于增益等浮点量的钳位 */
float constrain_float(float a,float x,float y)
{
	if(a < x) a = x;
	if(a > y) a = y;

	return a;
}



int32_t mlib_abs(int32_t v)
{
  return v>0?v:-v;
}

/* @brief  取较大值。命名理由同 mlib_abs。 */
int16_t mlib_max(int16_t x,int16_t y)
{
	if(x > y) return x;
	else return y;
}

/* @brief  死区处理：把"接近零的小值"压成零，并把其余值平移掉死区 【算法】为什么这样写 —— 这是遥控手感的关键函数 遥控器摇杆在中位时，ADC 读数总在 ±几 个 LSB 抖动， 如果直接当输入用，飞机会一直有微小修正（"抖舵"）。 */
int32_t applyDeadband(int32_t value, int32_t deadband)
{
    if (mlib_abs(value) < deadband) {
        value = 0;
    } else if (value > 0) {
        value -= deadband;
    } else if (value < 0) {
        value += deadband;
    }
    return value;
}



#define QMF_SORT(a,b) { if ((a)>(b)) QMF_SWAP((a),(b)); }
#define QMF_SWAP(a,b) { double temp=(a);(a)=(b);(b)=temp; }
#define QMF_COPY(p,v,n) { uint16_t i; for (i=0; i<n; i++) p[i]=v[i]; }

/* @brief 3 点中值滤波（返回 3 个数的中间值），用于快速去野值 */
double quickMedianFilter3(double * v)
{
    double p[3];
    QMF_COPY(p, v, 3);

                /* 3 次比较即可，返回 p[1] 就是中位数 */
    QMF_SORT(p[0], p[1]); QMF_SORT(p[1], p[2]); QMF_SORT(p[0], p[1]) ;
    return p[1];
}

/* @brief 5 点中值滤波，7 次比较 */
double quickMedianFilter5(double * v)
{
    double p[5];
    QMF_COPY(p, v, 5);

    QMF_SORT(p[0], p[1]); QMF_SORT(p[3], p[4]); QMF_SORT(p[0], p[3]);
    QMF_SORT(p[1], p[4]); QMF_SORT(p[1], p[2]); QMF_SORT(p[2], p[3]);
    QMF_SORT(p[1], p[2]); 
    return p[2];
}

/* @brief 7 点中值滤波，13 次比较。点数越多抗干扰越强，但延迟也越大 */
double quickMedianFilter7(double * v)
{
    double p[7];
    QMF_COPY(p, v, 7);

    QMF_SORT(p[0], p[5]); QMF_SORT(p[0], p[3]); QMF_SORT(p[1], p[6]);
    QMF_SORT(p[2], p[4]); QMF_SORT(p[0], p[1]); QMF_SORT(p[3], p[5]);
    QMF_SORT(p[2], p[6]); QMF_SORT(p[2], p[3]); QMF_SORT(p[3], p[6]);
    QMF_SORT(p[4], p[5]); QMF_SORT(p[1], p[4]); QMF_SORT(p[1], p[3]);
    QMF_SORT(p[3], p[4]);
    return p[3];
}



/* @brief  快速平方根倒数：返回 1/√x 【算法讲解 —— 这是图形学与嵌入式里的著名技巧（"雷神之锤"魔数）】 常规做法 1.0f/sqrtf(x) 要调用库函数，慢且占空间。 这里用两步逼近： */
float invSqrt(float x) 
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;          		/* 类型双关：把 float 的 32 位模式当作整数 */
	i = 0x5f3759df - (i>>1);      		/* 魔数 + 右移一位 = 平方根的倒数初值 */
	y = *(float*)&i;              		/* 读回浮点 */
	y = y * (1.5f - (halfx * y * y));   		/* 一次牛顿迭代，精度提升到约 1e-3 */
	return y;
}



// ----------------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------------

#define sinPolyCoef3 -1.666665710e-1f                                       // Double: -1.666665709650470145824129400050267289858e-1
#define sinPolyCoef5  8.333017292e-3f                                       // Double:  8.333017291562218127986291618761571373087e-3
#define sinPolyCoef7 -1.980661520e-4f                                       // Double: -1.980661520135080504411629636078917643846e-4
#define sinPolyCoef9  2.600054768e-6f                                       // Double:  2.600054767890361277123254766503271638682e-6

/* @brief  快速正弦（输入/输出均为弧度） 【算法】分三步 1) 范围检查：|x| > 32 弧度（约 5 圈）认为是非法输入，直接返回 0。 为什么要有这一步：多项式只在有限区间内有效，输入跑飞时必须兜住， */
float sin_approx(float x)
{
    int32_t xint = x;
	  float x2;
	
    if (xint < -32 || xint > 32) return 0.0f;                               // Stop here on error input (5 * 360 Deg)
    while (x >  M_PI) x -= (2.0f * M_PI);                                   // always wrap input angle to -PI..PI
    while (x < -M_PI) x += (2.0f * M_PI);
    if (x >  (0.5f * M_PI)) x =  (0.5f * M_PI) - (x - (0.5f * M_PI));       // We just pick -90..+90 Degree
    else if (x < -(0.5f * M_PI)) x = -(0.5f * M_PI) - ((0.5f * M_PI) + x);
    x2 = x * x;
    return x + x * x2 * (sinPolyCoef3 + x2 * (sinPolyCoef5 + x2 * (sinPolyCoef7 + x2 * sinPolyCoef9)));
}

/* @brief  快速余弦 【算法】直接复用正弦：cos(x) = sin(x + π/2)。 省掉一整套余弦系数与规约逻辑，代价只是多一次加法 —— 这就是"用关系式复用"的典型手法。 */
float cos_approx(float x)
{
    return sin_approx(x + (0.5f * M_PI));
}



float atan2_approx(float y, float x)
{
    #define atanPolyCoef1  3.14551665884836e-07f
    #define atanPolyCoef2  0.99997356613987f
    #define atanPolyCoef3  0.14744007058297684f
    #define atanPolyCoef4  0.3099814292351353f
    #define atanPolyCoef5  0.05030176425872175f
    #define atanPolyCoef6  0.1471039133652469f
    #define atanPolyCoef7  0.6444640676891548f

    float res, absX, absY;
    absX = ABS(x);                         /* 注意：这里用的是 mlib.h 的宏 ABS，不是函数 */
    absY = ABS(y);
    res  = MAX(absX, absY);                /* 取较大者作分母，保证比值 ≤ 1 */
    if (res) res = MIN(absX, absY) / res;           /* res = min/max ∈ [0,1]，即 tan(小角度) */
    else res = 0.0f;                       /* 防止 x=y=0 时除零 */
                /* 有理式逼近：分子是 4 次多项式，分母是 2 次多项式 */
    res = -((((atanPolyCoef5 * res - atanPolyCoef4) * res - atanPolyCoef3) * res - atanPolyCoef2) * res - atanPolyCoef1) / ((atanPolyCoef7 * res + atanPolyCoef6) * res + 1.0f);
                /* 下面三行：把 [0, π/4] 的结果还原到正确象限 —— 顺序不能换 */
    if (absY > absX) res = (M_PI / 2.0f) - res;
    if (x < 0) res = M_PI - res;
    if (y < 0) res = -res;
    return res;
}



float acos_approx(float x)
{
    float xa = ABS(x);
    float result = sqrtf(1.0f - xa) * (1.5707288f + xa * (-0.2121144f + xa * (0.0742610f + (-0.0187293f * xa))));
    if (x < 0.0f)
        return M_PI - result;
    else 
        return result;
}



