#ifndef _DBG_UART_H_
#define _DBG_UART_H_
#include <stdio.h>
void DBG_UART_Init(void);
#define DBG_LOG(a) printf a
#endif //_DBG_UART_H_
