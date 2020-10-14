#ifndef _SENSOR_MANAGER_H_
#define _SENSOR_MANAGER_H_

typedef enum SensorType
{
	STTemperature,
	STHumidity,
	STAccelerometerX,
	STAccelerometerY,
	STAccelerometerZ,
} SensorType_t;

void SensorInit(SensorType_t sensor);
void SensorTask(void);
void EnableSensor(SensorType_t sensor);
void DisableSensor(SensorType_t sensor);
void ToggleSensor(SensorType_t sensor);

#endif //_SENSOR_MANAGER_H_
