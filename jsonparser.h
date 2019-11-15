#ifndef JSONPARSER
#define JSONPARSER

#include "3dparty/cJSON/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "buffer.h"

buffer * tasksBuffer;
char * textBuffer;

#define INOUTPUTLENGTH 100

typedef struct
{
  char url[200];
  char output[INOUTPUTLENGTH];
  char user[50];
  char pass[100];
} download;

typedef struct
{
  uint8_t countOfInputs;
  char * input;
  uint32_t * delay;
  char duration[10];
  char output[INOUTPUTLENGTH];
} mix;

typedef struct
{
  char input1[INOUTPUTLENGTH];
  char input2[INOUTPUTLENGTH];
  char output[100];
} cct;

typedef struct
{
  char input[INOUTPUTLENGTH];
  char output[INOUTPUTLENGTH];
  int8_t silenceBorder;
  float minSilenceLength;
} split;

typedef struct
{
  uint8_t repeat;
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  void (*func)(void *);
  void * params;
  uint8_t lock;
  uint8_t runnow;
} task;


int readConfig(char * configFile);
int parseConfig();
void freeTasks();

#endif
