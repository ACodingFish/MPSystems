#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l475e_iot01.h"
#include "DBG_UART.h"
#include "es_wifi.h"
#include "wifi.h"
#include "WiFiController.h"
#include "WiFiSecrets.h"
#include "TOF.h"
#include "SensorManager.h"
#include "Time.h"


#ifdef _WIFI_HTTP_TEST_
#define PORT		80
#define SOCKET		0
static  uint8_t http[1024];
static  uint8_t  IP_Addr[4];

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

/* http server stuff */
int alarm_state = 0;
uint16_t proximity_alarm_threshold = 1200;
/* end http server stuff */

static bool WebServerProcess(void);
static WIFI_Status_t SendWebPage(uint8_t alarmOn, uint8_t temperature, uint16_t pros_distance, uint16_t prox_threshold);
void alarm_state_task(void);

int WiFi_Controller_Init(void)
{
	bool StopServer = false;

	DBG_LOG(("\nRunning HTML Server test\n"));
	uint8_t  MAC_Addr[6];

	//Initialize and use WIFI module
	WIFI_Status_t w_stat = WIFI_Init();
	if(w_stat ==  WIFI_STATUS_OK)
	{
	  DBG_LOG(("ES-WIFI Initialized.\n"));
	}
	else
	{
		DBG_LOG(("WIFI_STATUS NOT OK\n"));
		return -1;
	}

	w_stat = WIFI_GetMAC_Address(MAC_Addr);
	if(w_stat == WIFI_STATUS_OK)
	{
		DBG_LOG(("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",MAC_Addr[0],MAC_Addr[1],MAC_Addr[2],MAC_Addr[3],MAC_Addr[4],MAC_Addr[5]));
	}
	else
	{
		DBG_LOG(("> ERROR : CANNOT get MAC address\n"));
		return -1;
	}

	DBG_LOG(("\nConnecting to %s , %s\n",SSID,PASSWORD));
	w_stat = WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK);

	if(w_stat == WIFI_STATUS_OK)
	{

	} else
	{
	  DBG_LOG(("ERROR : es-wifi module NOT connected\n"));
	  return -1;
	}

	w_stat = WIFI_GetIP_Address(IP_Addr);

	if(w_stat == WIFI_STATUS_OK)
	{
	  DBG_LOG(("> es-wifi module connected: got IP Address : %d.%d.%d.%d\n",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));
	}
	else
	{
	  DBG_LOG((" ERROR : es-wifi module CANNOT get IP address\n"));
	  return -1;
	}


	if (WIFI_STATUS_OK!=WIFI_StartServer(SOCKET, WIFI_TCP_PROTOCOL, 1, "", PORT))
	{
	  DBG_LOG(("ERROR: Cannot start server.\n"));
	  return -1;
	}

	DBG_LOG(("Server is running and waiting for an HTTP  Client connection to %d.%d.%d.%d\n",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));

	do
	{
	uint8_t RemoteIP[4];
	uint16_t RemotePort;


	while (WIFI_STATUS_OK != WIFI_WaitServerConnection(SOCKET,1000,RemoteIP,&RemotePort))
	{
		DBG_LOG(("Waiting connection to  %d.%d.%d.%d\n",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));
		SensorTask();
		float prox_distance = SensorGetData(STProximity);
		if (prox_distance < proximity_alarm_threshold) // if the object is close
		{
		alarm_state = 1;
		} else
		{
		//alarm_state = 0;
		//alarm state can only be reset by pushbutton
		}
		alarm_state_task();

	}

	DBG_LOG(("Client connected %d.%d.%d.%d:%d\n",RemoteIP[0],RemoteIP[1],RemoteIP[2],RemoteIP[3],RemotePort));

	StopServer=WebServerProcess();

	if(WIFI_CloseServerConnection(SOCKET) != WIFI_STATUS_OK)
	{
		DBG_LOG(("ERROR: failed to close current Server connection\n"));
	  return -1;
	}
	}
	while(StopServer == false);

	if (WIFI_STATUS_OK!=WIFI_StopServer(SOCKET))
	{
	  DBG_LOG(("ERROR: Cannot stop server.\n"));
	}

	DBG_LOG(("Server is stop\n"));
	return 0;
}

void reset_alarm_state(void)
{
	alarm_state = 0;
	BSP_LED_Off(LED2);
}

Tick alarm_LED_timer = 0;
#define ALARM_LED_PERIOD MS(500)
void alarm_state_task(void)
{
	if ((alarm_state > 0)&&(GetTimeSince_ms(alarm_LED_timer) > ALARM_LED_PERIOD))
	{
		BSP_LED_Toggle(LED2);
		alarm_LED_timer = GetTime_ms();
	}
}

