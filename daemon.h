#ifndef DEAMON
#define DEAMON

#include <stdio.h>
void daemonize();

static char *pid_file_name = NULL;
static int pid_fd = -1;

#endif
