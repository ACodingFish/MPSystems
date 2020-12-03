#include "stm32l4xx_hal.h"
#include <math.h>
#include "Waveform_Generator.h"
extern DAC_HandleTypeDef hdac1;


#define NUM_SAMPLES 100
#define MIN_VOLTAGE 0.0f
#define MAX_VOLTAGE 3.3f
uint32_t dac_sine[NUM_SAMPLES];
#define PI 3.1415926

volatile uint32_t wave_index = 0;
WAVEFORM_T waveform_type = W_SINE;

void WG_Init(void)
{
  for (int i = 0; i < NUM_SAMPLES; i++)
  {
	  dac_sine[i] = (sin(i*2*PI/NUM_SAMPLES)+1)*((0xFFF+1)/2);
  }
  HAL_DAC_Start(&hdac1, DAC1_CHANNEL_2);
}

uint32_t WG_Volt_to_DAC(float volt)
{
	uint32_t dac_volt = (uint32_t)volt*(0xFFF+1)/MAX_VOLTAGE;
	return dac_volt;
}

uint32_t WG_Get_DAC_Val(void)
{
	uint32_t ret = 0;
	switch(waveform_type)
	{
		case W_SINE:
			ret = dac_sine[wave_index];
			break;
		case W_SQUARE:
			if (wave_index < (NUM_SAMPLES>>1))
			{
				ret = WG_Volt_to_DAC(MIN_VOLTAGE);
			} else
			{
				ret = WG_Volt_to_DAC(MAX_VOLTAGE);
			}
			break;
		case W_TRIANGLE:
			if (wave_index < (NUM_SAMPLES>>1))
			{
				ret = WG_Volt_to_DAC(((float)wave_index/(float)(NUM_SAMPLES>>1))*MAX_VOLTAGE);
			} else
			{
				ret = WG_Volt_to_DAC(((1.0f - (float)(wave_index - (NUM_SAMPLES>>1))/(float)(NUM_SAMPLES>>1)))*MAX_VOLTAGE);
			}
			break;
		default:
			waveform_type = W_SINE;
			ret = 0;
			break;
	}
	return ret;


}

void WG_Cycle_Waveform(void)
{
	waveform_type++;
	waveform_type %= W_COUNT;
}

void WG_Timer_Callback(void)
{
	uint32_t dac_volt = WG_Get_DAC_Val();
	HAL_DAC_SetValue(&hdac1, DAC1_CHANNEL_2, DAC_ALIGN_12B_R, dac_volt);
	wave_index++;
	if (wave_index > NUM_SAMPLES)
	{
		wave_index = 0;
	}
}



