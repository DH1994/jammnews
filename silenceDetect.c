#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "silenceDetect.h"

silencesDetect detectSlc(char * input, int8_t db, float length)
{
  FILE * fp;
  char tmp[100];

  char cmd[100];
  sprintf(cmd, "ffmpeg -i %s -af silencedetect=n=%ddB:d=%f -f null - 2>&1 | grep silence", input, db, length);
  fp = popen(cmd, "r");
  silencesDetect slcd;
  uint8_t countOfSlc = 0;

  while (fgets(tmp, sizeof(tmp), fp) != NULL)
  {
    char outputTxt[15];
    float slc;
    silence s;

    sscanf(tmp, "[silencedetect @ %*s%s %f%*s", &outputTxt[0], &slc);

    if(strstr(outputTxt, "silence_start"))
    {
        s.start = slc;
    }

    if(strstr(outputTxt, "silence_end:"))
    {
      s.end = slc;
      slcd.slc[countOfSlc] = s;
      slcd.countOfSilences = countOfSlc++;
    }
  }

  pclose(fp);

  return slcd;
}
