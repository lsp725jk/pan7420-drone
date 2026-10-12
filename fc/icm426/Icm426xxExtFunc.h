/**
 * and other intellectual property rights laws.
 */

/**
 * @defgroup Extfunc External functions @brief    External functions to be defined in application level. @{
 */

/**
 * @file Icm426xxExtFunc.h
 */

#ifndef _INV_ICM426XX_EXTFUNC_H_
#define _INV_ICM426XX_EXTFUNC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif



extern void inv_icm426xx_sleep_us(uint32_t us);



extern uint64_t inv_icm426xx_get_time_us(void);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM426XX_EXTFUNC_H_ */

/* @} */
