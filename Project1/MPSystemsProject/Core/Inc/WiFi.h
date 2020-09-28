#ifndef _WIFI_H_
#define _WIFI_H_

void WiFi_Init();
void WiFi_Send_Command(uint8_t* command, uint32_t cmd_sz);
void WiFi_Recieve();

#endif //_WIFI_H_
