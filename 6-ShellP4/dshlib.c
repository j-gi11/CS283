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

//  void printCommandList(command_list_t *clist) {
//     printf(CMD_OK_HEADER, clist->num);
//     for(int i = 0; i < clist->num; i++) {
//         for(int j = 0; j < clist->commands[i].argc; j++) {
//             printf("<%d> %s ", i + 1, clist->commands[i].argv[j]);
//         }
//         printf("\n");
//     }
// }

int exec_local_cmd_loop()
{
    char *cmd_buff = (char*)malloc(SH_CMD_MAX * CMD_MAX);
    int rc = 0;
    command_list_t *clist = (command_list_t*)malloc(sizeof(command_list_t));
    if (clist == NULL) return ERR_MEMORY;

    rc = initialize_command_list(clist);
    if(rc == ERR_MEMORY) return ERR_MEMORY; 
    rc = 0;
    
    while(1) {

        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL){
            printf("\n");
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
        rc = build_cmd_list(cmd_buff, clist);
        switch(rc) {
            case WARN_NO_CMDS:
                //printf(CMD_WARN_NO_CMD);
                continue;
            case ERR_MEMORY:
                return ERR_MEMORY;
            case  ERR_TOO_MANY_COMMANDS:
                printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                continue;
            case ERR_CMD_OR_ARGS_TOO_BIG:
                printf("command to big\n");
                continue;
            case ERR_CMD_ARGS_BAD:
                printf(CMD_ERR_ARGS_BAD);
                continue;
        }
        // printCommandList(clist);

 
        Built_In_Cmds BI_return = match_command(clist->commands[0].argv[0]);
        if(BI_return == BI_CMD_EXIT) {
            free(cmd_buff);
            free_cmd_list(clist);
            return OK;
        }

        if(BI_return != BI_NOT_BI) {
            BI_return = exec_built_in_cmd(&clist->commands[0], BI_return);
            if(BI_return != BI_EXECUTED) {
                printf(CMD_ERR_EXECUTE);
            }
            continue;
        }
        rc = execute_pipeline(clist);
    }

    return OK;
}


int alloc_cmd_buff(cmd_buff_t *cmd_buff) {

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

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    trimWhiteSpace(cmd_line);
    char* tok = strtok(cmd_line, PIPE_STRING);
    int rc = 0;
    clist->num = 0;
    while(tok != NULL) {
        if(clist->num >= CMD_MAX) return ERR_TOO_MANY_COMMANDS;
        
        cmd_buff_t *cmd_buff = &clist->commands[clist->num];
        clear_cmd_buff(cmd_buff);
        rc = build_cmd_buff(tok, cmd_buff);
        if(rc != OK) return rc;

        tok = strtok(NULL, PIPE_STRING);
        clist->num++;
    }
    return OK;
}


int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    trimWhiteSpace(cmd_line);
    if (strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    
    cmd_buff->argc = 0;
    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';
    int in_quote;
    
    char *current = cmd_line;
    while (*current != '\0') {
        while (*current != '\0' && isspace((unsigned char)*current))
            current++;
        if (*current == '\0')
            break;
        
        char *start = current;
        in_quote = 0;
        char quote_char = '\0';
        
        if (*current == '"' || *current == '\'') {
            in_quote = 1;
            quote_char = *current;
            current++;      // Skip the opening quote
            start = current;
            // Consume until we find the matching closing quote
            while (*current != '\0' && *current != quote_char)
                current++;
            // If we reached end-of-string without finding the closing quote, return error
            if (*current == '\0') {
                return ERR_CMD_ARGS_BAD;
            }
        } else {
            while (*current != '\0' && !isspace((unsigned char)*current) &&
                   *current != '"' && *current != '\'')
                current++;
        }
        
        size_t len = current - start;
        size_t max_len = (cmd_buff->argc == 0) ? EXE_MAX - 1 : ARG_MAX - 1;
        if (len >= max_len) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (cmd_buff->argc >= CMD_ARGV_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        
        strncpy(cmd_buff->argv[cmd_buff->argc], start, len);
        cmd_buff->argv[cmd_buff->argc][len] = '\0';
        cmd_buff->argc++;
        
        // If we were processing a quoted token, skip the closing quote.
        if (in_quote && *current == quote_char)
            current++;
    }
    
    if (cmd_buff->argc == 0) {
        return WARN_NO_CMDS;
    }
    
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

int initialize_command_list(command_list_t* clist) {
    if(clist == NULL) return ERR_MEMORY;
    clist->num = 0;
    int rc = 0;

    for(int i = 0; i < CMD_MAX; i++) {
        rc = alloc_cmd_buff(&clist->commands[i]);
        if (rc == ERR_MEMORY) return ERR_MEMORY;
    }

    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for(int i = 0; i < CMD_MAX; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    free(cmd_lst);
    return OK;
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
        if (cmd->argc < 2) return BI_EXECUTED;
        int rc = chdir(cmd->argv[1]);
        if(rc == 0) return BI_EXECUTED;
    }
    return BI_CMD_RC;
}


int execute_pipeline(command_list_t *clist) {
    if (clist == NULL || clist->num == 0) {
        return ERR_EXEC_CMD;
    }
    
    int pipes[CMD_MAX-1][2];
    pid_t pids[CMD_MAX];
    char *original_pointers[CMD_ARGV_MAX];
    
    for (int i = 0; i < clist->num; i++) {
        original_pointers[i] = clist->commands[i].argv[clist->commands[i].argc];
        clist->commands[i].argv[clist->commands[i].argc] = NULL;
    }
    
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            for (int j = 0; j < clist->num; j++) {
                clist->commands[j].argv[clist->commands[j].argc] = original_pointers[j];
            }
            return ERR_EXEC_CMD;
        }
    }
    
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            for (int j = 0; j < clist->num; j++) {
                clist->commands[j].argv[clist->commands[j].argc] = original_pointers[j];
            }
            return ERR_EXEC_CMD;
        } else if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            
            printf(CMD_ERR_EXECUTE);
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    int status;
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &status, 0);
    }
    
    for (int i = 0; i < clist->num; i++) {
        clist->commands[i].argv[clist->commands[i].argc] = original_pointers[i];
    }
    
    return OK;
}
