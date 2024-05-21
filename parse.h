#ifndef PARSE_H
#define PARSE_H

#include "login.h"
#include "helperfunctions.h"
#include <string.h>
#include <ctype.h>

void parse(int sockfd, char *tag, int message_num);
char *extract_content(int sockfd, char *tag, int message_num, char *header);
int is_alphanumeric(char *string);

#endif