#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"


//dragon compressed
const unsigned char dragon_compressed[] = {
    ' ', 72, '@', 1, '%', 4, ' ', 23, '\n', 1,
    ' ', 69, '%', 6, ' ', 25, '\n', 1,
    ' ', 68, '%', 6, ' ', 26, '\n', 1,
    ' ', 65, '%', 1, ' ', 1, '%', 7, ' ', 11, '@', 1, ' ', 14, '\n', 1,
    ' ', 64, '%', 10, ' ', 8, '%', 7, ' ', 11, '\n', 1,
    ' ', 39, '%', 7, ' ', 2, '%', 4, '@', 1, ' ', 9, '%', 12, '@', 1, ' ', 4, '%', 6, ' ', 2, '@', 1, '%', 4, ' ', 8, '\n', 1,
    ' ', 34, '%', 22, ' ', 6, '%', 28, ' ', 10, '\n', 1,
    ' ', 32, '%', 26, ' ', 3, '%', 12, ' ', 1, '%', 15, ' ', 11, '\n', 1,
    ' ', 31, '%', 29, ' ', 1, '%', 19, ' ', 5, '%', 3, ' ', 12, '\n', 1,
    ' ', 29, '%', 28, '@', 1, ' ', 1, '@', 1, '%', 18, ' ', 8, '%', 2, ' ', 12, '\n', 1,
    ' ', 28, '%', 33, ' ', 1, '%', 22, ' ', 16, '\n', 1,
    ' ', 28, '%', 58, ' ', 14, '\n', 1,
    ' ', 28, '%', 50, '@', 1, '%', 6, '@', 1, ' ', 14, '\n', 1,
    ' ', 6, '%', 8, '@', 1, ' ', 11, '%', 16, ' ', 8, '%', 26, ' ', 6, '%', 2, ' ', 16, '\n', 1,
    ' ', 4, '%', 13, ' ', 9, '%', 2, '@', 1, '%', 12, ' ', 11, '%', 11, ' ', 1, '%', 12, ' ', 6, '@', 1, '%', 1, ' ', 16, '\n', 1,
    ' ', 2, '%', 10, ' ', 3, '%', 3, ' ', 8, '%', 14, ' ', 12, '%', 24, ' ', 24, '\n', 1,
    ' ', 1, '%', 9, ' ', 7, '%', 1, ' ', 9, '%', 13, ' ', 13, '%', 12, '@', 1, '%', 11, ' ', 23, '\n', 1,
    '%', 9, '@', 1, ' ', 16, '%', 1, ' ', 1, '%', 13, ' ', 12, '@', 1, '%', 25, ' ', 21, '\n', 1,
    '%', 8, '@', 1, ' ', 17, '%', 2, '@', 1, '%', 12, ' ', 12, '@', 1, '%', 28, ' ', 18, '\n', 1,
    '%', 7, '@', 1, ' ', 19, '%', 15, ' ', 11, '%', 33, ' ', 14, '\n', 1,
    '%', 10, ' ', 18, '%', 15, ' ', 10, '%', 35, ' ', 6, '%', 4, ' ', 2, '\n', 1,
    '%', 9, '@', 1, ' ', 19, '@', 1, '%', 14, ' ', 9, '%', 12, '@', 1, ' ', 1, '%', 4, ' ', 1, '%', 17, ' ', 3, '%', 8, '\n', 1,
    '%', 10, ' ', 18, '%', 17, ' ', 8, '%', 13, ' ', 6, '%', 18, ' ', 1, '%', 9, '\n', 1,
    '%', 9, '@', 1, '%', 2, '@', 1, ' ', 16, '%', 16, '@', 1, ' ', 7, '%', 14, ' ', 5, '%', 24, ' ', 2, '%', 2, '\n', 1,
    ' ', 1, '%', 10, ' ', 18, '%', 1, ' ', 1, '%', 14, '@', 1, ' ', 8, '%', 14, ' ', 3, '%', 26, ' ', 1, '%', 2, '\n', 1,
    ' ', 2, '%', 12, ' ', 2, '@', 1, ' ', 11, '%', 18, ' ', 8, '%', 40, ' ', 2, '%', 3, ' ', 1, '\n', 1,
    ' ', 3, '%', 13, ' ', 1, '%', 2, ' ', 2, '%', 1, ' ', 2, '%', 1, '@', 1, ' ', 1, '%', 18, ' ', 10, '%', 37, ' ', 4, '%', 3, ' ', 1, '\n', 1,
    ' ', 4, '%', 18, ' ', 1, '%', 22, ' ', 11, '@', 1, '%', 31, ' ', 4, '%', 7, ' ', 1, '\n', 1,
    ' ', 5, '%', 39, ' ', 14, '%', 28, ' ', 8, '%', 3, ' ', 3, '\n', 1,
    ' ', 6, '@', 1, '%', 35, ' ', 18, '%', 25, ' ', 15, '\n', 1,
    ' ', 8, '%', 32, ' ', 22, '%', 19, ' ', 2, '%', 7, ' ', 10, '\n', 1,
    ' ', 11, '%', 26, ' ', 27, '%', 15, ' ', 2, '@', 1, '%', 9, ' ', 9, '\n', 1,
    ' ', 14, '%', 20, ' ', 11, '@', 1, '%', 1, '@', 1, '%', 1, ' ', 18, '@', 1, '%', 18, ' ', 3, '%', 3, ' ', 8, '\n', 1,
    ' ', 18, '%', 15, ' ', 8, '%', 10, ' ', 20, '%', 15, ' ', 4, '%', 1, ' ', 9, '\n', 1,
    ' ', 16, '%', 36, ' ', 22, '%', 14, ' ', 12, '\n', 1,
    ' ', 16, '%', 26, ' ', 2, '%', 4, ' ', 1, '%', 3, ' ', 22, '%', 10, ' ', 2, '%', 3, '@', 1, ' ', 10, '\n', 1,
    ' ', 21, '%', 19, ' ', 1, '%', 6, ' ', 1, '%', 2, ' ', 26, '%', 13, '@', 1, ' ', 10, '\n', 1,
    ' ', 81, '%', 7, '@', 1, ' ', 7, '\n', 1
};


