#include <stdint.h>
#include "stm32l4xx.h"
#include "Time.h"

#ifdef ENABLE_US_PRECISION
Tick SYSTEM_TICK_VAL = 0;
void Time_Init()
{
	SysTick_Config(SystemCoreClock/SYSTEM_TICK_DEFINITION); // set up SysTick
}

void IncrementTick(void)
{
  ++SYSTEM_TICK_VAL;
}


Tick GetTime_ms(void)
{
	return (SYSTEM_TICK_VAL / MS_TO_US_VAL);
}

Tick GetTime_us(void)
{
	return SYSTEM_TICK_VAL;
}

Tick GetTimeSince_ms(Tick time)
{
	return ((SYSTEM_TICK_VAL/MS_TO_US_VAL) - time);
}

Tick GetTimeSince_us(Tick time)
{
	return (SYSTEM_TICK_VAL - time);
}

#else

Tick SYSTEM_TICK_VAL = 0;
void Time_Init()
{
	  SysTick_Config(SystemCoreClock/SYSTEM_TICK_DEFINITION); // set up SysTick
}

void IncrementTick(void)
{
  ++SYSTEM_TICK_VAL;
}


Tick GetTime_ms(void)
{
	return SYSTEM_TICK_VAL;
}

Tick GetTimeSince_ms(Tick time)
{
	return (SYSTEM_TICK_VAL - time);
}

#endif
