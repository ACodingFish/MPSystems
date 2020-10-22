#include <stdint.h>
#include "stm32l475e_iot01_gyro.h"
#include "Time.h"
#include "Gyro.h"


//Gyro doesn't work for some reason...
void GyroRead(void);

#define GYRO_DELAY MS(100)
Tick gyro_timer = 0;

float gyro_vals[3] = {0, 0, 0};

uint32_t GyroInit(void)
{
	return (uint32_t)(GYR_StatusTypeDef)BSP_GYRO_Init();
}

void GyroRead(void)
{
	BSP_GYRO_GetXYZ(gyro_vals);
}

float GyroReadX(void)
{
	if (GetTimeSince_ms(gyro_timer) >= GYRO_DELAY)
	{
		GyroRead();
		gyro_timer = GetTime_ms();
	}
	return gyro_vals[GAxisX];
}

float GyroReadY(void)
{
	if (GetTimeSince_ms(gyro_timer) >= GYRO_DELAY)
	{
		GyroRead();
		gyro_timer = GetTime_ms();
	}
	return gyro_vals[GAxisY];
}

float GyroReadZ(void)
{
	if (GetTimeSince_ms(gyro_timer) >= GYRO_DELAY)
	{
		GyroRead();
		gyro_timer = GetTime_ms();
	}
	return gyro_vals[GAxisZ];
}
