#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <jsonparser.h>

#include "silenceDetect.h"
#include "functions.h"

char report[REPORT_LENGTH];

char * getReport()
{
  return report;
}

void clearReport()
{
  memset(report, 0 , REPORT_LENGTH);
}

void runOnSystem(char * cmd)
{
  FILE * fp;

  strcat(report, cmd);
  fp = popen(cmd, "r");
  char tmpLine[100];
  while (fgets(tmpLine, sizeof(tmpLine), fp) != NULL)
  {
    strcat(report, tmpLine);
  }
}

void shellCmd(void * params)
{
  system((char *)params);
}

void mixFiles(void * params)
{
  mix * m;
  m = params;

  char cmd[500];
  char tmp[100];
  sprintf(cmd, "ffmpeg -nostdin -y ");
  int i;

  for(i = 0; i < m->countOfInputs; i++)
  {
    sprintf(tmp, "-i %s ", m->input + i * INOUTPUTLENGTH);
    strcat(cmd, tmp);
  }

  strcat(cmd, "-filter_complex \"");
  for(i = 0; i < m->countOfInputs; i++)
  {
    if(m->delay[i])
    {
      sprintf(tmp, "[%d]adelay=%d|%d[%c]; ", i, m->delay[i], m->delay[i], i+97);
      strcat(cmd, tmp);
    }
  }

  for(i = 0; i < m->countOfInputs; i++)
  {
    if(m->delay[i])
    {
      sprintf(tmp, "[%c]", i+97);
    }
    else
    {
      sprintf(tmp, "[%d]", i);
    }
    strcat(cmd, tmp);
  }
  sprintf(tmp, "amix=%d:duration=%s\" %s", m->countOfInputs, m->duration, m->output);

  strcat(cmd, tmp);
  strcat(cmd, " >> /home/jammnews/jammnews.log");
  system(cmd);
}

void cctFiles(void * params)
{
  cct * c;
  c = params;

  char cmd[500];
  sprintf(cmd, "ffmpeg -nostdin -y -i %s -i %s -filter_complex [0:a][1:a]concat=n=2:v=0:a=1 %s", c->input1, c->input2, c->output);
  strcat(cmd, " >> /home/jammnews/jammnews.log");
  system(cmd);
}

void downloadFile(void * params)
{
  download * dw;
  dw = params;
  char cmd[500];
  sprintf(cmd, "curl --silent -u %s:%s %s -o %s", dw->user, dw->pass, dw->url, dw->output);
  strcat(cmd, " >> /home/jammnews/jammnews.log");
  system(cmd);
}

void splitAudio(void * params)
{
  split * sp;
  sp = params;

  char cmd[500];

  silencesDetect sd = detectSlc(sp->input, sp->silenceBorder, sp->minSilenceLength);
  sprintf(cmd, "mkdir %s", sp->output);
  system(cmd);

  float start = 0;

  for(int i = 0; i < sd.countOfSilences; i++)
    printf("start: %f / end: %f\n", sd.slc[i].start, sd.slc[i].end);

  for(int i = 0; i < sd.countOfSilences; i++)
  {
    sprintf(cmd, "ffmpeg -nostdin -y -loglevel error -i %s -ss %f -t %f %s/split%d.wav 2>&1", sp->input, start, sd.slc[i].start-start, sp->output, i);
    start = sd.slc[i].end;
    runOnSystem(cmd);
  }
}
