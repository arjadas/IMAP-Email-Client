#ifndef MIME_H
#define MIME_H

#include "login.h"
#include "helperfunctions.h"
#include <stdlib.h>
#include <stdio.h>

void mime(int sockfd, char *tag, int message_num, char *folder_name);
int match_mime_version(int sockfd, char *tag, int message_num, char *folder_name);
int match_content_type(int sockfd, char *tag, int message_num, char *folder_name);
int match_type_subtype(int sockfd, char *tag, int message_num, char *folder_name);
int get_body_part(int sockfd, char *tag, int message_num, int body_part, char *folder_name);

#endif
