#include "mime.h"

void mime(int sockfd, char *tag, char *input_message_num, char *folder_name)
{
    /*
        function to decode mime messages:
            the goal here is to decode MIME messages. the general idea is as follows:
            1. FETCH MIME-Version and Content-Type headers so we can check for a match
            2a. If matched: Findcharset=UTF-8 block
            2b. If not matched: print error message and exit with status 4
    */
    /* first extract the values from `message` num */
    for (int i = 0; input_message_num[i] != NULL_BYTE; i++)
    {
        if ( !(isdigit((int)input_message_num[i])) ){
            printf("Invalid input for messageNum. Insert a number instead.\n");
            exit(1);
        }
    }
    int message_num = atoi(input_message_num);

    int body_part = -1;

    /* step 1a: check for MIME-Version */
    if ( !(match_mime_version(sockfd, tag, message_num, folder_name)) ) 
    {
        printf("Header is not match the MIME-Version requirement.\n");
        exit(4);
    }

    /* step 1b: check for Content-Type */
    if ( !(match_content_type(sockfd, tag, message_num, folder_name)) ) 
    {
        printf("Header is not match the Content-Type requirement.\n");
        exit(4);
    }

    /* step 2: get BODYSTRUCUTRE */
    if ( !((body_part = get_body_part(sockfd, tag, message_num, folder_name))) ) 
    {
        printf("Header is either not match the Type/ Subtype or encoding requirement.\n");
        exit(4);
    }

    /* step 3: extract part NUMBER from server */
    print_body_part(sockfd, tag, message_num, body_part, folder_name);

    return;
}

int match_mime_version(int sockfd, char *tag, int message_num, char *folder_name)
{
    /*
        check for MIME-Version: 1.0 matching
    */
    char buffer[BUFFER_SIZE];
    char *compare = NULL;
    char *output = NULL;
    char *lower_output = NULL;
    int is_MIME_Version = FALSE;
    /* new comms new tag */
    modify_tag(tag);

    /* fetch MIME-Version from server for email `message_num` */
    sprintf(buffer, "%s fetch %d (BODY[HEADER.FIELDS (MIME-Version)])\r\n", tag, message_num);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);

    /* read in response from the server */
    output = get_fetch_line(sockfd, FETCH_COMPLETED);
    assert(output);
    
    /* check for MIME-Version */
    lower_output = str_to_lower(output);
    free(output);
    compare = strstr(lower_output, MIME_VERSION);
    if (compare != NULL) is_MIME_Version = TRUE;
    free(lower_output);
    return is_MIME_Version;
}

int match_content_type(int sockfd, char *tag, int message_num, char *folder_name)
{
    /*
        check for Content-Type matching,, NEED TO CHECK FOR CASE INSENSITIVITY HERE??
    */
    char buffer[BUFFER_SIZE];
    char *compare = NULL;
    char *output = NULL;
    char *lower_output = NULL;
    int is_Content_Type = FALSE;

    /* new comms new tag */
    modify_tag(tag);

    /* fetch Content-Type from server for email `message_num` */
    sprintf(buffer, "%s fetch %d (BODY[HEADER.FIELDS (Content-Type)])\r\n", tag, message_num);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    
    /* get response from the server */
    output = get_fetch_line(sockfd, FETCH_COMPLETED);
    assert(output);

    /* check for Content-Type */
    lower_output = NULL;
    lower_output = str_to_lower(output);
    free(output);
    compare = strstr(lower_output, CONTENT_TYPE);

    if (compare != NULL) is_Content_Type = TRUE;
    free(lower_output);
    return is_Content_Type;
}

