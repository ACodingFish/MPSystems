#ifndef _WAVEFORM_GENERATOR_H_
#define _WAVEFORM_GENERATOR_H_

typedef enum
{
	W_SINE,
	W_SQUARE,
	W_TRIANGLE,
	W_COUNT,
} WAVEFORM_T;
void WG_Init(void);
uint32_t WG_Get_DAC_Val(void);
void WG_Timer_Callback(void);
void WG_Cycle_Waveform(void);
#endif //_WAVEFORM_GENERATOR_H_
