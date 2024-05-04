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

    int socket_fd, status;
    struct addrinfo hints, *servinfo, *rp;
    char buffer[BUFFER_SIZE];

    char *server_name = "unimelb-comp30023-2024.cloud.edu.au";

    memset(&hints, 0, sizeof(hints)); // initialising all the bytes in the hints structure to 0
    hints.ai_family = AF_UNSPEC;      // allowing IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // setting the type of connection, in this case - TCP socket

    status = getaddrinfo(server_name, SERVER_PORT, &hints, &servinfo);

    // creating a socket
    for (rp = servinfo; rp != NULL; rp = rp->ai_next)
    {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); // ai_protocol is received from the getaddinfo

        if (socket_fd == -1)
        {
            perror("socket"); // error occured while creating the socket
            continue;
        }

        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) != -1)
        {
            close(socket_fd);
            perror("connect"); // error occured while connecting to the socket
            continue;
        }

        break; // Connected successfully
    }

    return 0;
}