int get_body_part(int sockfd, char *tag, int message_num, char *folder_name)
{
    /*
        finds the first body part that is of TYPE/SUBTYPE = `text/plain`, 
                                                charset=`UTF-8`, and 
                                                encoding = `quoted-printable | 7bit | 8bit`
        uses BODYSTRUCTURE
    */
    
    char buffer[BUFFER_SIZE];
    int body_part = 0;
    char *start = NULL;
    char *end = NULL;
    char *part = NULL;
    int num_open_brackets = 0;

    /* new comms new tag */
    modify_tag(tag);

    /* request BODYSTRUCTURE from server */
    sprintf(buffer, "%s fetch %d (BODYSTRUCTURE)\r\n", tag, message_num);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    
    /* get response from the server */
    char *output = NULL;
    output = get_fetch_line(sockfd, FETCH_COMPLETED); 

    /* find location of second `(` that comes after `BODYSTRUCTURE` */
    start = strstr(output, "BODYSTRUCTURE");
    assert(start != NULL);
    while ((*start) != '\0')
    {   
        assert(start); 
        if ((*start) == '(') num_open_brackets++;
        if (num_open_brackets == 2) break;
        start++;
    }
    
    /* so now we know where the first part starts and we now check each part to see if it is satisfactory */
    end = start;
    while ((*start) != '\0')
    {
        /* first look for the end bracket for the given part */
        part = find_part(start, end);
        body_part++;

        /* now check for what we need */
        if ( (strstr(part, "text") != NULL) && 
             (strstr(part, "plain") != NULL) && 
             (strstr(part, "utf-8") != NULL) && 
             ( (strstr(part, "quoted-printable") != NULL) || 
               (strstr(part, "7bit") != NULL) || 
               (strstr(part, "8bit") != NULL) ) ) break;
        start = end;
        end++;
    }
    free(part);
    
    free(output);
    return body_part;
}

char *find_part(char *start, char *end)
{
    /*
        find the end of a given part by find the corresponding end bracket and makes it lower case
    */
    assert((*end) == '(');
    int in_bracket = 1;
    while ((in_bracket != 0) && ((*end) != '\0'))
    {
        /* check if we have an open or close bracket */
        end++;
        if ((*end) == '(') in_bracket++;
        if ((*end) == ')') in_bracket--;
    }

    /* extract the part from the string */
    size_t length = end - start + 1;
    char *part = (char *)malloc(length + 1);
    assert(part != NULL);
    memcpy(part, start, length);
    part[length] = '\0';

    /* make part lower case */
    char *lower_part = NULL;
    lower_part = str_to_lower(part);
    assert(lower_part != NULL);
    free(part);

    return lower_part;
}

void print_body_part(int sockfd, char *tag, int message_num, int body_part, char *folder_name)
{
    /* 
        PRINTS OUT PART OF THE MESSAGE
    */
    char buffer[BUFFER_SIZE];    

    /* new comms new tag */
    modify_tag(tag);

    /* request BODY_PART `body_part` from server */
    sprintf(buffer, "%s fetch %d (BODY.PEEK[%d])\r\n", tag, message_num, body_part);
    write(sockfd, buffer, strlen(buffer)); memset(buffer, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);

    /* edit the buffer that we print the right thing */
    char *output = get_fetch_line(sockfd, FETCH_COMPLETED);
    assert(output);
    int length = 0;
    char *print_string = get_output_string(output, &length);

    /* print the print_string */
    int i = 0;
    while (i < length)
    {
        printf("%c", print_string[i]);
        i++;
    }

    free(print_string);
    free(output);
}

char *str_to_lower(char *str)
{
    /*
        makes the input string lower case
    */
   assert(str);

   int i = 0;
   char *output = NULL;
   output = strdup(str);
   assert(output);

   while (str[i] != '\0')
   {
        output[i] = tolower(str[i]);
        i++;
   }
   return output;
}

char *get_output_string(char *original_str, int *length)
{
    /* find the number of bytes (characters) to print */
    char *first_brace = strstr(original_str, "{");
    char *end_brace = first_brace;
    while ((*end_brace) != '}')
    {
        end_brace++;
    }
    first_brace++;
    size_t num_digits = end_brace - first_brace;
    char num_bytes_array[num_digits + 1];
    assert(num_bytes_array != NULL);
    memcpy(num_bytes_array, first_brace, num_digits);
    num_bytes_array[num_digits] = '\0';
    (*length) = atoi(num_bytes_array);

    /* find the first "\n" */
    while ((*end_brace) != '\n')
    {
        end_brace++;
    }
    end_brace++; 

    /* make the output string */
    char *output = (char *)malloc((*length));
    memcpy(output, end_brace, (*length));

    return output;
}


