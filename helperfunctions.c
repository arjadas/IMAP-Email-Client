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
    inputs->message_num = MESSAGE_NOT_GIVEN;
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
            inputs->message_num = atoi(argv[i]);
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
    return inputs;
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

    if ((inputs->message_num) > MESSAGE_NOT_GIVEN)
        printf("Number of messages: %d\n", inputs->message_num);
}