#ifndef FUNCTIONS
#define FUNCTIONS

#define REPORT_LENGTH 1024

char * getReport();
void clearReport();
void runOnSystem(char * cmd);
void shellCmd(void * params);
void mixFiles(void * params);
void cctFiles(void * params);
void downloadFile(void * params);
void splitAudio(void * params);

#endif
