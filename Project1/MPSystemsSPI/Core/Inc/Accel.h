#ifndef _ACCEL_H_
#define _ACCEL_H_

typedef enum AccelAxis
{
	AAxisX,
	AAxisY,
	AAxisZ,
}AccelAxis_t;

typedef enum
{
  ACCEL_OK = 0,
  ACCEL_ERROR = 1,
  ACCEL_TIMEOUT = 2
} ACCEL_StatusTypeDef;

uint32_t AccelInit(void);
float AccelReadX(void);
float AccelReadY(void);
float AccelReadZ(void);

#endif //_ACCEL_H_
