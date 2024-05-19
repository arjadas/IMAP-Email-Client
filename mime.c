#include "mime.h"
/*    
    ("{} FETCH {} BODY.PEEK[HEADER.FIELDS (Content-Type)]\r\n", cur_tag, messagenum);
*/
void mime(int sockfd, char *tag, int message_num, char *folder_name){
    /*
        function to decode mime messages:
            the goal here is to decode MIME messages. the general idea is to 
    */
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int bytes_received = 1;
    char *line;

    modify_tag(tag);
    // HEADER <field-name> <string>,,, MIME-Version: 1.0
    sprintf(buffer, "%s FETCH %d BODY.PEEK[HEADER.FIELDS (MIME-Version)]\r\n", tag, message_num);
    printf("%s\n", buffer);
    // if (message_num != MESSAGE_NOT_GIVEN)
    // {
    //     sprintf(buffer, "%s FETCH 1 BODY.PEEK[HEADER.FEILDS (DATE)]\r\n", tag);
    //     printf("%s\n", buffer);
    // }
    // else
    // { 
    //     // get the last recent message
    //     sprintf(buffer, "%s FETCH 1 BODY.PEEK[HEADER.FEILDS (DATE)]\r\n", tag);
    //     printf("%s\n", buffer);
    // }
    write(sockfd, buffer, strlen(buffer));
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    printf("%s\n", buffer);
    while (bytes_received > 0)
    {
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
        // // printf("%d\n", bytes_received);

        // line = strtok(buffer, "\n");

        printf("%s\n", buffer);
    }
    printf("GOT TO THE END OF MIME\n");
    exit(0);
}