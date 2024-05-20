#include "parse.h"

void parse(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;

    modify_tag(tag);

    if (message_num != MESSAGE_NOT_GIVEN)
    {
        sprintf(buffer, "%s FETCH %d BODY.PEEK[HEADER.FIELDS (TO)]\r\n", tag, message_num);
        // printf("%s", buffer);
    }
    else
    { // get the last recent message
        sprintf(buffer, "%s FETCH * BODY.PEEK[]\r\n", tag);
        // printf("%s", buffer);
    }

    /*if ( == 0)
    {
        fprintf(stderr, "Message not found\n");
        exit(3);
    }*/

    write(sockfd, buffer, strlen(buffer));

    memset(buffer, 0, BUFFER_SIZE); // resetting the buffer

    // receive response
    while (bytes_received != 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);

        printf("%s", buffer);
    }
    exit(0);
}