#ifndef _STM32_NETWORK_H_
#define _STM32_NETWORK_H_
typedef struct Network Network;
struct Network
{
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
};

void NetworkInit(Network* net);
int NetworkRead(Network* net, unsigned char* buff, int buff_sz, int timeout_ms);
int NetworkWrite(Network* net, unsigned char* buff, int buff_sz, int timeout_ms);
#endif //_STM32_NETWORK_H_
