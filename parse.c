#include "parse.h"

void parse(int sockfd, char *tag, char *message_num)
{
    char *message;

    message = extract_content_parse(sockfd, tag, message_num, "From");
    printf("From: %s\n", message);

    message = extract_content_parse(sockfd, tag, message_num, "To");
    if (message != NULL)
        printf("To: %s\n", message);
    else
        printf("To:\n");

    message = extract_content_parse(sockfd, tag, message_num, "Date");
    printf("Date: %s\n", message);

    message = extract_content_parse(sockfd, tag, message_num, "Subject");
    printf("Subject: %s\n", message);

    free(message);

    exit(0);
}

char *extract_content_parse(int sockfd, char *tag, char *message_num, char *header)
{
    char buffer[BUFFER_SIZE] = {'\0'};
    char end_message_ok[BUFFER_SIZE];
    // char temp[BUFFER_SIZE] = {'\0'};
    char content[BUFFER_SIZE] = {'\0'};
    int is_body = 0, body_end = 0;
    int content_present = 0;
    int curr_len = 0;
    char *to_return;

    modify_tag(tag);

    if (message_num != NULL)
    {
        sprintf(buffer, "%s FETCH %s BODY.PEEK[HEADER.FIELDS (%s)]\r\n", tag, message_num, header);
    }
    else
    { // get the last recent message
        sprintf(buffer, "%s FETCH * BODY.PEEK[HEADER.FIELDS (%s)]\r\n", tag, header);
    }

    write(sockfd, buffer, strlen(buffer));

    FILE *file = fdopen(sockfd, "r");
    assert(file);

    memset(buffer, 0, BUFFER_SIZE); // resetting the buffer

    sprintf(end_message_ok, "%s OK Fetch completed", tag);

    // receive response
    while ((fgets(buffer, BUFFER_SIZE, file) != NULL) && body_end != 1)
    {

        // printf("\n%d*************************************************\n", (int)strlen(buffer));
        // printf("%s", buffer);

        if (strstr(buffer, "FETCH (BODY["))
        {
            is_body = 1;
            continue;
        }
        else if (buffer[0] == '*' && is_body == 0)
        {
            continue;
        }
        else if (strcmp(buffer, ")\r\n") == 0 && is_body == 1)
        {
            // strcpy(line, buffer);
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, file);

            if (strstr(buffer, end_message_ok) != NULL)
            {
                is_body = 0;
                body_end = 1;
                // remove CR
                // content[--curr_len] = '\0';
                break;
            }
            /*else
            {
                // unfolding the very last line
                curr_len = strlen(content);
                content[--curr_len] = '\0';
                content[--curr_len] = '\0';
                // remove CR
                // content[--curr_len] = '\0';

                strcat(content, line);
            }*/
        }
        else if (strstr(buffer, "BAD Error"))
        {
            printf("Message not found\n");
            exit(3);
        }

        if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 0)
        { // reading the first line

            remove_header(buffer, header);
            content_present = 1;
            strcat(content, buffer);

            curr_len = strlen(content);
            content[--curr_len] = '\0'; // removing LF
            content[--curr_len] = '\0'; // removing CR
        }
        else if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 1)
        { // reading the folded lines, and unfolding them

            // curr_len = strlen(content);
            // content[--curr_len] = '\0';
            // content[--curr_len] = '\0';
            //  remove CR
            //  content[--curr_len] = '\0';

            strcat(content, buffer);

            curr_len = strlen(content);
            content[--curr_len] = '\0'; // removing LF
            content[--curr_len] = '\0'; // removing CR
        }

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }

    if (content_present == 1)
    {
        // curr_len = strlen(content);
        // content[curr_len - 2] = '\0'; // removing CR
        to_return = strdup(content);
    }
    else
    {
        if (strcmp(header, "Subject") == 0)
        {
            to_return = strdup("<No subject>");
        }
        else if (strcmp(header, "To") == 0)
        {
            to_return = NULL;
        }
    }

    return to_return;
}

void remove_header(char message[BUFFER_SIZE], char *header)
{
    char temp[1024] = {'\0'};

    strcpy(temp, message + strlen(header) + 2); // +2 because of a colon and space after the header
    memset(message, 0, BUFFER_SIZE);
    strcpy(message, temp);
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