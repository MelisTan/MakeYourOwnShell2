#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Include our own header files */
#include "parser.h"
#include "process.h"

#define VERSION "1.1"
#define PROMPT  ">"


int main(int argc, char *argv[])
{

    /* Komut satiri ile ilgili bilgileri tutan struct'i tanimliyoruz. */
    CommandLine cl;

    /* Yukaridaki cl degiskenini gosteren bir gosterici tanimliyoruz. */
    CommandLine *cl_ptr = &cl;

    /* Kullanicinin home dizinini kaydediyoruz. */
    char *home;
    home = getenv("HOME");

    while (1)
    {
        /* Ekrana PROMPT ile tanimli komut satiri bekleyicisini yaziyoruz. */
        printf("%s ", PROMPT);

        /* shell_process_line() ile satiri okuyup ayristiriyoruz. Artik cl_ptr
         * ile gosterilen CommandLine yapisindaki komut satirina dair bilgiler
         * var.
         */
        shell_process_line(cl_ptr);

        /* Eger komut satiri okunurken bir hata olduysa, error_code degiskeni
         * ayarlaniyor. shell_print_error() ise struct'taki error_code'un
         * degerine bakarak hata varsa ekrana bunu basiyor. continue ile
         * dongunun basina donuyoruz.
         */
        if (cl_ptr->error_code > 0) {
            shell_print_error(cl_ptr);
            continue;
        }

        /* Boru hatti olsun olmasin, cl_ptr->first_argv icinde bir komut yaziyor
         * olmali. Eger NULL ise kullanici hicbir sey yazmadan Enter'a basip
         * gecti demektir.
         */
        if (cl_ptr->first_argv != NULL) {
            /* Once gomulu komut mu degil mi diye bakiyoruz. Eger
             * komut gomuluyse, boru hatti olup olmamasiyla ilgilenmiyoruz,
             * dogrudan calistiriyoruz.
             */
            if (strcmp(cl_ptr->first_argv[0], "version") == 0) {
                printf("Shell version: %s\n", VERSION);
                shell_free_args(cl_ptr);
            }

            else if (strcmp(cl_ptr->first_argv[0], "exit") == 0) {
                shell_free_args(cl_ptr);
                exit(EXIT_SUCCESS);
            }

            else if (strcmp(cl_ptr->first_argv[0], "cd") == 0) {
                /* TODO: chdir fonksiyonunu uygun sekilde calistirip donus degerini alin. Fonksiyon icine gidilecek
                dizin bilgisi yazilmaldir. Dizin bilgisi parametre olarak yok ise basta alinan ev dizinine gidilmelidir.*/
                int changedir;

                if (cl_ptr->first_argv[1]!=NULL)
                {
                    changedir = chdir(cl_ptr->first_argv[1]);

                }
                else
                {
                    changedir = chdir(home);
                }

                /*  Donus degeri 0'dan kucuk ise ekrana hata mesaji basalim. */
                if( changedir < 0)
                {
                   perror("gsu_shell");
                }
                /*  cl_ptr icin tutulan bellek alanini yukaridaki orneklerdeki gibi sisteme geri verelim */
                shell_free_args(cl_ptr);
            }


            /* Komut satiri gomulu komutla baslamiyorsa, normal
             * prosedur isleyecek: Komut(lar) yeni yaratilan cocuk
             * surec(ler) tarafindan exec edilecek.
             *
             * Ayrintilar icin PDF'e bakin!
             */
            else
            {

                /* Cocuk sureclerin PID'lerini tutmak icin. */
                pid_t first_child, second_child;
                int exit_code1,exit_code2;
                int pipe_fd[2];
                int status;


                if (cl_ptr->has_pipe)
                {
                    pipe(pipe_fd);

                    if (pipe(pipe_fd) == -1)
                    {
                        fprintf(stderr, "Can't create pipe.");
                        shell_free_args(cl_ptr);
                    }

                    first_child = fork();
                    switch (first_child)
                    {
                        case -1:
                            perror("fork");
                            exit(1);
                            break;

                        case 0:

                            close(pipe_fd[0]);
                            dup2(pipe_fd[1], STDOUT_FILENO);

                            exit_code1= shell_exec_cmd(cl_ptr->first_argv);
                            exit(exit_code1);

                        default:
                            //parent's code
                            break;

                    }

                    second_child = fork();
                    switch (second_child)
                    {
                        case -1:
                            perror("fork");
                            exit(1);
                            break;

                        case 0:
                            close(pipe_fd[1]);
                            dup2(pipe_fd[0], STDIN_FILENO);


                            exit_code2 = shell_exec_cmd(cl_ptr->second_argv);
                            exit(exit_code2);


                        default:
                            //parent's code
                            break;
                    }

                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                    waitpid(first_child,&status,0);
                    waitpid(second_child,&status,0);


                }

                else
                {
                    first_child = fork();
                    switch (first_child)
                    {
                        case -1:
                            perror("fork");
                            exit(1);
                            break;

                        case 0:

                            exit_code1= shell_exec_cmd(cl_ptr->first_argv);
                            exit(exit_code1);

                        default:
                            //parent's code
                            waitpid(first_child,&status,0);
                            break;

                    }

                }





            } /* else */
        } /* if (cl_ptr) */
    } /* while(1) */

    return 0;
}

