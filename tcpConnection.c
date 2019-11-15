#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include "tcpConnection.h"

int server_fd, new_socket;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
char buffer[BUFF_LENGTH] = {0};
pthread_t connectionThread;
pthread_t connectionRefuse;
uint8_t connected = 0;

uint8_t getConnState()
{
  return connected;
}

void clearConnBuffer()
{
  memset(buffer, 0 , BUFF_LENGTH);
}

char * readConnBuffer()
{
  return buffer;
}

void sendToConnection(char * txt)
{
  send(new_socket , txt , strlen(txt) , 0 );
}

int startConnectionThread()
{
  if(pthread_create(&connectionThread, NULL, newConnection, NULL))
  {
    perror("Thread creation error");
    return 1;
  }

  if(pthread_create(&connectionRefuse, NULL, refuseConnections, NULL))
  {
    perror("Thread creation error");
    return 1;
  }
  return 0;
}

void *refuseConnections()
{
  int sock;
  while(!connected);
  while(connected)
  {
    if ((sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
      perror("Connection error");
    }
    close(sock);
  }
  return 0;
}

void *newConnection()
{
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
  {
    perror("Connection error");
  }
  connected = 1;
  int read_size;
  while(connected)
  {
    while( (read_size = recv(new_socket , buffer , sizeof(buffer) , 0)) > 0 );
    if (read_size <= 0 )
    {
      connected = 0;
      close(new_socket);
      pthread_cancel(connectionRefuse);
      startConnectionThread();
    }
  }
  return 0;
}

void listenForConnection(int port)
{

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
      perror("socket failed");
      exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( port );


  // Forcefully attaching socket to the port
  if (bind(server_fd, (struct sockaddr *)&address,
                               sizeof(address))<0)
  {
      perror("bind failed");
      exit(EXIT_FAILURE);
  }


  if (listen(server_fd, 3) < 0)
  {
      perror("listen");
      exit(EXIT_FAILURE);
  }

  connected = 0;
  startConnectionThread();
}
