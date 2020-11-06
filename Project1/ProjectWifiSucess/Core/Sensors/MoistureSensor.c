#include <stdint.h>
#include "ADC.h"
#include "MoistureSensor.h"

#define MIN_MOISTURE_VALUE 0 // Calibrate for dry soil
#define MAX_MOISTURE_VALUE 350 // Calibrate for wet (saturated) soil

uint32_t MoistureInit(void)
{
	return (uint32_t)MOISTURE_OK;
}

float MoistureRead(void)
{
	float moisture_reading = (float)ADC_Read(ADC_MOISTURE);
	if (moisture_reading > MAX_MOISTURE_VALUE)
	{
		moisture_reading = MAX_MOISTURE_VALUE;
	} else if (moisture_reading < MIN_MOISTURE_VALUE)
	{
		moisture_reading = MIN_MOISTURE_VALUE;
	}
	return ((moisture_reading - MIN_MOISTURE_VALUE)*100)/(MAX_MOISTURE_VALUE - MIN_MOISTURE_VALUE); // converted from 12 bit to percentage
}

