#include "mime.h"
/*    
    ("{} FETCH {} BODY.PEEK[HEADER.FIELDS (Content-Type)]\r\n", cur_tag, messagenum);
*/
void mime(int sockfd, char *tag, int message_num, char *folder_name){
    /*
        function to decode mime messages:
            the goal here is to decode MIME messages. the general idea is as follows:
            1. FETCH MIME-Version and Content-Type headers so we can check for a match
            2a. If matched: Findcharset=UTF-8 block
            2b. If not matched: print error message and exit with status 4
    */
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;
    char fetch_completed[] = "OK Fetch completed";
    int MIME_Version_found = FALSE;
    int Content_Type_found = FALSE;
    int body_part = FALSE;

    /* step 1a: check for MIME-Version */
    if ( !(MIME_Version_found = match_mime_version(sockfd, tag, message_num, folder_name)) ) return;

    /* step 1b: check for Content-Type */
    if ( !(Content_Type_found = match_content_type(sockfd, tag, message_num, folder_name)) ) return;

    /* step 2: get BODYSTRUCUTRE */
    if ( !(body_part = match_type_subtype(sockfd, tag, message_num, folder_name)) ) return;

    /* step 3: extract part NUMBER from server */
    printf("STEP 4\n");
    get_body_part(sockfd, tag, message_num, body_part, folder_name);

    printf("GOT TO THE END OF MIME\n");
    exit(0);
}

int match_mime_version(int sockfd, char *tag, int message_num, char *folder_name)
{
    /*
        check for MIME-Version: 1.0 matching
    */
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;
    char fetch_completed[] = "OK Fetch completed";
    char MIME_Version[] = "MIME-Version: 1.0";
    int MIME_Version_found = FALSE;
    char *match_location = NULL;

    /* new comms new tag */
    modify_tag(tag);

    /* fetch MIME-Version from server for email `message_num` */
    sprintf(buffer, "%s fetch %d (BODY[HEADER.FIELDS (MIME-Version)])\r\n", tag, message_num);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    
    /* read initial response: POTENTIAL BUG BUFFER NOT BIG ENOUGH */
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    while (bytes_received > 0)
    {
        /* output readin for testing */
        if (bytes_received > 0)
        {
            printf("%s, bytes_received = %d\n", buffer, bytes_received);
            
        }
        /* check for a match */
        if (((match_location = strstr(buffer, MIME_Version)) != NULL) && (MIME_Version_found == FALSE))
        {
            MIME_Version_found = TRUE; printf("MIME FOUND: %s\n", buffer);
        }
        /* once we get the `fetch completed` message from the server we can stop */
        if (strstr(buffer, fetch_completed) != NULL)
        {
            printf("MIME-Version BREAK\n\n");
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    }
    if (MIME_Version_found == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

int match_content_type(int sockfd, char *tag, int message_num, char *folder_name)
{
    /*
        check for Content-Type matching,, NEED TO CHECK FOR CASE INSENSITIVITY HERE??
    */
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;
    char fetch_completed[] = "OK Fetch completed";
    char Content_Type[] = "Content-Type: multipart/alternative";
    int Content_Type_found = FALSE;
    char *match_location = NULL;

    /* new comms new tag */
    modify_tag(tag);

    /* fetch Content-Type from server for email `message_num` */
    sprintf(buffer, "%s fetch %d (BODY[HEADER.FIELDS (Content-Type)])\r\n", tag, message_num);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    
    /* read initial response: POTENTIAL BUG BUFFER NOT BIG ENOUGH */
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    while (bytes_received > 0)
    {
        /* output readin for testing */
        if (bytes_received > 0)
        {
            printf("%s, bytes_received = %d\n", buffer, bytes_received);
            
        }
        /* check for a match */
        if (((match_location = strstr(buffer, Content_Type)) != NULL) && (Content_Type_found == FALSE))
        {
            Content_Type_found = TRUE; printf("CONTENT FOUND: %s\n", buffer);
        }
        /* once we get the `fetch completed` message from the server we can stop */
        if (strstr(buffer, fetch_completed) != NULL)
        {
            printf("Content-Type BREAK\n\n");
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    }
    if (Content_Type_found == TRUE)
    {
        return TRUE;
    }
    return FALSE;
}

int match_type_subtype(int sockfd, char *tag, int message_num, char *folder_name)
{
    /*
        finds the BODYSTRUCTURE so that we can look for the BODY PART number of Content-type:text/plain 
    */
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;
    char fetch_completed[] = "OK Fetch completed";
    int body_part;

    /* new comms new tag */
    modify_tag(tag);

    /* request BODYSTRUCTURE from server */
    sprintf(buffer, "%s fetch %d (BODYSTRUCTURE)\r\n", tag, message_num);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    while (bytes_received > 0)
    {
        /* debuggin print */
        if (bytes_received > 0)
        {
            printf("%s, bytes_received = %d\n", buffer, bytes_received);
            
        }
        /* search for stuff that we need */

        /* stop when server is done */
        if (strstr(buffer, fetch_completed) != NULL)
        {
            printf("BREAK\n\n");
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    }
    body_part = 1;
    return body_part;
}

int get_body_part(int sockfd, char *tag, int message_num, int body_part, char *folder_name)
{
    /* 
        dfds
    */
    char buffer[BUFFER_SIZE];
    int bytes_received = 1;
    char fetch_completed[] = "OK Fetch completed";

    /* new comms new tag */
    modify_tag(tag);

    /* request BODY_PART `body_part` from server */
    sprintf(buffer, "%s fetch %d (BODY.PEEK[%d])\r\n", tag, message_num, body_part);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    while (bytes_received > 0)
    {
        /* debuggin print */
        if (bytes_received > 0)
        {
            printf("%s", buffer);
            
        }
        /* stop when server is done */
        if (strstr(buffer, fetch_completed) != NULL)
        {
            printf("BREAK\n\n");
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = read(sockfd, buffer, BUFFER_SIZE);
    }
    return TRUE;
}