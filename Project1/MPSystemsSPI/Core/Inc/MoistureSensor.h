#ifndef _MOISTURE_SENSOR_H_
#define _MOISTURE_SENSOR_H_

typedef enum
{
  MOISTURE_OK = 0,
  MOISTURE_ERROR = 1,
  MOISTURE_TIMEOUT = 2
} MOISTURE_StatusTypeDef;

uint32_t MoistureInit(void);
float MoistureRead(void);

#endif //_MOISTURE_SENSOR_H_
