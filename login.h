#ifndef LOGIN_H
#define LOGIN_H

#define _POSIX_C_SOURCE 200112L

#define SERVER_PORT "143"
#define BUFFER_SIZE 1024

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