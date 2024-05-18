#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TAG_SIZE 5

/* begin arguments processing here */
#define MESSAGE_NOT_GIVEN -1

enum Input
{
    COMMAND,
    SERVER_NAME,
    COMPLETE
};

typedef struct
{
    char *username;
    char *password;
    char *folder;
    int message_num;
    char *command;
    char *server_name;
} inputs_t;

/*
    ./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au
    ./fetchmail retrieve -f Test unimelb-comp30023-2024.cloud.edu.au  -p pass -u test@comp30023 -n 1

*/
inputs_t *process_args(int argc, char **argv);
void print_inputs(inputs_t *inputs);

/* end argument processing here */

char *generate_tag();
void modify_tag(char *tag);

#endif