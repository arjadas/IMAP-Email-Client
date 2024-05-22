#include "retrieve.h"

void retrieve(int sockfd, char *tag, char *message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    int is_body = 0; // whether to print the line or not
    int body_end = 0;

    modify_tag(tag);

    if (message_num != NULL)
    {
        sprintf(buffer, "%s FETCH %s BODY.PEEK[]\r\n", tag, message_num);
    }
    else
    { // get the last recent message
        sprintf(buffer, "%s FETCH * BODY.PEEK[]\r\n", tag);
    }

    write(sockfd, buffer, strlen(buffer));

    memset(buffer, 0, BUFFER_SIZE); // resetting the buffer

    FILE *file = fdopen(sockfd, "r");
    assert(file);

    sprintf(temp, "%s OK Fetch completed", tag);

    // receive response
    while ((fgets(buffer, BUFFER_SIZE, file) != NULL) && body_end != 1)
    {
        if (strstr(buffer, "FETCH (BODY[]"))
        {
            is_body = 1;
            continue;
        }
        else if (buffer[0] == '*' && is_body == 0)
        {
            continue;
        }
        else if (strstr(buffer, ")\r\n") && is_body == 1)
        {
            strcpy(line, buffer);
            fgets(buffer, BUFFER_SIZE, file);

            if (strstr(buffer, temp) != NULL)
            {
                is_body = 0;
                body_end = 1;
                break;
            }
            else
            {

                printf("%s", line);
            }
        }
        else if (strstr(buffer, "BAD Error"))
        {
            printf("Message not found\n");
            exit(3);
        }

        if (is_body == 1)
        {
            printf("%s", buffer);
        }

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }
    exit(0);
}