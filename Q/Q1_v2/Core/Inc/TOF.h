#ifndef _TOF_H_
#define _TOF_H_
void TOF_Init(VL53L0X_Dev_t dev);
uint16_t TOF_GetReading(void);
#endif //_TOF_H_
