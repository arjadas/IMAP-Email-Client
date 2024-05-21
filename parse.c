#include "parse.h"

void parse(int sockfd, char *tag, int message_num, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    char end_message_ok[BUFFER_SIZE];
    // char temp[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    char content[BUFFER_SIZE] = {'\0'};
    int is_body = 0, body_end = 0;
    int content_present = 0;
    int curr_len = 0;
    // int bytes_received = 1;

    modify_tag(tag);

    if (message_num != MESSAGE_NOT_GIVEN)
    {
        sprintf(buffer, "%s FETCH %d BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n", tag, message_num);
        // printf("%s", buffer);
    }
    else
    { // get the last recent message
        sprintf(buffer, "%s FETCH * BODY.PEEK[HEADER.FIELDS (FROM)]\r\n", tag);
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

    sprintf(end_message_ok, "%s OK Fetch completed", tag);
    // printf("%s\n", end_message_ok);

    // receive response
    while ((fgets(buffer, BUFFER_SIZE, file) != NULL) && body_end != 1)
    {

        // printf("\n%d*************************************************\n", (int)strlen(buffer));
        // printf("%s", buffer);

        // printf("%c", buffer[0]);

        if (strstr(buffer, "FETCH (BODY["))
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
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, file);

            if (strstr(buffer, end_message_ok) != NULL)
            {
                is_body = 0;
                body_end = 1;
                break;
            }
            else
            {
                // printf("%s", line);
                strcat(content, line);
            }
        }
        else if (strstr(buffer, "BAD Error"))
        {
            printf("Message not found\n");
            exit(3);
        }

        if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 0)
        { // reading the first line
            // printf("%s", buffer);
            content_present = 1;
            strcat(content, buffer);
        }
        else if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 1)
        { // reading the folded lines

            curr_len = strlen(content);
            content[curr_len - 1] = '\0';
            content[curr_len - 2] = '\0';

            strcat(content, buffer);
        }

        // printf("%s", buffer);

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }
    printf("%s", content);

    /*while (bytes_received != 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);

        printf("%s", buffer);
    }*/

    exit(0);
}

int is_alphanumeric(char *string)
{

    char *str = string;

    while (*str)
    {
        if (isalnum(*str))
            return 1; // found alphanumeric

        str++;
    }

    return 0; // didn't find alphanumeric
}