#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int shell_exec_cmd(char **argv);

#endif
