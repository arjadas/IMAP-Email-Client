#include "retrieve.h"

void retrieve(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int bytes_received = 1;
    char *line;
    int print_line = 0; // whether to print the line or not
    int body_end = 0;

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

    memset(buffer, 0, BUFFER_SIZE); // resetting the buffer

    sprintf(temp, ")\r\n%s OK Fetch completed", tag);
    // printf("%s\n", temp);

    // receive response
    while (bytes_received != 0 && body_end != 1)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);

        printf("\n%d*************************************************\n", bytes_received);

        line = strtok(buffer, "\r\n");
        // printf("this line- %s\n", line);

        while (line != NULL)
        {
            if (strstr(line, "(BODY[]"))
            {
                print_line = 1;
                // continue;
            }

            if (strstr(line, temp) != NULL)
            {
                print_line = 0;
                body_end = 1;
                // continue;
            }

            if (print_line == 1 && line[0] != '*')
            {
                printf("%s\r\n", line);
            }

            line = strtok(NULL, "\r\n");
        }

        // printf("%s", buffer);

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }
    exit(0);
}