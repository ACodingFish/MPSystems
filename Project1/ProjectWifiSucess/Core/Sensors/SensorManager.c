#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "SensorManager.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "MoistureSensor.h"
#include "Time.h"
#include "Common.h"
#include "DBG_UART.h"

bool SensorEnabled(SensorType_t sensor);


#define SENSOR_TIMER_DELAY MS(1000)

uint32_t SENSOR_MASK = 0x0;
uint32_t SENSOR_INIT_MASK = 0x0;

float sensor_val[STNumSensors] = {0};

typedef struct Sensor
{
	SensorType_t sensor;
	SensorType_t parent;
	float (*read)(void);
	float val;
	char name[16];
	uint32_t (*init)(void);
	uint32_t INIT_OK;
} Sensor_t;

Sensor_t sensors[STNumSensors] =
{
		{STTemperature, STNoParent,BSP_TSENSOR_ReadTemp, 0.0,"Temp", BSP_TSENSOR_Init, TSENSOR_OK},
		{STHumidity, STNoParent, BSP_HSENSOR_ReadHumidity, 0.0, "Humidity", BSP_HSENSOR_Init, HSENSOR_OK},
		//{STAccelerometerX, STNoParent, AccelReadX, 0.0,"Accel X", AccelInit, ACCEL_OK},
		//{STAccelerometerY, STAccelerometerX, AccelReadY, 0.0, "Accel Y", AccelInit, ACCEL_OK},
		//{STAccelerometerZ, STAccelerometerY, AccelReadZ, 0.0, "Accel Z", AccelInit, ACCEL_OK},
		//{STGyroscopeX, STNoParent, GyroReadX, 0.0,"Gyro X", GyroInit, GYR_OK},
		//{STGyroscopeY, STGyroscopeX, GyroReadY, 0.0, "Gyro Y", GyroInit, GYR_OK},
		//{STGyroscopeZ, STGyroscopeY, GyroReadZ, 0.0, "Gyro Z", GyroInit, GYR_OK},
		{STSoilMoisture, STNoParent, MoistureRead, 0.0, "Soil Mois. %", MoistureInit, MOISTURE_OK},
};

void EnableSensor(SensorType_t sensor)
{
	if (FLAG_CHK_FLAG(SENSOR_INIT_MASK, sensor) != false)
	{
		FLAG_SET_FLAG(SENSOR_MASK, sensor);
	}

}

void DisableSensor(SensorType_t sensor)
{
	if (FLAG_CHK_FLAG(SENSOR_INIT_MASK, sensor) != false)
	{
		FLAG_CLR_FLAG(SENSOR_MASK, sensor);
	}
}

void ToggleSensor(SensorType_t sensor)
{
	if (FLAG_CHK_FLAG(SENSOR_INIT_MASK, sensor) != false)
	{
		FLAG_TGL_FLAG(SENSOR_MASK, sensor);
	}
}

bool SensorEnabled(SensorType_t sensor)
{
	return FLAG_CHK_FLAG(SENSOR_MASK, sensor);
}

void SensorInit(SensorType_t sensor)
{
	switch(sensor)
	{
		case STTemperature:
		case STHumidity:
		//case STAccelerometerX:
		//case STGyroscopeX:
		case STSoilMoisture:
			if (sensors[(uint8_t) sensor].init() != sensors[(uint8_t) sensor].INIT_OK)
			{
				//sensor failed to initialize
			} else
			{
				FLAG_SET_FLAG(SENSOR_INIT_MASK, sensor);
			}
			break;
		/*case STAccelerometerY:
		case STAccelerometerZ:
		case STGyroscopeY:
		case STGyroscopeZ:
			if (FLAG_CHK_FLAG(SENSOR_INIT_MASK, sensors[(uint8_t)sensor].parent) == false)
			{
				if (sensors[(uint8_t) sensor].init() != sensors[(uint8_t) sensor].INIT_OK)
				{
					//sensor failed to initialize
				} else
				{
					FLAG_SET_FLAG(SENSOR_INIT_MASK, sensor);
				}
			} else
			{
				FLAG_SET_FLAG(SENSOR_INIT_MASK, sensor);
			}
			break;*/
		default:
			break;
	}

}

Tick sensor_timer = 0;
void SensorTask(void)
{
	  if (GetTimeSince_ms(sensor_timer) > SENSOR_TIMER_DELAY)
	  {
		  for (int i = 0; i < STNumSensors; i++)
		  {
			  sensors[i].val = sensors[i].read();
		  }

		 /* char msg[2048] = "";
		  uint16_t msg_index = 0;
		  for (int i = 0; i < STNumSensors; i++)
		  {
			  if (SensorEnabled((SensorType_t)i))
			  {
				  msg_index += sprintf(msg+msg_index,"%s: %f\r\n",sensors[i].name,sensors[i].val);
			  } else
			  {
				  //msg_index += sprintf(msg+msg_index,"%s: %s\r\n",sensors[i].name,"Disabled");
			  }

		  }

		  msg_index += sprintf(msg+msg_index,"\r\n%c",0); // end in a string to terminate debug logging
		  DBG_LOG((msg));
		  sensor_timer = GetTime_ms();*/
	  }
}


void SensorsGetData(float* sensor_data)
{
	for (int i = 0; i < STNumSensors; i++)
	{
		sensor_data[i] = sensors[i].val;
	}
}

float SensorGetData(SensorType_t sensor)
{
	if (sensor < STNumSensors)
	{
		return sensors[(int)sensor].val;
	} else
	{
		return 0.0;
	}
}
