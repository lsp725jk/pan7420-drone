/**
 * and other intellectual property rights laws.
 */

/**
 * @defgroup SelfTest Self-test @brief    API to execute self-test procedures. @{
 */

/**
 * @file Icm426xxSelfTest.h
 */

#ifndef _INV_ICM426XX_SELFTEST_H_
#define _INV_ICM426XX_SELFTEST_H_

#include "InvExport.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * forward declaration
 */
struct inv_icm426xx;



int inv_icm426xx_run_selftest(struct inv_icm426xx *s, int *result);



int inv_icm426xx_get_st_bias(struct inv_icm426xx *s, int st_bias[6]);



int inv_icm426xx_set_st_bias(struct inv_icm426xx *s, const int st_bias[6]);

#ifdef __cplusplus
}
#endif

#endif /* _INV_ICM426XX_SELFTEST_H_ */

/* @} */
