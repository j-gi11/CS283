#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;

    rc = alloc_cmd_buff(&cmd);
    if(rc == ERR_MEMORY) return ERR_MEMORY;
    rc = 0;


    // TODO IMPLEMENT MAIN LOOP
    while(1) {
        cmd_buff = malloc(SH_CMD_MAX);
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff
        clear_cmd_buff(&cmd);
        rc = build_cmd_buff(cmd_buff, &cmd);
        free(cmd_buff);
        cmd_buff = NULL;

        switch(rc) {
            case WARN_NO_CMDS:
                continue;
            case ERR_MEMORY:
                return ERR_MEMORY;
            case ERR_CMD_OR_ARGS_TOO_BIG:
                printf("Command to big\n");
                continue;;
            case ERR_CMD_ARGS_BAD:
                printf("Bad command\n");
                continue;
            default:
                break;
        }
        // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
        // the cd command should chdir to the provided directory; if no directory is provided, do nothing
        Built_In_Cmds BI_return = match_command(cmd.argv[0]);
        if(BI_return == BI_CMD_EXIT) {
            free_cmd_buff(&cmd);
            return OK;
        }

        if(BI_return != BI_NOT_BI) {
            BI_return = exec_built_in_cmd(&cmd, BI_return);
            continue;
        }
        // TODO IMPLEMENT if not built-in command, fork/exec as an external command
        // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        rc = exec_cmd(&cmd);
        
    }
    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff == NULL) return ERR_MEMORY;

    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        if(i == 0) {
            cmd_buff->argv[i] = malloc(CMD_MAX);
        } else {
            cmd_buff->argv[i] = malloc(ARG_MAX);
        }

        if (cmd_buff->argv[i] == NULL) return ERR_MEMORY;
    }

    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if(cmd_buff->_cmd_buffer == NULL) return ERR_MEMORY;

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        free(cmd_buff->argv[i]);
    }
    free(cmd_buff->_cmd_buffer);
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i][0] = '\0';
    }
    cmd_buff->_cmd_buffer[0] = '\0';
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    trimWhiteSpace(cmd_line);
    
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    
    char *current = cmd_line;
    int in_quotes = 0;
    char *arg_start = current;
    size_t arg_len = 0;
    
    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';
    
    while (*current != '\0') {
        if (*current == '"' || *current == '\'') {
            if (!in_quotes) {
                arg_start = current + 1;
            }
            in_quotes = !in_quotes;
            current++;
            continue;
        }
        
        if (!in_quotes && isspace((unsigned char)*current)) {
            if (current > arg_start) {
                if (cmd_buff->argc >= CMD_ARGV_MAX) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                
                arg_len = current - arg_start;
                if (*(current - 1) == '"' || *(current - 1) == '\'') {
                    arg_len--;
                }
                
                size_t max_len = (cmd_buff->argc == 0) ? EXE_MAX - 1 : ARG_MAX - 1;
                
                if (arg_len >= max_len) {
                    return ERR_CMD_OR_ARGS_TOO_BIG;
                }
                
                strncpy(cmd_buff->argv[cmd_buff->argc], arg_start, arg_len);
                cmd_buff->argv[cmd_buff->argc][arg_len] = '\0';
                cmd_buff->argc++;
            }
            
            while (!in_quotes && *(current + 1) && isspace((unsigned char)*(current + 1))) {
                current++;
            }
            
            arg_start = current + 1;
        }
        
        current++;
    }
    
    if (current > arg_start) {
        if (cmd_buff->argc >= CMD_ARGV_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        
        arg_len = current - arg_start;
        if (*(current - 1) == '"' || *(current - 1) == '\'') {
            arg_len--;
        }
        
        size_t max_len = (cmd_buff->argc == 0) ? EXE_MAX : ARG_MAX;
        
        if (arg_len >= max_len) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        
        strncpy(cmd_buff->argv[cmd_buff->argc], arg_start, arg_len);
        cmd_buff->argv[cmd_buff->argc][arg_len] = '\0';
        cmd_buff->argc++;
    }
    
    if (cmd_buff->argc == 0) {
        return WARN_NO_CMDS;
    }

    if(in_quotes) return ERR_CMD_ARGS_BAD;

    // for(int i = 0; i < cmd_buff->argc; i++) {
    //     printf("argv[%d]: %s\n", i, cmd_buff->argv[i]);
    // }
    // printf("argc: %d\n", cmd_buff->argc);
    // printf("buff: %s\n", cmd_buff->_cmd_buffer);

    return OK;
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

Built_In_Cmds match_command(const char *input) {
    if(strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    }

    if(strcmp(input, "dragon")== 0) {
        return BI_CMD_DRAGON;
        
    }

    if(strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }

    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd, Built_In_Cmds code) {
    if (code == BI_CMD_DRAGON) {
        print_dragon();
        return BI_EXECUTED;
    }

    if(code == BI_CMD_CD) {
        int rc = chdir(cmd->argv[1]);
        if(rc == 0) return BI_EXECUTED;
    }

    return BI_RC;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid;
    int status;
    char* temp = cmd->argv[cmd->argc];
    cmd->argv[cmd->argc] = NULL;

    pid = fork();
    if (pid < 0) {
        return ERR_EXEC_CMD;
    }
    else if (pid == 0) {
        if (execvp(cmd->argv[0], cmd->argv) < 0) {
            return ERR_EXEC_CMD;
        }
    } else {
        if (wait(&status) < 0) {
            return ERR_EXEC_CMD;
        }
    }
    cmd->argv[cmd->argc] = temp;
    return OK;
}

