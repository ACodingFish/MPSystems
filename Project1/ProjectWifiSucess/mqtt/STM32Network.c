#include "STM32Network.h"

void NetworkInit(Network* net)
{
	net->mqttread = NetworkRead;
	net->mqttwrite = NetworkWrite;
}
int NetworkRead(Network* net, unsigned char* buff, int buff_sz, int timeout_ms)
{

}
int NetworkWrite(Network* net, unsigned char* buff, int buff_sz, int timeout_ms)
{

}
