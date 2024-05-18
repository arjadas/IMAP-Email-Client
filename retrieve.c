#include "retrieve.h"

void retrieve(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    int num_messages = 0, bytes_received = 0;

    modify_tag(tag);

    if (message_num != MESSAGE_NOT_GIVEN)
    {
        sprintf(buffer, "%s FETCH %d BODY.PEEK[]\r\n", tag, message_num);
        // printf("%s", buffer);

        write(sockfd, buffer, strlen(buffer));

        // receive response
        bytes_received = read(sockfd, buffer, BUFFER_SIZE - 1);
        // buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
        exit(0);
    }
    else
    { // get the last recent message's seq-number
        sprintf(buffer, "%s STATUS %s (MESSAGES)\r\n", tag, folder_name);

        write(sockfd, buffer, strlen(buffer));

        // receive response
        bytes_received = read(sockfd, buffer, BUFFER_SIZE - 1);
        // buffer[bytes_received] = '\0';
        printf("%s\n", buffer);

        // ** needs work- get the sequence number from the buffer, read the value besides MESSAGES

        if (num_messages == 0)
        {
            fprintf(stderr, "Message not found\n");
            exit(3);
        }
        else
        { // recursion back to the same function
            retrieve(sockfd, tag, num_messages, folder_name);
        }
    }
}