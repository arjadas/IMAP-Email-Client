#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>


#define TAG_SIZE 5
#define INBOX_LEN 5
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 1024
#define FETCH_COMPLETED "OK Fetch completed"
#define NULL_BYTE '\0'


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
    char *message_num;
    char *command;
    char *server_name;
} inputs_t;

/*
    ./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au
    ./fetchmail retrieve -f Test unimelb-comp30023-2024.cloud.edu.au  -p pass -u test@comp30023 -n 1

*/
inputs_t *process_args(int argc, char **argv);
void print_inputs(inputs_t *inputs);
int validate_inputs(char *input);

/* end argument processing here */

char *generate_tag();
void modify_tag(char *tag);

/* for getting server response for MIME and LIST */
char *get_fetch_line(int sockfd, char *completed_message);
char *add_buffer_to_output_string(char *output, char *buffer, int bytes_received);

#endif