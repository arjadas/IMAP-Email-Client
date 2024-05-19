#define _POSIX_C_SOURCE 200112L // maybe removed

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// they may be remvoed as well
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "helperfunctions.h"
#include "login.h"
#include "retrieve.h"

int main(int argc, char **argv)
{
    inputs_t *inputs = process_args(argc, argv);
    // print_inputs(inputs);

    // establish connection to the server
    int sockfd = connect_to_server(inputs->server_name);
    if (sockfd == -1)
    {
        fprintf(stderr, "Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    char *tag = generate_tag();

    // logging in
    login(sockfd, tag, inputs->username, inputs->password);

    // select the folder
    select_folder(sockfd, tag, inputs->folder);

    // retrieve the email if command line says retrieve
    retrieve(sockfd, tag, inputs->message_num, inputs->folder);

    close(sockfd); // closing the socket at end

    free(tag);

    return 0;
}