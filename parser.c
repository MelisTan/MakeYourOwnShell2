#include "parser.h"

const char *cl_error_msgs[] = {
                                "Multiple pipes are not supported.",
                              };

void shell_print_error(CommandLine* cl) {
    if (cl->error_code > 0) {
        fprintf(stderr, "Error: %s\n", cl_error_msgs[cl->error_code - 1]);
    }
}

char** shell_tokenize_line(char *line, int *argc) {
    *argc = 1;
    char prev_white = (line[0] == ' ');
    int i;

    /* Cleanup leading and trailing whitespaces and
     * count the number of tokens. */
    for (i = 0; line[i] != '\0'; ++i) {
        if (line[i] == ' ' && !prev_white) {
            (*argc)++;
            prev_white = 1;
        }
        else if (line[i] != ' ') {
            prev_white = 0;
        }
    }
    *argc -= prev_white;

    /* Allocate an array of strings. The +1
     * is for the termination NULL ptr. */
    char **argv = malloc(sizeof(char *) * ((*argc) + 1));
    if (argv) {
        char *token = strtok(line, " ");
        i = 0;
        while (token) {
            argv[i++] = strdup(token);
            token = strtok(NULL, " ");
        }
        /* Put a NULL at the end */
        argv[i] = NULL;
    }
    return argv;
}

void shell_process_line(CommandLine *cl) {
    /* Reads a new line of maximum size 2048 bytes */
    char line[2048];

    /* Initialize all fields to 0 */
    memset(cl, 0, sizeof(CommandLine));

    if (fgets(line, sizeof(line), stdin) && line[0] != '\n') {

        /* Smash the newline character */
        line[strlen(line) - 1] = '\0';

        /*  Check for '!' which denotes the "last command"
        and use strcpy() function between line and last_line properly*/
        if(strcmp(line,"!")== 0 && last_line != NULL)
        {
            strcpy(line,last_line);
        }

        /*  Copy line to last_line */
        strcpy(last_line,line);


        /* Let's first try to tokenize from '|'.
         * If second_cmd is NULL, there isn't any |. */
        char *first_cmd = strtok(line, "|");
        char *second_cmd = strtok(NULL, "|");

        /* If there's multiple |'s, set the error code */
        if (strtok(NULL, "|")) {
            cl->error_code = CL_ERROR_MULTIPLE_PIPE;
        }
        else {
            cl->first_argv = shell_tokenize_line(first_cmd, &(cl->first_argc));
            if (second_cmd) {
                cl->second_argv = shell_tokenize_line(second_cmd, &(cl->second_argc));
                cl->has_pipe = 1;
            }
        }
    }
}

void shell_free_args(CommandLine* cl) {
    int i;
    if (cl->first_argv) {
        for (i = 0; cl->first_argv[i]; ++i) {
            free(cl->first_argv[i]);
        }
        free(cl->first_argv);
    }
    if (cl->second_argv) {
        for (i = 0; cl->second_argv[i]; ++i) {
            free(cl->second_argv[i]);
        }
        free(cl->second_argv);
    }
}
