#ifndef PARSE_H
#define PARSE_H

#include "login.h"
#include "helperfunctions.h"
#include <string.h>
#include <ctype.h>

void parse(int sockfd, char *tag, char *message_num);
char *extract_content(int sockfd, char *tag, char *message_num, char *header);
void remove_header(char message[BUFFER_SIZE], char *header);
int is_alphanumeric(char *string);

#endif