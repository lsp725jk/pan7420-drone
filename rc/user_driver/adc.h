#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "board.h"

#define ADC_SAMPLE_NUM	6
	 
extern ADC_HandleTypeDef hadc;
extern uint16_t aADCxConvertedData[];
void MX_ADC_Init(void);



#ifdef __cplusplus
}
#endif

#endif
