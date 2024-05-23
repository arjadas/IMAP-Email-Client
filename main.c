#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "helperfunctions.h"
#include "login.h"
#include "retrieve.h"
#include "mime.h"
#include "parse.h"
#include "list.h"

int main(int argc, char **argv)
{
    inputs_t *inputs = process_args(argc, argv);

    // establish connection to the server
    int sockfd = connect_to_server(inputs);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    char *tag = generate_tag();

    // logging in
    login(sockfd, tag, inputs);

    // select the folder
    select_folder(sockfd, tag, inputs);

    // run function depending on command
    if (strcmp(inputs->command, "retrieve") == 0)
    {
        retrieve(sockfd, tag, inputs);
    }
    else if (strcmp(inputs->command, "parse") == 0)
    {
        parse(sockfd, tag, inputs);
    }
    else if (strcmp(inputs->command, "mime") == 0)
    {
        mime(sockfd, tag, inputs);
    }
    else if (strcmp(inputs->command, "list") == 0)
    {
        list(sockfd, tag);
    }

    close(sockfd); // closing the socket at end

    free(tag);
    free_input(inputs);

    return 0;
}