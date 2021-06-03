//------------------------------------------------------------------------
// NAME: Николай Динков
// CLASS: XIv
// NUMBER: 16
// PROBLEM: SHELL (2nd BigHomework)
// FILE NAME: shell.c (unix file name)
// FILE PURPOSE: 
// Реализация на команден интерпретатор shell
//------------------------------------------------------------------------
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//------------------------------------------------------------------------
// FUNCTION: read_line (име на функцията)
// Функцията чете от стандартния вход
// PARAMETERS:
// Няма
//------------------------------------------------------------------------
char *read_line()
{
    char *cmd_line = NULL;
    size_t bytes = 0;
    int charac = 0;
    charac = getline(&cmd_line, &bytes, stdin);
    if(charac == 1)
    {
        free(cmd_line);
        return 0;
    }
    if(charac == -1)
    {
        free(cmd_line);
        //printf("\n");
        exit(0);
    }
    cmd_line[charac - 1] = '\0';
    //printf("%s\n", cmd_line);
    //printf("%d\n", charac);
    return cmd_line;
}

//------------------------------------------------------------------------
// FUNCTION: count_needed_whitespaces (име на функцията)
// Функцията брои само нужните бели места, с други думи, 
// когато не се повтарят едни след други
// PARAMETERS:
// const char *cmdline - изчетения стринг от read_line
//------------------------------------------------------------------------
int count_needed_whitespaces(const char *cmdline)
{
    int to_count = 0;
    int counter = 0;
    if(cmdline[strlen(cmdline) - 1] != ' ')
    {
        counter++;
    }
    for(int i = 0; cmdline[i] != '\0'; i++)
    {
        if(cmdline[i] != ' ')
        {
            to_count++;
        }
        if(cmdline[i] == ' ' && to_count > 0 && cmdline[i - 1] != ' ')
        {
            //printf("IT is on %d\n", i);
            counter++;
        }
    }
    return counter;
}

//------------------------------------------------------------------------
// FUNCTION: parse_cmdline (име на функцията)
// Функцията разделя подадения стринг на масив от стрингове 
// като пропуска белите места и не добавя празните стрингове към масива
// PARAMETERS:
// const char *cmdline - изчетения стринг от read_line
//------------------------------------------------------------------------
char **parse_cmdline(const char *cmdline)
{
    int sizeForMem = 1000;
    int needed_whitespaces = count_needed_whitespaces(cmdline);
    char **parsed_cmdline = calloc(needed_whitespaces + 1, sizeof(char*));
    parsed_cmdline[needed_whitespaces] = NULL;
    //printf("!: %d\n", needed_whitespaces);
    //printf("strlen: %ld\n", strlen(cmdline));

    int string_in_count = 0;
    int on = 0;
    for(int i = 0; i < needed_whitespaces; i++)
    {
        char *new_space;
        if(cmdline[strlen(cmdline) - 1] != ' ' && i == needed_whitespaces - 1)
        {
            //printf("1\n");
            new_space = strchr(cmdline + on, '\0') + 1;
        }else{
            new_space = strchr(cmdline + on, ' ') + 1;
        }
        parsed_cmdline[string_in_count] = calloc(sizeForMem, sizeof(char));
        int j = -1;
        for(int i = on; i < new_space - (cmdline + on) + on - 1; i++)
        {
            if(cmdline[i] != ' ')
            {
                //printf("%c\n", cmdline[i]);
                parsed_cmdline[string_in_count][++j] = cmdline[i];
                if(j + 1 == 100)
                {
                    sizeForMem *= 2;
                    parsed_cmdline[string_in_count] = realloc(parsed_cmdline[string_in_count], sizeForMem * sizeof(char));
                }
            }
        }
        //printf("%s\n", parsed_cmdline[string_in_count]);
        on += new_space - (cmdline + on);
        if(strlen(parsed_cmdline[string_in_count]) == 0)
        {
            free(parsed_cmdline[string_in_count]);
            needed_whitespaces++;
        }else{
            string_in_count++;
        }
    }
    //printf("%d\n", string_in_count);
    return parsed_cmdline;
}

int main()
{
    while(1)
    {
        printf("$ ");
        char *theline = read_line();
        if(theline == 0)
        {
            continue;
        }
        char **masslines = parse_cmdline(theline);
        //printf("%s\n", theline);

        pid_t pid = fork();
        int child_status;

        if(pid == 0)
        {
            //printf("THERE: %s\n", masslines[0]);
            int err = execv(masslines[0], masslines);
            if(err == -1)
            {
                perror(masslines[0]);
                int fordel = count_needed_whitespaces(theline);
                free(theline);
                for(int i = 0; i < fordel + 1; i++)
                {
                    free(masslines[i]);
                }
                free(masslines);
                exit(0);
            }
        }else if(pid > 0){
            pid_t err = waitpid(pid, &child_status, 0);
            if(err != pid)
            {
                perror("a.out");
            }
        }else{
            perror("fork");
        }
        //printf("There   !   ");
        int fordel = count_needed_whitespaces(theline);
        free(theline);
        for(int i = 0; i < fordel + 1; i++)
        {
            free(masslines[i]);
        }
        free(masslines);
    }
    return 0;
}
