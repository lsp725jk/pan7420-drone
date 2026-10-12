/**
 * ch08_ex14_once.h —— 变式：#pragma once（非标准但是事实标准）
 */
#pragma once

struct BaroSample {
    int pressure_pa;
    int temperature;
};

int baro_read_raw(void);
