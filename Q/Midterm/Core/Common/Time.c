#include <stdint.h>
#include <Time.h>
#include "stm32l4xx.h"

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

void TimerInit(Timer* timer)
{
	timer->start_time = 0;
}

char TimerIsExpired(Timer* timer)
{
	return (GetTimeSince_ms(timer->start_time) > timer->timer_period);
}
void TimerCountdownMS(Timer* timer, unsigned int time)
{
	// set time
	timer->start_time = GetTime_ms();
	timer->timer_period = time;
}
void TimerCountdown(Timer* timer, unsigned int time)
{
	// seconds
	timer->start_time = GetTime_ms();
	timer->timer_period = MS(time)*1000;
}

int TimerLeftMS(Timer* timer)
{
	return (int)((int64_t)timer->timer_period - (int64_t)GetTimeSince_ms(timer->start_time));
}
