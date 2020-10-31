#include "stm32l4xx.h"
#include <stdint.h>
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_def.h"
#include "stm32l475e_iot01.h"
#include "TOF.h"

VL53L0X_Dev_t tof_device;

#define TOF_ADDR         ((uint16_t)0x0052)
#define TOF_ID           ((uint16_t)0xEEAA)
#define TOF_XSHUT_PIN 	GPIO_PIN_6
#define TOF_XSHUT_PORT GPIOC


void TOF_Init(VL53L0X_Dev_t dev)
{
	tof_device = dev;
	HAL_GPIO_WritePin(TOF_XSHUT_PORT, TOF_XSHUT_PIN, GPIO_PIN_SET); // Set shutdown pin high (active low)

	HAL_Delay(1000); // delay 1s to allow device to start up

	uint16_t dev_init_fail = VL53L0X_DataInit(&tof_device); // initialize device
	if (VL53L0X_ERROR_NONE == dev_init_fail)
	{
		tof_device.Present = 1;			// set device as present
		SetupSingleShot(tof_device); 	// set up device for single ranging mode and calibrates device
	}
	else
	{
		//Data Init Failed
	}

}

uint16_t TOF_GetReading(void)
{
	uint16_t distance = 0xFFFF;
	if (tof_device.Present == 1)
	{
		VL53L0X_RangingMeasurementData_t tof_data;

		VL53L0X_PerformSingleRangingMeasurement(&tof_device, &tof_data);

		distance = tof_data.RangeMilliMeter;

	}
	return distance;
}
