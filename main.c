#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>

// #include <arpa/inet.h> , was used in server, so mayybe not required here
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#define SERVER_PORT "143"
#define BUFFER_SIZE 256

int main(int argc, char **argv)
{

    int sockfd, status;
    struct addrinfo hints, *servinfo, *rp;
    /*
    servinfo is a linked list with all kinds of address information
    */
    char buffer[BUFFER_SIZE];

    char *server_name = "unimelb-comp30023-2024.cloud.edu.au";

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

    return 0;
}