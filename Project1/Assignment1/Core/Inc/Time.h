#ifndef _TIME_H_
#define _TIME_H_
//do not forget to initialize in stm32l4xx_it.c systick handler
#include <stdint.h>
#define Tick uint64_t

#ifdef ENABLE_US_PRECISION
	#define SYSTEM_TICK_US_DEFINITION 1000000ULL
	#define	MS_TO_US_VAL 1000ULL
	#define MS(x) (x*MS_TO_US_VAL)
#else
	#define SYSTEM_TICK_US_DEFINITION 1000ULL
	#define TICKS_PER_MS 1
	#define MS(x) (x)
#endif

void Time_Init();
void IncrementTick(void);


Tick GetTime_ms(void);
Tick GetTimeSince_ms(Tick time);

#ifdef ENABLE_US_PRECISION
Tick GetTime_us(void);
Tick GetTimeSince_us(uint64_t time);
#endif

#endif //_TIME_H_
