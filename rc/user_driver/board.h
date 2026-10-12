#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "py32f0xx_hal.h"
#include "stdlib.h"

#define CRSF_EN

void APP_ErrorHandler(void);
void APP_SystemClockConfig(void);
void init_random_seed(void) ;
#ifdef __cplusplus
}
#endif

#endif
