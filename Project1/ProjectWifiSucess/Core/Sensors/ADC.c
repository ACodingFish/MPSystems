#include <stdint.h>
#include <stdbool.h>
#include "Time.h"
#include "Common.h"
#include "ADC.h"

typedef struct ADC
{
	uint16_t val;
	ADC_HandleTypeDef *hadc;
	Tick timer;
} ADC_t;

#define ADC_POLL_TIME_MS MS(100)

ADC_t ADC_list[ADC_COUNT];
uint16_t ADC_READY_MASK = 0x0;

void ADC_Init(ADC_HandleTypeDef *hadc, ADCType_t adc_index)
{
	ADC_list[adc_index].hadc = hadc;
	ADC_list[adc_index].val = 0;
	ADC_list[adc_index].timer = GetTime_ms(); // initialize ADC Timer
	HAL_ADC_Start_IT(hadc);
}

void ADC_ReadTask(void)
{
	for (uint16_t i = 0; i < ADC_COUNT; i++)
	{
		if ((GetTimeSince_ms(ADC_list[i].timer)>ADC_POLL_TIME_MS)&&(FLAG_CHK_FLAG(ADC_READY_MASK,i)!=false))
		{
			ADC_list[i].timer = GetTime_ms(); // reset timer

			ADC_list[i].val = HAL_ADC_GetValue(ADC_list[i].hadc); // get ADC Reading

			FLAG_CLR_FLAG(ADC_READY_MASK,i); // restart the interrupt/flags.
			HAL_ADC_Start_IT(ADC_list[i].hadc);

		}
	}
}

uint16_t ADC_Read(uint16_t ADC_index)
{
	return ADC_list[ADC_index].val; // get adc reading
}

// callback for ADC value fetch complete
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	for (int i = 0; i < ADC_COUNT; i++)
	{
		if (ADC_list[i].hadc == hadc)
		{
			FLAG_SET_FLAG(ADC_READY_MASK,i);
		}
	}
}


