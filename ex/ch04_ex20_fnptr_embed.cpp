/**
 * ch04_ex20_fnptr_embed.c —— 函数指针在嵌入式里的四种真实用法
 */
#include <stdio.h>
#include <string.h>

/**
 * ---- 1. 驱动接口：结构体 + 函数指针（"虚函数表"的 C 版）----
 */
typedef struct device_s device_t;       /**
 * 前向声明，支持自引用
 */

struct device_s {
    const char *name;
    int  (*init)(device_t *self);
    int  (*read)(device_t *self, int *out);
    int  (*write)(device_t *self, int val);
};

/* 具体驱动 A：模拟一个传感器 */
static int sensor_init(device_t *self)  { (void)self; return 0; }
static int sensor_read(device_t *self, int *out)
{
    (void)self;
    if (out == NULL) { return -1; }
    *out = 1234;
    return 0;
}
static int sensor_write(device_t *self, int val)
{
    (void)self; (void)val;
    return -1;                                  /* 只读设备，写操作不支持 */
}

/* 具体驱动 B：模拟一个执行器 */
static int servo_init(device_t *self)   { (void)self; return 0; }
static int servo_read(device_t *self, int *out)
{
    (void)self;
    if (out == NULL) { return -1; }
    *out = 1500;                                /* 当前 PWM 值 */
    return 0;
}
static int servo_write(device_t *self, int val)
{
    (void)self;
    if (val < 1000 || val > 2000) { return -2; }
    return 0;
}

static device_t g_sensor = { "sensor", sensor_init, sensor_read, sensor_write };
static device_t g_servo  = { "servo",  servo_init,  servo_read,  servo_write };

/* 统一的调用入口：调用者不需要知道具体是哪个设备 */
static int device_do_read(device_t *dev, int *out)
{
    if (dev == NULL || dev->read == NULL) {
        return -1;                                      /* 检查：结构体空 或 函数指针没注册 */
    }
    return dev->read(dev, out);
}

static int device_do_write(device_t *dev, int val)
{
    if (dev == NULL || dev->write == NULL) {
        return -1;
    }
    return dev->write(dev, val);
}

/* ---- 2. 事件回调注册（主循环只负责分发）---- */
typedef void (*event_cb_t)(int event_id);

static void cb_led(int id)     { printf("    回调 LED: 事件 %d\n", id); }
static void cb_buzzer(int id)  { printf("    回调 蜂鸣器: 事件 %d\n", id); }

#define MAX_CB 4
static event_cb_t g_callbacks[MAX_CB];
static int        g_cb_count = 0;

static int register_cb(event_cb_t cb)
{
    if (cb == NULL || g_cb_count >= MAX_CB) {
        return -1;
    }
    g_callbacks[g_cb_count++] = cb;
    return 0;
}

static void dispatch_event(int id)
{
    for (int i = 0; i < g_cb_count; i++) {
        if (g_callbacks[i] != NULL) {                   /* 每项都要判空 */
            g_callbacks[i](id);
        }
    }
}

int main(void)
{
    printf("---- 1. 驱动表：一套接口，多种实现 ----\n");
    device_t *devs[2] = { &g_sensor, &g_servo };
    for (int i = 0; i < 2; i++) {
        int v = 0;
        int rc_init = (devs[i]->init != NULL) ? devs[i]->init(devs[i]) : -1;
        int rc_read = device_do_read(devs[i], &v);
        printf("  %-7s init=%d read=%d 值=%d\n",
               devs[i]->name, rc_init, rc_read, v);
    }
    printf("  写操作：sensor 支持吗? %d（-1 表示不支持）\n",
           device_do_write(&g_sensor, 100));
    printf("          servo  写 1500: %d\n", device_do_write(&g_servo, 1500));
    printf("          servo  写 3000: %d（超范围，返回 -2）\n",
           device_do_write(&g_servo, 3000));

    printf("---- 2. 回调注册与分发 ----\n");
    register_cb(cb_led);
    register_cb(cb_buzzer);
    register_cb(NULL);                          /* 注册空指针会被拒绝 */
    printf("  已注册 %d 个回调，分发事件 7：\n", g_cb_count);
    dispatch_event(7);
    printf("  主循环不需要知道事件 7 该干什么 —— 那是注册者的事。\n");

    printf("---- 3. 表项没注册是常态，必须判空 ----\n");
    event_cb_t empty_table[MAX_CB] = { NULL, NULL, NULL, NULL };
    int called = 0;
    for (int i = 0; i < MAX_CB; i++) {
        if (empty_table[i] != NULL) {
            empty_table[i](1);
            called++;
        }
    }
    printf("  全空表分发了一次，实际调用 %d 个回调（安全跳过）\n", called);
    printf("  如果少了这个判空，就是跳到 0 地址 -> 必崩。\n");

    printf("---- 4. 嵌入式里的典型用途 ----\n");
    printf("  a) 驱动表：同一套 upper layer 代码配不同传感器；\n");
    printf("  b) 事件回调：按键、串口收包、定时器到点；\n");
    printf("  c) 状态机跳转表：state_table[state]() 一行搞定分派；\n");
    printf("  d) 命令解析表：命令字 -> 处理函数。\n");
    printf("  【代价】没有编译期检查，表项写错/漏注册只能在运行期发现，\n");
    printf("         所以：表要 static const（防误改），调用前一定判空。\n");
    return 0;
}
