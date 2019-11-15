#ifndef TCPCONNECTION
#define TCPCONNECTION

#include <stdint.h>

#define BUFF_LENGTH 2048

int check();
void clearConnBuffer();
char * readConnBuffer();
int startConnectionThread();
void *newConnection();
void *refuseConnections();
void listenForConnection(int port);
void sendToConnection(char * txt);
uint8_t getConnState();
char * getReport();

#endif
