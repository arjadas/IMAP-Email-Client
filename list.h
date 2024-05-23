#ifndef LIST_H
#define LIST_H

#include "login.h"
#include "helperfunctions.h"
#include "parse.h"

void list(int sockfd, char *tag);
char *extract_content_list(int sockfd, char *tag, char *message_num, char *header);

#endif