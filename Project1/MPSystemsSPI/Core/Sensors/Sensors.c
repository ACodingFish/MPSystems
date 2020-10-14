#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Sensors.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
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

float sensor_val[NUM_SENSORS] = {0};

typedef struct Sensor
{
	SensorType_t sensor;
	float (*read)(void);
	float val;
	char name[10];
} Sensor_t;

Sensor_t sensors[NUM_SENSORS] =
{
		{STTemperature,BSP_TSENSOR_ReadTemp, 0.0,"Temp"},
		{STHumidity,BSP_HSENSOR_ReadHumidity, 0.0, "Humidity"},
		{STAccelerometerX, BSP_TSENSOR_ReadTemp, 0.0,"Accel X"},
		{STAccelerometerY, BSP_TSENSOR_ReadTemp, 0.0, "Accel Y"},
		{STAccelerometerZ, BSP_TSENSOR_ReadTemp, 0.0, "Accel Z"},
};

void EnableSensor(SensorType_t sensor)
{
	SENSOR_SET_FLAG(SENSOR_MASK, sensor);
}

void DisableSensor(SensorType_t sensor)
{
	SENSOR_CLR_FLAG(SENSOR_MASK, sensor);
}

void ToggleSensor(SensorType_t sensor)
{
	SENSOR_TGL_FLAG(SENSOR_MASK, sensor);
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
			if (BSP_TSENSOR_Init() != TSENSOR_OK)
			{
			  //Temperature sensor failed to initialize...
			}
			break;
		case STHumidity:
			if (BSP_HSENSOR_Init() != HSENSOR_OK)
			{
			  //Humidity sensor failed to initialize...
			}
			break;
		case STAccelerometerX:
			break;
		case STAccelerometerY:
			break;
		case STAccelerometerZ:
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

		  msg_index += sprintf(msg+msg_index,"\r\n\0");
		  DebugLog(msg,sizeof(msg));
		  sensor_timer = GetTime_ms();
	  }
}


