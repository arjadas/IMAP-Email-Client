#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define TAG_SIZE 5
#define INBOX_LEN 5
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 1024
#define FETCH_COMPLETED "OK Fetch completed"
#define NULL_BYTE '\0'

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

inputs_t *process_args(int argc, char **argv);
void print_inputs(inputs_t *inputs);
int validate_inputs(inputs_t *inputs);
int check_characters(char *string);
int is_seqnum(char *message_num);
int injection(inputs_t *inputs);

char *generate_tag();
void modify_tag(char *tag);

/* for getting server response for MIME */
char *get_fetch_line(int sockfd, char *completed_message);
char *add_buffer_to_output_string(char *output, char *buffer, int bytes_received);
void free_input(inputs_t *inputs);

#endif