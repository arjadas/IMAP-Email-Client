#include "list.h"
#include "helperfunctions.h"

void list(int sockfd, char *tag, char *folder_name)
{
    char buffer[BUFFER_SIZE];

    int bytes_received = 1;

    /* new comms new tag */
    modify_tag(tag);

    /* fetch MIME-Version from server for email `message_num` */
    sprintf(buffer, "%s fetch 1:* (BODY[HEADER.FIELDS (Subject)]) \r\n", tag);
    write(sockfd, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    /*
    buffer[BUFFER_SIZE - 1] = '\0';
    char *server_reply = get_fetch_line(sockfd, FETCH_COMPLETED);
    assert(server_reply);

    // output the subject headers
    char *index = server_reply;
    char *temp = server_reply;
    char *output = NULL;
    int message_num = 0;
    while ((*index) != NULL_BYTE)
    {
        index = strstr(temp, "{");
        if (index != NULL) message_num++;
        if (index == NULL) break;
        output = extract_message(index);
        printf("%d: %s\n", message_num, output);
        free(output);
        index++;
        temp = index;
    }
    */

    // receive response
    while (bytes_received > 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
        printf("%d\n", bytes_received);
        printf("%s", buffer);
    }
}

char *extract_message(char *input)
{
    /* find the number of bytes (characters) to print */
    char *end_brace = input;
    while (((*end_brace) != '}') && (*end_brace) != NULL_BYTE)
    {
        end_brace++;
    }
    input++;
    size_t num_digits = end_brace - input;
    char num_bytes_array[num_digits + 1];
    assert(num_bytes_array != NULL);
    memcpy(num_bytes_array, input, num_digits);
    num_bytes_array[num_digits] = '\0';
    int length = atoi(num_bytes_array);

    /* find the first `Subject` */
    char *subject = strstr(input, "Subject");
    char *output = NULL;
    if (subject != NULL)
    {
        /* find the content we want to add */
        while ((*subject) != ' ')
        {
            subject++;
        }
        subject++;

        /* copy into the new array */
        output = (char *)malloc(length + 1);
        int i = 0;
        for (i = 0; subject[i] != ')'; i++)
        {
            /* fill array ADD UNFORLDING LOGIC HERE */
            output[i] = subject[i];
        }
        output[i + 1] = ')';
        output[i + 2] = NULL_BYTE;
    }
    else
    {
        output = strdup("<No subject>");
    }
    return output;
}