#include "retrieve.h"

void retrieve(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int bytes_received = 1;
    char *line;

    modify_tag(tag);

    if (message_num != MESSAGE_NOT_GIVEN)
    {
        sprintf(buffer, "%s FETCH %d BODY.PEEK[]\r\n", tag, message_num);
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

    sprintf(temp, "%s OK", tag);
    printf("%s\n", temp);

    // receive response
    while (bytes_received > 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
        // printf("%d\n", bytes_received);

        line = strtok(buffer, "\n");

        while (line != NULL)
        {
            if (strstr(line, temp) == NULL || strstr(line, "* ") == NULL)
            {
                printf(" %s\n", line);
            }
            else if (strstr(line, ")\r\n"))
            {
                break;
            }

            line = strtok(NULL, "\n");
        }
    }
    exit(0);
}