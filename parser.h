#ifndef _PARSER_H_
#define _PARSER_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CL_ERROR_MULTIPLE_PIPE  1

char last_line[2048];

typedef struct CommandLine {
    char **first_argv, **second_argv;
    int first_argc, second_argc;

    /* 1 if there is | in the command line */
    int has_pipe;

    /* An error_code for further usage */
    int error_code;

    /* 1 if the command will be launched in the background */
    int background;
} CommandLine;

char** shell_tokenize_line(char *line, int *argc);
void shell_free_args(CommandLine* cl);
void shell_process_line(CommandLine* cl);
void shell_print_error(CommandLine* cl);

#endif