static bool WebServerProcess(void)
{
  uint8_t temp;
  uint16_t prox_distance;
  uint16_t  respLen;
  uint8_t resp[1024]= "";
  bool    stopserver=false;

  SensorTask();
  prox_distance = SensorGetData(STProximity);
  if (prox_distance < proximity_alarm_threshold) // if the object is close
  {
  	alarm_state = 1;
  } else
  {
  	//alarm_state = 0;
	//alarm state can only be reset by pushbutton
  }

  alarm_state_task();

  if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, resp, 1000, &respLen, WIFI_READ_TIMEOUT))
  {
   DBG_LOG(("get %d byte from server\n",respLen));

   if( respLen > 0)
   {
      if(strstr((char *)resp, "GET")) // GET: put web page
      {
        temp = SensorGetData(STTemperature);
        if(SendWebPage(alarm_state, temp, prox_distance, proximity_alarm_threshold) != WIFI_STATUS_OK)
        {
          DBG_LOG(("> ERROR : Cannot send web page\n"));
        }
        else
        {
          DBG_LOG(("Send page after  GET command\n"));
        }
       }
       else if(strstr((char *)resp, "POST"))// POST: received info
       {
         DBG_LOG(("Post request\n"));

         if(strstr((char *)resp, "pThresh"))
         {
        	char* loc = strstr((char *)resp, "pThresh");
        	char* start = (strstr(loc, "=") + 1); // move one ahead of equals sign.
        	char* end;
        	int32_t val = strtod(start, &end);
        	if ((val <= PROX_MAX_VAL)&& (val >= 0))
        	{
            	proximity_alarm_threshold = val;
            	DBG_LOG(("New PThresh: %d\n",(int)val));
        	}
         }
         if(strstr((char *)resp, "stop_server"))
         {
           if(strstr((char *)resp, "stop_server=0"))
           {
             stopserver = false;
           }
           else if(strstr((char *)resp, "stop_server=1"))
           {
             stopserver = true;
           }
         }
         temp = SensorGetData(STTemperature);
         if(SendWebPage(alarm_state, temp, prox_distance, proximity_alarm_threshold) != WIFI_STATUS_OK)
         {
           DBG_LOG(("> ERROR : Cannot send web page\n"));
         }
         else
         {
           DBG_LOG(("Send Page after POST command\n"));
         }
       }
     }
  }
  else
  {
    DBG_LOG(("Client close connection\n"));
  }
  return stopserver;

 }

