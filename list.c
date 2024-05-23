#include "list.h"
#include "helperfunctions.h"

void list(int sockfd, char *tag)
{
    char *message;

    message = extract_content_list(sockfd, tag, "1:*", "Subject");
    printf("%s", message);

    free(message);
}

char *extract_content_list(int sockfd, char *tag, char *message_num, char *header)
{
    char buffer[BUFFER_SIZE] = {'\0'};
    char end_message_ok[BUFFER_SIZE];
    char temp[BUFFER_SIZE] = {'\0'};
    char content[BUFFER_SIZE] = {'\0'};
    int is_body = 0, body_end = 0;
    int content_present = 0;
    int curr_len = 0;
    char *to_return;
    int message_count = 0;
    int subject_read = 0;

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
        if (strstr(buffer, "FETCH (BODY["))
        {
            is_body = 1;
            continue;
        }
        else if (buffer[0] == '*')
        {
            continue;
        }
        else if (strcmp(buffer, ")\r\n") == 0 && is_body == 1)
        {
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
                // new subject area, hence resetting content present and subject read for that area
                content_present = 0;
                subject_read = 0;
                continue;
            }
        }

        if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 0)
        { // reading the first line

            remove_header(buffer, header);
            subject_read = 1;
            content_present = 1;

            // adding message number
            sprintf(temp, "%d: ", ++message_count);
            strcat(content, temp);
            memset(temp, 0, BUFFER_SIZE);

            strcat(content, buffer);

            curr_len = strlen(content);
            content[--curr_len] = '\0'; // removing LF
            content[--curr_len] = '\n'; // replacing CR with LF
        }
        else if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 1)
        { // reading the folded lines, and unfolding them

            curr_len = strlen(content);
            content[--curr_len] = '\0'; // replacing LF with null byte

            strcat(content, buffer);

            curr_len = strlen(content);
            content[--curr_len] = '\0'; // removing LF
            content[--curr_len] = '\n'; // replacing CR with LF
        }

        if (is_alphanumeric(buffer) == 0 && subject_read == 1)
            continue;

        if (content_present == 0 && subject_read == 0)
        {
            if (strcmp(header, "Subject") == 0)
            {
                // adding message number
                sprintf(temp, "%d: ", ++message_count);
                strcat(content, temp);
                memset(temp, 0, BUFFER_SIZE);

                strcat(content, "<No subject>\n");
            }
        }

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }

    fclose(file); // closing the file after finishing reading

    to_return = strdup(content);
    return to_return;
}