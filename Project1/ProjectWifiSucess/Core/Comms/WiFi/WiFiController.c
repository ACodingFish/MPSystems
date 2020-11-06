#include <stdbool.h>
#include "DBG_UART.h"
#include "WiFiSecrets.h"
#include "es_wifi.h"
#include "wifi.h"
#include "WiFiController.h"

#define PORT		80
#define SOCKET		0
static  uint8_t http[1024];
static  uint8_t  IP_Addr[4];

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000

static bool WebServerProcess(void);
static  WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temperature);

int WiFi_Controller_Init(void)
{
	bool StopServer = false;

	DBG_LOG(("\nRunning HTML Server test\n"));
	uint8_t  MAC_Addr[6];

	/*Initialize and use WIFI module */
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

static bool WebServerProcess(void)
{
  uint8_t temp;
  uint16_t  respLen;
  static   uint8_t resp[1024];
  bool    stopserver=false;

  if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, resp, 1000, &respLen, WIFI_READ_TIMEOUT))
  {
   DBG_LOG(("get %d byte from server\n",respLen));

   if( respLen > 0)
   {
      if(strstr((char *)resp, "GET")) /* GET: put web page */
      {
        temp = 0;
        if(SendWebPage(0, temp) != WIFI_STATUS_OK)
        {
          DBG_LOG(("> ERROR : Cannot send web page\n"));
        }
        else
        {
          DBG_LOG(("Send page after  GET command\n"));
        }
       }
       else if(strstr((char *)resp, "POST"))/* POST: received info */
       {
         DBG_LOG(("Post request\n"));

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
         temp = 0;
         if(SendWebPage(0, temp) != WIFI_STATUS_OK)
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

static WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temperature)
{
  uint8_t  temp[50];
  uint16_t SentDataLength;
  WIFI_Status_t ret;

  /* construct web page content */
  strcpy((char *)http, (char *)"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");
  strcat((char *)http, (char *)"<html>\r\n<body>\r\n");
  strcat((char *)http, (char *)"<title>STM32 Web Server</title>\r\n");
  strcat((char *)http, (char *)"<h2>InventekSys : Web Server using Es-Wifi with STM32</h2>\r\n");
  strcat((char *)http, (char *)"<br /><hr>\r\n");
  strcat((char *)http, (char *)"<p><form method=\"POST\"><strong>Temp: <input type=\"text\" value=\"");
  sprintf((char *)temp, "%d", temperature);
  strcat((char *)http, (char *)temp);
  strcat((char *)http, (char *)"\"> <sup>O</sup>C");

  if (ledIsOn)
  {
    strcat((char *)http, (char *)"<p><input type=\"radio\" name=\"radio\" value=\"0\" >LED off");
    strcat((char *)http, (char *)"<br><input type=\"radio\" name=\"radio\" value=\"1\" checked>LED on");
  }
  else
  {
    strcat((char *)http, (char *)"<p><input type=\"radio\" name=\"radio\" value=\"0\" checked>LED off");
    strcat((char *)http, (char *)"<br><input type=\"radio\" name=\"radio\" value=\"1\" >LED on");
  }

  strcat((char *)http, (char *)"</strong><p><input type=\"submit\"></form></span>");
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

