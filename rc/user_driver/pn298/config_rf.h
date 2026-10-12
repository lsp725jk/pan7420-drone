/**
 * 文件名 : config_pn298.h
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
#define __PN298_MODE                0x01
	
// <o>通信速率 <0x00=>1M <0x40=>2M <0xC0=>250K
#define __PN298_RATE                 0x00

// <o>PN298功率
// <0xc9ca=>10dBm
// <0xc9c2=>6dBm
// <0x49c6=>3dBm
// <0x49c2=>2Bm
// <0xc9c5=>1dBm
// <0xc9c1=>0dBm
// <0x49c1=>-8dBm
#define __PN298_POWER                0xc9ca





// </h>

#endif //__CONFIG_PN298_H