unsigned int dragon_len = sizeof(dragon_compressed) / sizeof(dragon_compressed[0]);


/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

void printDragon() {
    for (size_t i = 0; i < dragon_len; ) {
        char character = dragon_compressed[i++]; // Get the character
        int count = dragon_compressed[i++];     // Get the count

        // Print the character 'count' times
        for (int j = 0; j < count; j++) {
            putchar(character);
        }
    }
}



void trimWhiteSpace(char* str) {
    char *start = str;
    char *end;

    while (*start && isspace((unsigned char)*start)) {
        start++;
    }


    if (*start == '\0') {
        *str = '\0';
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    trimWhiteSpace(cmd_line);
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    char *token = strtok(cmd_line, PIPE_STRING);
    while (token != NULL) {
        if (clist->num >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        trimWhiteSpace(token);
        if (strlen(token) == 0) {
            token = strtok(NULL, PIPE_STRING);
            continue;
        }

        char *space_ptr = strchr(token, SPACE_CHAR);
        if (space_ptr != NULL) {
            int exe_len = space_ptr - token;
            if (exe_len >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strncpy(clist->commands[clist->num].exe, token, exe_len);
            clist->commands[clist->num].exe[exe_len] = '\0';

            char args[ARG_MAX];
            strncpy(args, space_ptr + 1, ARG_MAX - 1);
            args[ARG_MAX - 1] = '\0';
            trimWhiteSpace(args);
            if (strlen(args) >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(clist->commands[clist->num].args, args);
        } else {
            if (strlen(token) >= EXE_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strcpy(clist->commands[clist->num].exe, token);
            clist->commands[clist->num].args[0] = '\0';
        }

        clist->num++;
        token = strtok(NULL, PIPE_STRING);
    }

    return OK;
}

