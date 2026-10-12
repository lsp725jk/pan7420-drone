/**
 * 文件名 : config_pn298.h
 */
/**
 * 文件：drivers/bsp/config_rf.h            【角色】射频参数总配置（唯一改射频
 */
// -------- <<< Use Configuration Wizard in Context Menu >>> --------------------
#ifndef __CONFIG_PN298_H
#define __CONFIG_PN298_H



// <h>RF配置
// <o>芯片型号 <0x00=>PAN7420 <0x01=>PAN7421

#define __CHIP_TYPE                 0x00



// <o>PAYLOAD（有效载荷） <1-64>

#define __PN298_PAYLOAD             12

// <o>初始通道(频点) <0-255>

#define __PN298_INIT_CHN             36

// <h>初始地址
// <o>地址长度 <3=>3字节 <4=>4字节 <5=>5字节

#define __PN298_INIT_ADDR_LEN        5
// <o0>地址第1字节 <0-255>
// <o1>地址第2字节 <0-255>
// <o2>地址第3字节 <0-255>
// <o3>地址第4字节 <0-255>
// <o4>地址第5字节 <0-255>

#define __PN298_INIT_ADDR_BYTE0      0xCC
#define __PN298_INIT_ADDR_BYTE1      0xCC
#define __PN298_INIT_ADDR_BYTE2      0xCC
#define __PN298_INIT_ADDR_BYTE3      0xCC
#define __PN298_INIT_ADDR_BYTE4      0xCC
/* 【预处理器条件编译讲解】 下面这段是“按地址长度自动拼出地址数组”的写法，是 C 预处理器的经典技巧： #if / #elif / #else / #endif —— 编译**之前**由预处理器判断， 只保留成立的那一个分支，其余分支的文本被整段丢弃（不参与编译、不占空间）。 */
#if   __PN298_INIT_ADDR_LEN == 3
    #define __PN298_INIT_ADDR        {__PN298_INIT_ADDR_BYTE0,__PN298_INIT_ADDR_BYTE1,__PN298_INIT_ADDR_BYTE2}
#elif __PN298_INIT_ADDR_LEN == 4                                                  
    #define __PN298_INIT_ADDR        {__PN298_INIT_ADDR_BYTE0,__PN298_INIT_ADDR_BYTE1,__PN298_INIT_ADDR_BYTE2,__PN298_INIT_ADDR_BYTE3}
#elif __PN298_INIT_ADDR_LEN == 5                                                                       
    #define __PN298_INIT_ADDR        {__PN298_INIT_ADDR_BYTE0,__PN298_INIT_ADDR_BYTE1,__PN298_INIT_ADDR_BYTE2,__PN298_INIT_ADDR_BYTE3,__PN298_INIT_ADDR_BYTE4}
#else
    #error rf通信地址长度错误
#endif
// </h>

// <o>RF模式
// <0x00=>xn297L普通型模式
// <0x01=>xn297L增强型模式
// <0x10=>ble模式
// <0x20=>nordic普通型模式
// <0x21=>nordic增强型模式
/* PN298 的工作模式编号。编号含义是“高 4 位选协议族、bit0 选普通/增强型”， 详见 bsp_pn298.h 里 PN298_*_MODE 那一段的注释。 【为什么必须是 0x01】对本项目来说，0x01（XN297L 增强型）是唯一满足以下 全部要求的模式： */
#define __PN298_MODE                0x0001
	
// <o>通信速率 <0x00=>1M <0x40=>2M <0xC0=>250K
/* 空中数据速率（air data rate）。 0x00 = 1Mbps（当前值） 0x40 = 2Mbps 0xC0 = 250Kbps 【怎么选】速率不是越快越好，它和“距离/抗干扰”是互相矛盾的： · 速率越高 → 同样一包数据占用空中的时间越短，单位时间能传的包更多， */
#define __PN298_RATE                 0x00

// <o>PN298功率
// <0xc9ca=>10dBm
// <0xc9c2=>6dBm
// <0x49c6=>3dBm
// <0x49c2=>2Bm
// <0xc9c5=>1dBm
// <0xc9c1=>0dBm
// <0x49c1=>-8dBm

#define __PN298_POWER                0xC9Ca





// </h>

#endif //__CONFIG_PN298_H
/* 【注意（疑似问题，未改动）】 1) 文件头注释写的是“文件名 : config_pn298.h”，而实际文件名是 config_rf.h， 属于改名时漏改注释，不影响编译。 2) 本文件里 __CHIP_TYPE 在 bsp_pn298.c 中被当作**布尔量**使用 */
