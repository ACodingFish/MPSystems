#include "stm32l4xx.h"
#include "DebugUART.h"

void DebugUARTSend(char * msg, uint16_t sz);
uint16_t DebugUARTGetBytes(char * msg, uint16_t sz);

UART_HandleTypeDef * uart;

void DebugUARTInit(UART_HandleTypeDef * huart)
{
	uart = huart;
}

void DebugUARTTask(void)
{

}

void DebugLog(char* msg, uint16_t max_sz)
{
	uint16_t sz = DebugUARTGetBytes(msg,max_sz);
	DebugUARTSend(msg,sz);
}

void DebugUARTSend(char * msg, uint16_t sz)
{
	HAL_UART_Transmit_DMA(uart, (uint8_t*)msg, sz);
}

uint16_t DebugUARTGetBytes(char * msg, uint16_t sz)
{
	uint16_t byte_count = 0;
	for (int i = 0; i < sz; i++)
	{
		if (msg[i] != '\0')
		{
			byte_count++;
		} else
		{
			break;
		}
	}
	return (byte_count*sizeof(char));
}