static WIFI_Status_t SendWebPage(uint8_t alarmOn, uint8_t temperature, uint16_t prox_distance, uint16_t prox_threshold)
{
	uint8_t  temp[4];
	uint8_t prox_dist[8];
	uint8_t prox_thresh[8];
	//uint8_t prox_max[8];
	uint16_t SentDataLength;
	WIFI_Status_t ret;

	// construct web page content
	strcpy((char *)http, (char *)"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
	strcat((char *)http, (char *)"<html>\r\n<body text=\"#F4F32B\" bgcolor=\"#8808D4\">\r\n");
	strcat((char *)http, (char *)"<title>STM32 Web Server</title>\r\n");
	strcat((char *)http, (char *)"<h2><center>STM32: Sensor Dashboard</center></h2>\r\n");
	strcat((char *)http, (char *)"<hr>\r\n");
	strcat((char *)http, (char *)"<h3><center>Device</center></h3><hr><p><form method=\"POST\"><strong>");
	//strcat((char *)http, (char *)"Temperature (<sup>o</sup>C): <input type=\"text\" value=\"");
	strcat((char *)http, (char *)"<p>Temperature: ");
	sprintf((char *)temp, "%d", temperature);
	strcat((char *)http, (char *)temp);
	strcat((char *)http, (char *)" <sup>o</sup>C");
	//strcat((char *)http, (char *)"\">");
	strcat((char *)http, (char *) "<p>Prox Distance:\t\t");
	//strcat((char *)http, (char *)"<p>Prox Distance (mm): <input type=\"text\" value=\"");
	sprintf((char *)prox_dist, "%d", prox_distance);
	strcat((char *)http, (char *)prox_dist);
	strcat((char *)http, (char *) " mm");
	//strcat((char *)http, (char *)"\">");

	if (alarmOn)
	{
	  strcat((char*)http, (char *) "<p>Alarm State:\t\tON");
	//strcat((char *)http, (char *)"<p><input type=\"radio\" name=\"radio\" value=\"0\" >Alarm off");
	//strcat((char *)http, (char *)"<br><input type=\"radio\" name=\"radio\" value=\"1\" checked>Alarm on");
	}
	else
	{
	  strcat((char*)http, (char *) "<p>Alarm State:\t\tOFF");
	//strcat((char *)http, (char *)"<p><input type=\"radio\" name=\"radio\" value=\"0\" checked>Alarm off");
	//strcat((char *)http, (char *)"<br><input type=\"radio\" name=\"radio\" value=\"1\" >Alarm on");
	}

	strcat((char *)http, (char *)"<hr><h3><center>Settings</center></h3><hr><p>Prox Threshold (mm):\t<input type=\"text\" name=\"pThresh\" value=\"");
	sprintf((char *)prox_thresh, "%d", prox_threshold);
	strcat((char *)http, (char *)prox_thresh);
	strcat((char *)http, (char *)"\">");
	strcat((char *)http, (char *)" (Current: ");
	strcat((char *)http, (char *)prox_thresh);
	strcat((char *)http, (char *)")");
	//strcat((char *)http, (char *)"<br><p>Prox Threshold (mm): <input type=\"range\" min=\"0\" max=\"");
	//strcat((char *)http, (char *)prox_max);
	//strcat((char *)http, (char *)"\" value=\"");
	//sprintf((char *)prox_thresh, "%d", prox_threshold);
	//strcat((char *)http, (char *)prox_thresh);
    //strcat((char *)http, (char *)"\" class=\"slider\" id=\"range\">");

  //strcat((char *)http, (char *)"<p><form method=\"POST\"><strong>Prox Threshold: <input type=\"text\" value=\"");
  //sprintf((char *)prox_thresh, "%d", prox_threshold);
  //strcat((char *)http, (char *)prox_thresh);
  //strcat((char *)http, (char *)"\"> mm");
  strcat((char *)http, (char *)"</strong><p><center><input type=\"submit\"></center></form></span>");
  strcat((char *)http, (char *)"</body>\r\n</html>\r\n");

  ret = WIFI_SendData(0, (uint8_t *)http, strlen((char *)http), &SentDataLength, WIFI_WRITE_TIMEOUT);

  if((ret == WIFI_STATUS_OK) && (SentDataLength != strlen((char *)http)))
  {
    ret = WIFI_STATUS_ERROR;
  }

  return ret;
}

void WiFiControllerISR(void)
{
	SPI_WIFI_ISR();
}
#else
#include <stdlib.h>
#include "SensorManager.h"
#include "Time.h"
bool wifi_connected = false;
#define PORT		10000
#define SOCKET		0
static  uint8_t  IP_Addr[4];
#define WIFI_SEND_TIMEOUT 5000
#define WIFI_RECV_TIMEOUT 0
uint16_t wifi_buff_sz = WIFI_BUFF_SZ;
uint16_t wifi_data_transferred = 0;
uint8_t wifi_buff[WIFI_BUFF_SZ];
uint8_t RemoteIP[4];
uint16_t RemotePort;

void WiFi_Controller_Cmd(uint8_t * cmd, uint16_t cmd_sz);

int WiFi_Controller_Init(void)
{

	DBG_LOG(("\nRunning TCP Socket Server test\n"));
	uint8_t  MAC_Addr[6];

	//Initialize and use WIFI module
	WIFI_Status_t w_stat = WIFI_Init();
	if(w_stat ==  WIFI_STATUS_OK)
	{
	  DBG_LOG(("ES-WIFI Initialized.\n"));
	}
	else
	{
		DBG_LOG(("WIFI_STATUS NOT OK\n"));
		return -1;
	}

	w_stat = WIFI_GetMAC_Address(MAC_Addr);
	if(w_stat == WIFI_STATUS_OK)
	{
		DBG_LOG(("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n",MAC_Addr[0],MAC_Addr[1],MAC_Addr[2],MAC_Addr[3],MAC_Addr[4],MAC_Addr[5]));
	}
	else
	{
		DBG_LOG(("> ERROR : CANNOT get MAC address\n"));
		return -1;
	}

	DBG_LOG(("\nConnecting to %s , %s\n",SSID,PASSWORD));
	w_stat = WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK);

	if(w_stat == WIFI_STATUS_OK)
	{

	} else
	{
	  DBG_LOG(("ERROR : es-wifi module NOT connected\n"));
	  return -1;
	}

	w_stat = WIFI_GetIP_Address(IP_Addr);

	if(w_stat == WIFI_STATUS_OK)
	{
	  DBG_LOG(("> es-wifi module connected: got IP Address : %d.%d.%d.%d\n",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));
	}
	else
	{
	  DBG_LOG((" ERROR : es-wifi module CANNOT get IP address\n"));
	  return -1;
	}


	if (WIFI_STATUS_OK!=WIFI_StartServer(SOCKET, WIFI_TCP_PROTOCOL, 1, "", PORT))
	{
	  DBG_LOG(("ERROR: Cannot start server.\n"));
	  return -1;
	}

	DBG_LOG(("Server is running and waiting for an TCP Client connection to %d.%d.%d.%d\n",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));

	HAL_Delay(50);
	return 0;
}

