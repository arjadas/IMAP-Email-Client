#ifndef LOGIN_H
#define LOGIN_H

#define SERVER_PORT "143"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int connect_to_server(char *server_name);
void login(int sockfd, char *tag, char *username, char *password);
void select_folder(int sockfd, char *tag, char *folder_name);

#endif