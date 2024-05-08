#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>

// #include <arpa/inet.h> , was used in server, so mayybe not required here
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "helperfunctions.h"

#define SERVER_PORT "143"
#define BUFFER_SIZE 256

int main(int argc, char **argv)
{
    inputs_t *inputs = process_args(argc, argv);
    print_inputs(inputs);

    int sockfd, status;
    struct addrinfo hints, *servinfo, *rp;
    /*
    servinfo is a linked list with all kinds of address information
    */
    char buffer[BUFFER_SIZE];

    char *server_name = "unimelb-comp30023-2024.cloud.edu.au";
    char *username, *password;
    char *folder_name = "INBOX";
    char *tag; // need to create a getTag function

    memset(&hints, 0, sizeof(hints)); // initialising all the bytes in the hints structure to 0
    hints.ai_family = AF_UNSPEC;      // allowing IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // setting the type of connection, in this case - TCP socket

    status = getaddrinfo(server_name, SERVER_PORT, &hints, &servinfo);

    /* getaddrinfo() returns a list of address structures.
              Try each address until we successfully connect(2).
              If socket(2) (or connect(2)) fails, we (close the socket
              and) try the next address. */

    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    // creating a socket
    for (rp = servinfo; rp != NULL; rp = rp->ai_next)
    {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        // ai_protocol is received from the getaddinfo
        // socket() returns a socket descriptor that can be used in later system calls, or -1 on error.

        if (sockfd == -1)
        {
            perror("socket"); // error occured while creating the socket
            continue;
        }

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("connect"); // error occured while connecting to the socket
            continue;
        }

        break; // Connected successfully
    }

    if (rp == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(servinfo); // free the memory as not needed anymore

    // loggin in to the IMAP server
    sprintf(buffer, "%s LOGIN %s %s\r\n", tag, username, password);
    write(sockfd, buffer, strlen(buffer));

    // do you think it's a good practice resetting the buffer after every action?
    // memset(buffer, 0, BUFFER_SIZE);

    // receiving response
    read(sockfd, buffer, BUFFER_SIZE - 1); // -1 to for the null terminator at the end
    printf("%s\n", buffer);

    // check if login was successful
    if (strstr(buffer, "OK") == NULL) // is it better to convert response to lower case and compare or compare twice OK & ok?
    {
        fprintf(stderr, "Login failure\n");
        exit(3);
    }

    // tell the system which folder to read from
    sprintf(buffer, "%s SELECT %s\r\n", tag, folder_name);
    write(sockfd, buffer, strlen(buffer));

    // receive response
    read(sockfd, buffer, BUFFER_SIZE - 1);
    printf("%s\n", buffer);

    // Check if folder exists
    if (strstr(buffer, "OK") == NULL)
    {
        fprintf(stderr, "Folder not found\n");
        exit(3);
    }

    close(sockfd); // closing the socket at end

    return 0;
}
