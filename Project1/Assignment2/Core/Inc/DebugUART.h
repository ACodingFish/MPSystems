#ifndef _DEBUG_UART_H_
#define _DEBUG_UART_H_
// Must enable uart IRQ (EX. USART1_IRQn) and DMA
void DebugUARTInit(UART_HandleTypeDef * uart_handle);
void DebugUARTTask(void);
void DebugLog(char* msg, uint16_t max_sz);
#endif //_DEBUG_UART_H_
