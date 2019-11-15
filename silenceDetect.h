#ifndef silenceDetect
#define silenceDetect

#include <stdint.h>

typedef struct
{
  float start;
  float end;
} silence;

typedef struct
{
  uint8_t countOfSilences;
  silence slc[100];
} silencesDetect;

silencesDetect detectSlc(char * input, int8_t db, float length);

#endif
