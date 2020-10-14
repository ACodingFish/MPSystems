#ifndef _GYRO_H_
#define _GYRO_H_

typedef enum GyroAxis
{
	GAxisX,
	GAxisY,
	GAxisZ,
}GyroAxis_t;

typedef enum
{
  GYR_OK = 0,
  GYR_ERROR = 1,
  GYR_TIMEOUT = 2
} GYR_StatusTypeDef;

uint32_t GyroInit(void);
float GyroReadX(void);
float GyroReadY(void);
float GyroReadZ(void);

#endif //_GYRO_H_
