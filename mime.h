#ifndef MIME_H
#define MIME_H

#define MIME_VERSION "mime-version: 1.0"
#define CONTENT_TYPE "content-type: multipart/alternative"

#include "login.h"
#include "helperfunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

void mime(int sockfd, char *tag, char *message_num, char *folder_name);
int match_mime_version(int sockfd, char *tag, int message_num, char *folder_name);
int match_content_type(int sockfd, char *tag, int message_num, char *folder_name);
int get_body_part(int sockfd, char *tag, int message_num, char *folder_name);
char *find_part(char *start, char *end);
void print_body_part(int sockfd, char *tag, int message_num, int body_part, char *folder_name);
char *str_to_lower(char *str);
char *get_output_string(char *original_str, int *length);

#endif
