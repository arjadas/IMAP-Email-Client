#include "list.h"
#include "helperfunctions.h"

void list(int sockfd, char *tag, char *folder_name)
{
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;
    /* new comms new tag */
    modify_tag(tag);

    sprintf(buffer, "%s fetch 1:* (BODY[HEADER.FIELDS (Subject)]) \r\n", tag);
    write(sockfd, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    // receive response
    while (bytes_received > 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
        printf("%d\n", bytes_received);
        printf("%s", buffer);
    }
}