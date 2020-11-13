#ifndef _WIFI_CONTROLLER_H_
#define _WIFI_CONTROLLER_H_

#define _WIFI_HTTP_TEST_
void WiFiControllerISR(void);
int WiFi_Controller_Init(void);

#ifdef _WIFI_HTTP_TEST_
void reset_alarm_state(void);
#else
#define WIFI_BUFF_SZ 1024
int WiFi_Controller_Task(void);
#endif

#endif //_WIFI_CONTROLLER_H_
