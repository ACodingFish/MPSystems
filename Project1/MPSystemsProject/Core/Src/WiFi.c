#include <stdint.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#include "Time.h"
#include "WiFi.h"

void WiFi_Connect(); // if callback gives "\r\n>"

SPI_HandleTypeDef* SPI_WiFi;
uint8_t wifi_connection_step = 0;

void WiFi_Init(SPI_HandleTypeDef* sp_handle)
{
	SPI_WiFi = sp_handle;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
}

#define WIFI_CMD_SZ 1024
char WIFI_CMD_BUFF[WIFI_CMD_SZ];
void WiFi_Connect() // if callback gives "\r\n>"
{
	if (wifi_connection_step < 6)
	{
		wifi_connection_step++;
	}
	switch(wifi_connection_step)
	{
		case 1:// SSID
			strcpy(WIFI_CMD_BUFF,"AS=0,ABC\r\x0A\0");
			break;
		case 2:// AP
			strcpy(WIFI_CMD_BUFF,"AD\r\x0A\0");
			break;
		case 3:// TCP Protocol
			strcpy(WIFI_CMD_BUFF,"P1=0\r\x0A\0");
			break;
		case 4:// TCP Port
			strcpy(WIFI_CMD_BUFF,"P4=2000\r\0");
			break;
		case 5:// Start TCP COMM Server
			strcpy(WIFI_CMD_BUFF,"P5=1\r\x0A\0");
			break;
		case 6:// Read Messages
			strcpy(WIFI_CMD_BUFF,"MR\r\x0A\0");
			break;
		default:
			break;
	}

	WiFi_Send_Command((uint8_t*)WIFI_CMD_BUFF, sizeof(WIFI_CMD_BUFF)/sizeof(char));

}

void WiFi_Send_Command(uint8_t* command, uint32_t cmd_sz)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
	//HAL_StatusTypeDef status =
	HAL_SPI_Transmit(SPI_WiFi, command, cmd_sz, 1000);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
	//return (status == HAL_OK);

}

#define WIFI_RECV_SZ 1024
char WIFI_RECV_BUFF[WIFI_RECV_SZ];
void WiFi_Recieve()
{
	HAL_SPI_Receive(SPI_WiFi, (uint8_t*)WIFI_RECV_BUFF, WIFI_RECV_SZ, 1000);
	if (strcmp(WIFI_RECV_BUFF,"\r\n> \0") != 0) // reset/first power on of wifi module
	{
		wifi_connection_step = 0;
		WiFi_Connect();
	}

}
