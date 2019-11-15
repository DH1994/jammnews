#include "jsonparser.h"
#include "buffer.h"
#include "functions.h"
#include "3dparty/b64/b64.h"

void * saveSettingArr(cJSON * item, char * object, uint8_t * length, uint8_t elementSize);
void saveSettingString(cJSON * item, char * object, char * setting);
int saveSettingVal(cJSON * item, char * object);
void saveTask(cJSON *item, void * param);

int readConfig(char * configFile)
{
  long length;
  FILE * f = fopen (configFile, "rb");

  if (f)
  {
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    textBuffer = malloc (length);
    if (textBuffer)
    {
      fread (textBuffer, 1, length, f);
    }
    fclose (f);
  }

  if (textBuffer)
  {
    return 1;
  }
  return 0;
}

int parseConfig(char * configFile)
{
  if(readConfig(configFile))
  {
    cJSON * json = cJSON_Parse(textBuffer);
    cJSON * tasks = cJSON_GetObjectItemCaseSensitive(json, "tasks");

    uint16_t arrLength = cJSON_GetArraySize(tasks);

    tasksBuffer = initBuffer(arrLength, sizeof(task));
    for(int i = 0; i < arrLength; i++)
    {
      cJSON * task = cJSON_GetArrayItem(tasks, i);

      saveTask(task, NULL);
    }
    free(textBuffer);
    return 0;
  }
  return 1;
}

void saveTask(cJSON *item, void * param)
{
  cJSON * action = cJSON_GetObjectItemCaseSensitive(item, "action");

  if(!bufferFull(tasksBuffer))
  {
    if (cJSON_IsString(action) && (action->valuestring))
    {
      task t;
      t.lock = 0;
      t.hour = saveSettingVal(item, "hour");
      t.min = saveSettingVal(item, "min");
      t.sec = saveSettingVal(item, "sec");
      t.repeat = saveSettingVal(item, "repeat");
      t.runnow = 0;

      if(strstr(action->valuestring, "cmd"))
      {
        t.params = malloc(200);
        t.func = &shellCmd;
        saveSettingString(item, "cmd", t.params);
      }

      if(strstr(action->valuestring, "download"))
      {
        t.params = malloc(sizeof(download));
        download * tmp = t.params;
        t.func = &downloadFile;
        saveSettingString(item, "output", tmp->output);
        saveSettingString(item, "url", tmp->url);
        saveSettingString(item, "user", tmp->user);
        char pass[100];
        saveSettingString(item, "pass", pass);
        strcpy(tmp->pass, (char *)b64_decode(pass, strlen(pass)));
      }

      if(strstr(action->valuestring, "cct"))
      {
        t.params = malloc(sizeof(cct));
        cct * tmp = t.params;
        t.func = &cctFiles;
        saveSettingString(item, "input1", tmp->input1);
        saveSettingString(item, "input2", tmp->input2);
        saveSettingString(item, "output", tmp->output);
      }

      if(strstr(action->valuestring, "mix"))
      {
        t.params = malloc(sizeof(mix));
        mix * tmp = t.params;
        t.func = &mixFiles;

        tmp->input = (char *)saveSettingArr(item, "input", &tmp->countOfInputs, INOUTPUTLENGTH);
        tmp->delay = (uint32_t *)saveSettingArr(item, "delay", &tmp->countOfInputs, sizeof(uint32_t));
        saveSettingString(item, "output", tmp->output);
        saveSettingString(item, "duration", tmp->duration);
      }

      if(strstr(action->valuestring, "split"))
      {
        t.params = malloc(sizeof(split));
        split * tmp = t.params;
        t.func = &splitAudio;

        saveSettingString(item, "input", tmp->input);
        saveSettingString(item, "output", tmp->output);

        float minSlcLength = (float)saveSettingVal(item, "minSilenceLength");
        minSlcLength /= 100;
        tmp->minSilenceLength = minSlcLength;
        tmp->silenceBorder = saveSettingVal(item, "silenceBorder");
      }

      writeBuffer(tasksBuffer, &t);
    }
  }
}

void * saveSettingArr(cJSON * item, char * object, uint8_t * length, uint8_t elementSize)
{
  char * setting;
  cJSON * arr = cJSON_GetObjectItemCaseSensitive(item, object);
  uint8_t arrLength = cJSON_GetArraySize(arr);

  setting = malloc(arrLength * elementSize);
  *length = arrLength;
  for(int i = 0; i < arrLength; i++)
  {
    cJSON * tmp = cJSON_GetArrayItem(arr, i);

    if (cJSON_IsString(tmp) && (tmp->valuestring != NULL))
    {
      strcpy((setting + i * elementSize), tmp->valuestring);
    }
    else
    {
      if (cJSON_IsNumber(tmp) && (tmp->valueint))
      {
        memcpy((setting + i * elementSize), &tmp->valueint, elementSize);
      }
      else
      {
        memset((setting + i * elementSize), 0 , elementSize);
      }
    }
  }
  return setting;
}

void saveSettingString(cJSON * item, char * object, char * setting)
{
  cJSON * objectItem = cJSON_GetObjectItemCaseSensitive(item, object);
	if (cJSON_IsString(objectItem) && (objectItem->valuestring != NULL))
	{
		strcpy(setting, objectItem->valuestring);
	}
}

int saveSettingVal(cJSON * item, char * object)
{
  cJSON * objectItem = cJSON_GetObjectItemCaseSensitive(item, object);
  if (cJSON_IsNumber(objectItem) && (objectItem->valueint))
  {
    return objectItem->valueint;
  }

  if (cJSON_IsBool(objectItem))
  {
    if(cJSON_IsTrue(objectItem))
    {
      return 1;
    }
  }
  return 0;
}

void freeTasks()
{
  for(int i = 0; i < tasksBuffer->size; i++)
  {
    task * tmp = readBuffer(tasksBuffer);
    if(tmp->func == mixFiles)
    {
      mix * m;
      m = tmp->params;
      free(m->delay);
      free(m->input);
    }
    free(tmp->params);
  }
  freeBuff(tasksBuffer);
  free(tasksBuffer);
  free(textBuffer);
}
