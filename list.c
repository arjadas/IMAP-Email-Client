#include "list.h"
#include "helperfunctions.h"

void list(int sockfd, char *tag, char *folder_name)
{
    // char buffer[BUFFER_SIZE];
    // int bytes_received = 1;

    char *message;

    /*sprintf(buffer, "%s fetch 1:* (BODY[HEADER.FIELDS (Subject)]) \r\n", tag);
    write(sockfd, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    // receive response
    while (bytes_received > 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
        printf("%d\n", bytes_received);
        printf("%s", buffer);
    }*/

    message = extract_content_list(sockfd, tag, "1:*", "Subject");
    printf("%s", message);
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

        // printf("\n%d*************************************************\n", (int)strlen(buffer));
        // printf("%s", buffer);

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
            else
            {
                // new subject area, hence resetting content present fot that area
                content_present = 0;
                continue;
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

            // adding message number
            sprintf(temp, "%d: ", ++message_count);
            strcat(content, temp);
            memset(temp, 0, BUFFER_SIZE);

            strcat(content, buffer);
        }
        else if ((is_body == 1) && is_alphanumeric(buffer) != 0 && content_present == 1)
        { // reading the folded lines, and unfolding them

            curr_len = strlen(content);
            content[--curr_len] = '\0';
            content[--curr_len] = '\0';
            //  remove CR
            //  content[--curr_len] = '\0';

            strcat(content, buffer);
        }

        if (is_alphanumeric(buffer) == 0 && subject_read == 1)
            continue;

        if (content_present == 1)
        {
            // curr_len = strlen(content);
            // content[curr_len - 1] = '\0'; // removing LF
            // content[curr_len - 2] = '\0'; // removing CR
            // to_return = strdup(content);
        }
        else if (content_present == 0 && subject_read == 0)
        {
            if (strcmp(header, "Subject") == 0)
            {
                // adding message number
                sprintf(temp, "%d: ", ++message_count);
                strcat(content, temp);
                memset(temp, 0, BUFFER_SIZE);

                strcat(content, "<No subject>\n");
                // to_return = strdup("<No subject>");
            }
        }

        memset(buffer, 0, BUFFER_SIZE); // resetting the buffer
    }

    to_return = strdup(content);
    return to_return;
}