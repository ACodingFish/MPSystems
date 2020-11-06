#ifndef _ADC_H_
#define _ADC_H_
#include "stm32l4xx.h"
typedef enum ADCType
{
	ADC_MOISTURE, //A5, PC14
	ADC_COUNT,
}ADCType_t;

void ADC_Init(ADC_HandleTypeDef *hadc, ADCType_t adc_index);
void ADC_ReadTask(void);
uint16_t ADC_Read(uint16_t ADC_index);

#endif //_ADC_H_