#define PING_TEST_TIME MS(15000)
#define PING_FAILURE_MAX 3
uint8_t ping_failure_count = 0;
Tick ping_timer = 0;
int WiFi_Controller_Task(void)
{

	if (wifi_connected == false)
	{
		if (WIFI_STATUS_OK != WIFI_WaitServerConnection(SOCKET,1000,RemoteIP,&RemotePort))
		{
			DBG_LOG(("Waiting connection to  %d.%d.%d.%d\n",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));

		} else
		{
			wifi_connected = true;
			ping_failure_count = 0;
			ping_timer = GetTime_ms();
			DBG_LOG(("Client connected %d.%d.%d.%d:%d\n",RemoteIP[0],RemoteIP[1],RemoteIP[2],RemoteIP[3],RemotePort));
		}

	} else
	{
		if ((WIFI_IS_CMDDATA_READY()!=false) && (WIFI_IsConnected()==WIFI_STATUS_OK))
		{
			if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, wifi_buff, wifi_buff_sz, &wifi_data_transferred, WIFI_RECV_TIMEOUT))
			{
				if (wifi_data_transferred > 0)
				{
					//ping_failure_count = 0;
					//ping_timer = GetTime_ms();
					// add command interpreting
					WiFi_Controller_Cmd(wifi_buff, wifi_data_transferred);
				}
			} else
			{
				wifi_connected = false;
			}
		}

		if ((GetTimeSince_ms(ping_timer)>PING_TEST_TIME) && (ping_failure_count < PING_FAILURE_MAX))
		{
			// ping
			/*uint16_t send_sz = sprintf((char*)wifi_buff,".");
			if ((WIFI_IsConnected()==WIFI_STATUS_OK)&&(WIFI_SendDataTo(SOCKET, wifi_buff, send_sz, &wifi_data_transferred, WIFI_SEND_TIMEOUT, RemoteIP, RemotePort)==WIFI_STATUS_OK))
			{
				ping_failure_count = 0;
			} else
			{
				ping_failure_count++;
			}
			ping_timer = GetTime_ms();*/
		}

		if (ping_failure_count >= PING_FAILURE_MAX)
		{
			wifi_connected = false;
		}

		if (wifi_connected == false)
		{
			if(WIFI_CloseServerConnection(SOCKET) != WIFI_STATUS_OK)
			{
				DBG_LOG(("ERROR: failed to close current Server connection\n"));
			  return -1;
			}

			if (WIFI_STATUS_OK!=WIFI_StopServer(SOCKET))
			{
			  DBG_LOG(("ERROR: Cannot stop server.\n"));
			}

			DBG_LOG(("Server is stop\n"));
		}
	}
	return 0;
}

typedef enum WiFi_Command
{
	WFC_GetSensorData,
}WiFi_Command_t;

void WiFi_Controller_Cmd(uint8_t * cmd, uint16_t cmd_sz)
{
	uint16_t sync_sz = WIFI_CMD_SYNC_SZ;
	if (cmd_sz > sync_sz-1) // if there is a cmd
	{
		for (int i = 0; i < sync_sz-1; i++) // don't check for null byte
		{
			if (cmd[i] != (uint8_t)WIFI_CMD_SYNC[i])
			{
				return;
			}
		}

		// if the cmd is valid
		int command = atoi((char*)cmd+WIFI_CMD_SYNC_SZ-1);
		uint16_t send_sz = 0;
		float sensors[STNumSensors];
		switch((WiFi_Command_t)command)
		{
			case WFC_GetSensorData:
				// send sensor data
				SensorsGetData(sensors);
				//send_sz+= sprintf((char*)wifi_buff, "-- Sending Sensor Data --\n");
				send_sz+= sprintf((char*)wifi_buff, "SEN:");
				for (int i = 0; i < STNumSensors; i++)
				{
					send_sz+= sprintf((char*)wifi_buff + send_sz, "%d,%f-",i,sensors[i]);
				}
				break;
			default:
				send_sz+= sprintf((char*)wifi_buff, "Invalid Command: %d\n", command);
				break;

		}

		if (send_sz > 0)
		{
			WIFI_SendDataTo(SOCKET, wifi_buff, send_sz, &wifi_data_transferred, WIFI_SEND_TIMEOUT, RemoteIP, RemotePort);
		}
	}
}

void WiFiControllerISR(void)
{
	SPI_WIFI_ISR();
}
#endif


