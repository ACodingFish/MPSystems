#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "Pedometer.h"
#include "SensorManager.h"
#include "Time.h"
#include "DebugUART.h"


#define PEDOMETER_READ_RATE ACCEL_READ_RATE
Tick ped_timer = 0;

#define PEDOMETER_BUFFER_SZ 10
float ped_buff[PEDOMETER_BUFFER_SZ];


#define DEFAULT_PEDOMETER_AVG_VAL 1000.0f // roughly 1 G, which is about gravity

#define PERCENT_CHANGE_STEP_MAX 2.0f			// 200 percent
#define PERCENT_CHANGE_STEP_MIN 0.1f

#define MIN_TIME_BETWEN_STEPS MS(400) // hopefully good enough
Tick step_timer = 0;

int16_t step_counter = 0;

float PedometerReadAccelAvg(void);
float PedometerGetBuffAvg(void);

void PedometerInit(void)
{
	  SensorInit(STAccelerometerX);
	  SensorInit(STAccelerometerY);
	  SensorInit(STAccelerometerZ);
	  EnableSensor(STAccelerometerX);
	  EnableSensor(STAccelerometerY);
	  EnableSensor(STAccelerometerZ);

	  for (int i = 0; i < PEDOMETER_BUFFER_SZ; i++)
	  {
		  ped_buff[i] = DEFAULT_PEDOMETER_AVG_VAL;
	  }
}

float PedometerReadAccelAvg(void) // Measured in milli G's
{
	float x = SensorGetData(STAccelerometerX);
	float y = SensorGetData(STAccelerometerY);
	float z = SensorGetData(STAccelerometerZ);

	return (float)sqrt(x*x + y*y + z*z);
}

float PedometerGetBuffAvg(void)
{
	double sum = 0.0;
	for(int i = 0; i < PEDOMETER_BUFFER_SZ; i++)
	{
		sum += ped_buff[i];
	}
	sum /= PEDOMETER_BUFFER_SZ;
	return (float)sum;
}

char step_msg[100] = "";
uint16_t step_msg_index = 0;
void PedometerTask(void)
{
	if (GetTimeSince_ms(ped_timer)>PEDOMETER_READ_RATE)
	{
		float accel_avg = PedometerReadAccelAvg();
		float buff_avg = PedometerGetBuffAvg();
		float differential = abs((int32_t)(accel_avg - buff_avg));

		if (differential>(PERCENT_CHANGE_STEP_MAX*buff_avg))
		{
			// not a step, too large of a change
		} else if (differential>(PERCENT_CHANGE_STEP_MIN*buff_avg))
		{
			// step
			if (GetTimeSince_ms(step_timer)>MIN_TIME_BETWEN_STEPS)
			{

				step_msg_index = 0;
				step_counter++;
				step_msg_index += sprintf(step_msg+step_msg_index,"Steps: %d\r\n%c",step_counter,0);
				DebugLog(step_msg,sizeof(step_msg));
				step_timer = GetTime_ms();
			}
		} else
		{
			// not a step, too small of a change
		}

		for (int i = 0; i < PEDOMETER_BUFFER_SZ-1; i++)
		{
			ped_buff[i] = ped_buff[i+1];
		}
		ped_buff[0] = accel_avg;

		ped_timer = GetTime_ms();
	}

}
