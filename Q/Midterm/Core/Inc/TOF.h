#ifndef _TOF_H_
#define _TOF_H_

typedef enum
{
  PROX_SENSOR_OK = 0,
  PROX_SENSOR_ERROR
}PROX_SENSOR_Status_TypDef;

#define PROX_MAX_VAL 8190

uint32_t TOF_Init(void);
uint16_t TOF_GetReading(void);
float TOF_SensorManagerRead(void);
#endif //_TOF_H_
