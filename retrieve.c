#include "retrieve.h"

void retrieve(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    int num_messages = 0;

    if (message_num != MESSAGE_NOT_GIVEN)
    {
        sprintf(buffer, "%s FETCH %d BODY.PEEK[]\r\n", tag, message_num);
        printf("%s", buffer); // printing the raw message
        exit(0);
    }
    else
    { // get the last recent message's seq-number
        sprintf(buffer, "%s STATUS %s (MESSAGES)\r\n", tag, folder_name);

        // get the sequence number from the buffer, read the value besides MESSAGES

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