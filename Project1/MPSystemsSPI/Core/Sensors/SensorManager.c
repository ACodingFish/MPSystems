#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "SensorManager.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "Accel.h"
#include "stm32l475e_iot01_gyro.h"
#include "Time.h"
#include "DebugUART.h"

#define SENSOR_SET_FLAG(mask, x) (mask |= (0x01<<x))
#define SENSOR_CLR_FLAG(mask, x) (mask &= ~(0x01<<x))
#define SENSOR_TGL_FLAG(mask, x) (mask ^= (0x01<<x))
#define SENSOR_CHK_FLAG(mask, x) ((mask & (0x01<<x))>0)

bool SensorEnabled(SensorType_t sensor);


#define SENSOR_TIMER_DELAY MS(1000)

#define NUM_SENSORS 5
uint32_t SENSOR_MASK = 0x0;
uint32_t SENSOR_INIT_MASK = 0x0;

float sensor_val[NUM_SENSORS] = {0};

typedef struct Sensor
{
	SensorType_t sensor;
	float (*read)(void);
	float val;
	char name[10];
	uint32_t (*init)(void);
	uint32_t INIT_OK;
} Sensor_t;

Sensor_t sensors[NUM_SENSORS] =
{
		{STTemperature,BSP_TSENSOR_ReadTemp, 0.0,"Temp", BSP_TSENSOR_Init, TSENSOR_OK},
		{STHumidity,BSP_HSENSOR_ReadHumidity, 0.0, "Humidity", BSP_HSENSOR_Init, HSENSOR_OK},
		{STAccelerometerX, AccelReadX, 0.0,"Accel X", AccelInit, ACCEL_OK},
		{STAccelerometerY, AccelReadY, 0.0, "Accel Y", AccelInit, ACCEL_OK},
		{STAccelerometerZ, AccelReadZ, 0.0, "Accel Z", AccelInit, ACCEL_OK},
};

void EnableSensor(SensorType_t sensor)
{
	if (SENSOR_CHK_FLAG(SENSOR_INIT_MASK, sensor) != false)
	{
		SENSOR_SET_FLAG(SENSOR_MASK, sensor);
	}

}

void DisableSensor(SensorType_t sensor)
{
	if (SENSOR_CHK_FLAG(SENSOR_INIT_MASK, sensor) != false)
	{
		SENSOR_CLR_FLAG(SENSOR_MASK, sensor);
	}
}

void ToggleSensor(SensorType_t sensor)
{
	if (SENSOR_CHK_FLAG(SENSOR_INIT_MASK, sensor) != false)
	{
		SENSOR_TGL_FLAG(SENSOR_MASK, sensor);
	}
}

bool SensorEnabled(SensorType_t sensor)
{
	return SENSOR_CHK_FLAG(SENSOR_MASK, sensor);
}

void SensorInit(SensorType_t sensor)
{
	switch(sensor)
	{
		case STTemperature:
		case STHumidity:
		case STAccelerometerX:
			if (sensors[(uint32_t) sensor].init() != sensors[(uint32_t) sensor].INIT_OK)
			{
				//sensor failed to initialize
			} else
			{
				SENSOR_SET_FLAG(SENSOR_INIT_MASK, sensor);
			}
			break;
		case STAccelerometerY:
			if (SENSOR_CHK_FLAG(SENSOR_INIT_MASK, STAccelerometerX) == false)
			{
				if (sensors[(uint32_t) sensor].init() != sensors[(uint32_t) sensor].INIT_OK)
				{
					//sensor failed to initialize
				} else
				{
					SENSOR_SET_FLAG(SENSOR_INIT_MASK, sensor);
				}
			} else
			{
				SENSOR_SET_FLAG(SENSOR_INIT_MASK, sensor);
			}
			break;
		case STAccelerometerZ:
			if ((SENSOR_CHK_FLAG(SENSOR_INIT_MASK, STAccelerometerX) == false) &&
					(SENSOR_CHK_FLAG(SENSOR_INIT_MASK, STAccelerometerY) == false))
			{
				if (sensors[(uint32_t) sensor].init() != sensors[(uint32_t) sensor].INIT_OK)
				{
					//sensor failed to initialize
				} else
				{
					SENSOR_SET_FLAG(SENSOR_INIT_MASK, sensor);
				}
			} else
			{
				SENSOR_SET_FLAG(SENSOR_INIT_MASK, sensor);
			}
			break;
		default:
			break;
	}

}

Tick sensor_timer = 0;
void SensorTask(void)
{
	  if (GetTimeSince_ms(sensor_timer) > SENSOR_TIMER_DELAY)
	  {
		  for (int i = 0; i < NUM_SENSORS; i++)
		  {
			  sensors[i].val = sensors[i].read();
		  }

		  char msg[2048] = "";
		  uint16_t msg_index = 0;
		  for (int i = 0; i < NUM_SENSORS; i++)
		  {
			  if (SensorEnabled((SensorType_t)i))
			  {
				  msg_index += sprintf(msg+msg_index,"%s: %f\r\n",sensors[i].name,sensors[i].val);
			  } else
			  {
				  msg_index += sprintf(msg+msg_index,"%s: %s\r\n",sensors[i].name,"Disabled");
			  }

		  }

		  msg_index += sprintf(msg+msg_index,"\r\n%c",0); // end in a string to terminate debug logging
		  DebugLog(msg,sizeof(msg));
		  sensor_timer = GetTime_ms();
	  }
}


