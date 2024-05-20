#include "retrieve.h"

void retrieve(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int bytes_received = 1;
    char line[BUFFER_SIZE];
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

    FILE *file = fdopen(sockfd, "r");
    assert(file);

    memset(buffer, 0, BUFFER_SIZE); // resetting the buffer

    sprintf(temp, "%s OK Fetch completed", tag);
    // printf("%s\n", temp);

    // receive response
    while ((fgets(buffer, BUFFER_SIZE, file) != NULL) && body_end != 1)
    {

        // printf("\n%d*************************************************\n", (int)strlen(buffer));

        if (strstr(buffer, "(BODY[]"))
        {
            print_line = 1;
            // continue;
        }

        if (strstr(buffer, ")\r\n"))
        {
            strcpy(line, buffer);
            fgets(buffer, BUFFER_SIZE, file);

            if (strstr(buffer, temp) != NULL)
            {
                print_line = 0;
                body_end = 1;
                break;
            }
            else
            {
                if (buffer[0] != '*')
                    printf("%s", line);
            }
        }

        if (print_line == 1 && buffer[0] != '*')
        {
            printf("%s", buffer);
        }

        // printf("%s", buffer);

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }
    exit(0);
}