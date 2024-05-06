#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>

// #include <arpa/inet.h> , was used in server, so mayybe not required here
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define SERVER_PORT "143"
#define BUFFER_SIZE 256
#define LOGIN_CMD "LOGIN"
#define SELECT_CMD "SELECT"

/* begin arguments processing here */

#define MESSAGE_NOT_GIVEN -1

enum Input {
    COMMAND,
    SERVER_NAME,
    COMPLETE
};

typedef struct {
    char *username;
    char *password;
    char *folder;
    int message_num;
    char *command;
    char *server_name;
} inputs_t;

/* 
    ./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au 
    ./fetchmail retrieve -f Test unimelb-comp30023-2024.cloud.edu.au  -p pass -u test@comp30023 -n 1

*/
inputs_t *process_args(int argc, char **argv);
void print_inputs(inputs_t *inputs);

/* end argument processing here */

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
    sprintf(buffer, "%s %s %s %s\r\n", tag, LOGIN_CMD, username, password);
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
    sprintf(buffer, "%s %s %s\r\n", tag, SELECT_CMD, folder_name);
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


// read in function

inputs_t *process_args(int argc, char **argv) {
    /*
        Structure of program run statement:
        fetchmail-u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t]<command> <server_name>
        Where <command> may be one of: retrieve, parse, mime, or list.
    */

   /* make an inputs_t struct */
   inputs_t *inputs = (inputs_t *)malloc(sizeof(inputs_t)); assert(inputs);
   assert(inputs != NULL);

   /* initialise values */
   inputs->command = NULL;
   inputs->folder = NULL;
   inputs->message_num = MESSAGE_NOT_GIVEN;
   inputs->password = NULL;
   inputs->server_name = NULL;
   inputs->username = NULL;

   /* set input to command so we know what we're up to */
   enum Input input = COMMAND;

   /* now we add each element into the struct from argv */
   int i = 1;
   while (i < argc)
   {
        if (!(strcmp(argv[i], "-u")))
        {
            /* we have username next */
            assert((i + 1) < argc); i++;
            inputs->username = strdup(argv[i]);
            assert(inputs->username != NULL);
        }
        else if (!(strcmp(argv[i], "-f")))
        {
            /* we have the folder next */
            assert((i + 1) < argc); i++;
            inputs->folder = strdup(argv[i]);
            assert(inputs->folder != NULL);
        }
        else if (!(strcmp(argv[i], "-p")))
        {
            /* we have password next */
            assert((i + 1) < argc); i++;
            inputs->password = strdup(argv[i]);
            assert(inputs->password != NULL);
        }
        else if (!(strcmp(argv[i], "-n")))
        {
            /* we have the message number next */
            assert((i + 1) < argc); i++;
            inputs->message_num = atoi(argv[i]);
        }
        else if (input == COMMAND) 
        {
            inputs->command = strdup(argv[i]);
            assert(inputs->command);
            input++; // update since we have this value now
        }
        else if (input == SERVER_NAME)
        {
            inputs->server_name = strdup(argv[i]);
            assert(inputs->server_name);
            input++; // update since we have this value now
        }
        /* always increase regardless */
        i++;
   }
   return inputs;
}

void print_inputs(inputs_t *inputs) 
{
    /* function to print out the values of the command line arguments */
    if (inputs->username != NULL)
    {
        printf("Username: %s, ", inputs->username);
    }
    if (inputs->password != NULL)
    {
        printf("Password: %s, ", inputs->password);
    }
    if (inputs->folder != NULL)
    {
        printf("Folder: %s, ", inputs->folder);
    }
    if (inputs->server_name != NULL)
    {
        printf("Server: %s, ", inputs->server_name);
    }
    if (inputs->command != NULL)
    {
        printf("Command: %s, ", inputs->command);
    }
    if ((inputs->message_num) > MESSAGE_NOT_GIVEN)
    {
        printf("Number of messages: %d\n", inputs->message_num);
    }
}