#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <syslog.h>

#include "daemon.h"
#include "functions.h"
#include "jsonparser.h"
#include "tcpConnection.h"

//#define DAEMON
#define CONFIG_PATH "/home/jammnews/jammnews/tasks.json"
#define PORT 4444

int main(int argc, char const *argv[])
{
  #ifdef DAEMON
  daemonize();
  #endif

  parseConfig(CONFIG_PATH);

  time_t     now;
  struct tm  ts;
  task * t;


  if(bufferAvailable(tasksBuffer))
  {
    t = readBuffer(tasksBuffer);
  }

  listenForConnection(PORT);

  while(1)
  {
    time(&now);
    ts = *localtime(&now);

    if(strlen(readConnBuffer()) > 0)
    {
      char tmp[50] = {0};
      strcpy(tmp, readConnBuffer());

      if(strstr(tmp,"getConf"))
      {
        if(readConfig(CONFIG_PATH))
        {
          sendToConnection(textBuffer);
          free(textBuffer);
        }
      }

      if(strstr(readConnBuffer(), "runnow"))
      {
        linkedListItem * tmp;
        tmp = tasksBuffer->first;

        int runnow;
        sscanf(readConnBuffer(), "runnow(%d)", &runnow);

        for(int i = 0; i < tasksBuffer->size; i ++)
        {
          if(i == runnow)
          {
            task * t;
            t = tmp->data;
            t->runnow = 1;
          }
          tmp = tmp->next;
        }
      }
      clearConnBuffer();
      tmp[0] = 0;
    }


    if(((ts.tm_hour == t->hour || t->repeat) && ts.tm_min == t->min && ts.tm_sec > t->sec) || t->runnow)
    {
      if(!t->lock)
      {
        t->func(t->params);
        t->runnow = 0;
        t->lock = 1;
        if(getConnState())
        {
          sendToConnection(getReport());
          clearReport();
        }

      }
      else
      {
        if(bufferAvailable(tasksBuffer))
        {
          t = readBuffer(tasksBuffer);
        }
      }
    }
    else
    {
      if(bufferAvailable(tasksBuffer))
      {
        t = readBuffer(tasksBuffer);
      }
      usleep(100000);
    }

    if(t->lock)
    {
      if(ts.tm_min > t->min || ts.tm_min < t->min)
      {
        t->lock = 0;
      }
    }
  }

  freeTasks();
  syslog (LOG_NOTICE, "jammnews terminated.");
  closelog();
  return EXIT_SUCCESS;
}
