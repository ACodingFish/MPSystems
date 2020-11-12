#include "stm32l4xx.h"
#include <stdint.h>
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_def.h"
#include "stm32l475e_iot01.h"
#include "TOF.h"

extern I2C_HandleTypeDef hi2c2;

#define TOF_ADDR         ((uint16_t)0x0052)
#define TOF_ID           ((uint16_t)0xEEAA)
#define TOF_XSHUT_PIN 	GPIO_PIN_6
#define TOF_XSHUT_PORT GPIOC

VL53L0X_Dev_t tof_device = // Initialize tof structure with I2C Handle and Device Location
{
	.I2cHandle = &hi2c2,
	.I2cDevAddr = TOF_ADDR
};


uint32_t TOF_Init(void)
{
	HAL_GPIO_WritePin(TOF_XSHUT_PORT, TOF_XSHUT_PIN, GPIO_PIN_SET); // Set shutdown pin high (active low)

	HAL_Delay(1000); // delay 1s to allow device to start up

	uint16_t dev_init_fail = VL53L0X_DataInit(&tof_device); // initialize device
	if (VL53L0X_ERROR_NONE == dev_init_fail)
	{
		tof_device.Present = 1;			// set device as present
		SetupSingleShot(tof_device); 	// set up device for single ranging mode and calibrates device
		return (uint32_t) PROX_SENSOR_OK;
	}
	else
	{
		//Data Init Failed
		return (uint32_t) PROX_SENSOR_ERROR;
	}

}

uint16_t TOF_GetReading(void)
{
	uint16_t distance = 0xFFFF; // max distance if error
	if (tof_device.Present == 1) // if the device is present
	{
		VL53L0X_RangingMeasurementData_t tof_data;

		VL53L0X_PerformSingleRangingMeasurement(&tof_device, &tof_data); // get a single datapoint

		distance = tof_data.RangeMilliMeter;

	}
	return distance;
}

float TOF_SensorManagerRead(void)
{
	return (float)TOF_GetReading();
}
