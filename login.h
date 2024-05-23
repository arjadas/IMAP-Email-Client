#ifndef LOGIN_H
#define LOGIN_H

#define SERVER_PORT "143"

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "helperfunctions.h"

int connect_to_server(inputs_t *inputs);
void login(int sockfd, char *tag, inputs_t *inputs);
void select_folder(int sockfd, char *tag, inputs_t *inputs);

#endif