#ifndef _SENSOR_MANAGER_H_
#define _SENSOR_MANAGER_H_

typedef enum SensorType
{
	STTemperature = 0x0,
	STHumidity,
	STAccelerometerX,
	STAccelerometerY,
	STAccelerometerZ,
	STGyroscopeX,
	STGyroscopeY,
	STGyroscopeZ,
	STNumSensors,
	STNoParent,
} SensorType_t;

#define ACCEL_READ_RATE MS(50)

void SensorInit(SensorType_t sensor);
void SensorTask(void);
void EnableSensor(SensorType_t sensor);
void DisableSensor(SensorType_t sensor);
void ToggleSensor(SensorType_t sensor);
float SensorGetData(SensorType_t sensor);

#endif //_SENSOR_MANAGER_H_
