#include "helperfunctions.h"

// read in function

inputs_t *process_args(int argc, char **argv)
{
    /*
        Structure of program run statement:
        fetchmail-u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t]<command> <server_name>
        Where <command> may be one of: retrieve, parse, mime, or list.
    */

    /* make an inputs_t struct */
    inputs_t *inputs = (inputs_t *)malloc(sizeof(inputs_t));
    assert(inputs);
    assert(inputs != NULL);

    /* initialise values */
    inputs->command = NULL;
    inputs->folder = NULL;
    inputs->message_num = NULL;
    inputs->password = NULL;
    inputs->server_name = NULL;
    inputs->username = NULL;

    /* set input to command so we know what we're up to */
    enum Input input = COMMAND;

    /* now we add each element into the struct from argv */
    int i = 1;
    while (i < argc)
    {
        if (!(strcmp(argv[i], "-u")))
        {
            /* we have username next */
            assert((i + 1) < argc);
            i++;
            inputs->username = strdup(argv[i]);
            assert(inputs->username != NULL);
        }
        else if (!(strcmp(argv[i], "-f")))
        {
            /* we have the folder next */
            assert((i + 1) < argc);
            i++;
            inputs->folder = strdup(argv[i]);
            assert(inputs->folder != NULL);
        }
        else if (!(strcmp(argv[i], "-p")))
        {
            /* we have password next */
            assert((i + 1) < argc);
            i++;
            inputs->password = strdup(argv[i]);
            assert(inputs->password != NULL);
        }
        else if (!(strcmp(argv[i], "-n")))
        {
            /* we have the message number next */
            assert((i + 1) < argc);
            i++;
            inputs->message_num = strdup(argv[i]);
        }
        else if (input == COMMAND)
        {
            inputs->command = strdup(argv[i]);
            assert(inputs->command);
            input++; // update since we have this value now
        }
        else if (input == SERVER_NAME)
        {
            inputs->server_name = strdup(argv[i]);
            assert(inputs->server_name);
            input++; // update since we have this value now
        }
        /* always increase regardless */
        i++;
    }
    /* check if a filename was provided */
    if (inputs->folder == NULL)
    {
        /* set to INBOX */
        char inbox[] = "INBOX";
        inputs->folder = (char *)malloc(sizeof(char) * (INBOX_LEN + 1));
        assert(inputs->folder);
        for (int i = 0; i < INBOX_LEN; i++)
        {
            inputs->folder[i] = inbox[i];
        }
        inputs->folder[i++] = '\0';
    }

    return inputs;
}

int validate_inputs(char *input)
{
    /* checks if `input` is an injection */
}

void print_inputs(inputs_t *inputs)
{
    /* function to print out the values of the command line arguments */
    if (inputs->username != NULL)
        printf("Username: %s, ", inputs->username);

    if (inputs->password != NULL)
        printf("Password: %s, ", inputs->password);

    if (inputs->folder != NULL)
        printf("Folder: %s, ", inputs->folder);

    if (inputs->server_name != NULL)
        printf("Server: %s, ", inputs->server_name);

    if (inputs->command != NULL)
        printf("Command: %s, ", inputs->command);

    if ((inputs->message_num) != NULL)
        printf("Number of messages: %s\n", inputs->message_num);
}

char *generate_tag()
{
    char *tag = (char *)malloc(TAG_SIZE * sizeof(char));
    return tag;
}

void modify_tag(char *tag)
{
    static int count = 0;
    static char prefix = 'A';

    sprintf(tag, "%c%03d", prefix, count++);

    if (count == 101)
    {
        count = 0;
        prefix++;
    }
}

char *get_fetch_line(int sockfd, char *completed_message)
{
    /*
        reads in response from server until `completed_message` is found
    */
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    char *output = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    memset(output, 0, BUFFER_SIZE);
    assert(output);

    int fetch_incomplete = FALSE;
    int bytes_received = 0;

    /* get initial read and set null byte */
    bytes_received = read(sockfd, output, BUFFER_SIZE - 1);
    output[bytes_received] = '\0';

    /* check if we're at the end of the response */
    if (strstr(output, completed_message) != NULL)
        fetch_incomplete = TRUE;

    /* get the rest of the response until `OK Fetch completed` found */
    while (fetch_incomplete == FALSE)
    {
        /* append to buffer */
        bytes_received = read(sockfd, buffer, BUFFER_SIZE - 1);
        buffer[bytes_received] = '\0';

        /* add to output string */
        output = add_buffer_to_output_string(output, buffer, bytes_received);

        /* check if we're at the end of the response */
        if (strstr(output, completed_message) != NULL)
        {
            fetch_incomplete = TRUE;
            break;
        }

        /* reset buffer */
        memset(buffer, 0, BUFFER_SIZE);
    }
    return output;
}

char *add_buffer_to_output_string(char *output, char *buffer, int bytes_received)
{
    /*
        add buffer to the end of output
    */
    int new_size = strlen(output);
    new_size = sizeof(char) * (strlen(output) + bytes_received + 1);
    output = (char *)realloc(output, new_size);
    assert(output);
    strcat(output, buffer);
    assert(output);
    return output;
}