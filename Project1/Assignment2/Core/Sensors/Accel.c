#include <stdint.h>
#include "stm32l475e_iot01_accelero.h"
#include "Time.h"
#include "Accel.h"

void AccelRead(void);

#define ACCEL_DELAY MS(100)
Tick accel_timer = 0;

int16_t accel_vals[3] = {0, 0, 0};

uint32_t AccelInit(void)
{
	return (uint32_t)(ACCEL_StatusTypeDef)BSP_ACCELERO_Init();
}

void AccelRead(void)
{
	BSP_ACCELERO_AccGetXYZ(accel_vals);
}

float AccelReadX(void)
{
	if (GetTimeSince_ms(accel_timer) >= ACCEL_DELAY)
	{
		AccelRead();
		accel_timer = GetTime_ms();
	}
	return (float)accel_vals[AAxisX];
}

float AccelReadY(void)
{
	if (GetTimeSince_ms(accel_timer) >= ACCEL_DELAY)
	{
		AccelRead();
		accel_timer = GetTime_ms();
	}
	return (float)accel_vals[AAxisY];
}

float AccelReadZ(void)
{
	if (GetTimeSince_ms(accel_timer) >= ACCEL_DELAY)
	{
		AccelRead();
		accel_timer = GetTime_ms();
	}
	return (float)accel_vals[AAxisZ];
}
