
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>

//INCLUDES for extra credit
//#include <signal.h>
//#include <pthread.h>
//-------------------------

#include "dshlib.h"
#include "rshlib.h"


/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 * 
 *      This function is fully implemented for you and should not require
 *      any changes for basic functionality.  
 * 
 *      IF YOU IMPLEMENT THE MULTI-THREADED SERVER FOR EXTRA CREDIT YOU NEED
 *      TO DO SOMETHING WITH THE is_threaded ARGUMENT HOWEVER.  
 */
int start_server(char *ifaces, int port, int is_threaded){
    int svr_socket;
    int rc;
    is_threaded++; //get rid of warning
    //
    //TODO:  If you are implementing the extra credit, please add logic
    //       to keep track of is_threaded to handle this feature
    //

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0){
        int err_code = svr_socket;  //server socket will carry error code
        printf(CMD_ERR_RDSH_COMM);
        return err_code;
    }

    rc = process_cli_requests(svr_socket);

    stop_server(svr_socket);


    return rc;
}

/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket){
    return close(svr_socket);
}

/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  Specifically: 
 * 
 *      1. Create the server socket using the socket() function. 
 *      2. Calling bind to "bind" the server to the interface and port
 *      3. Calling listen to get the server ready to listen for connections.
 * 
 *      after creating the socket and prior to calling bind you might want to 
 *      include the following code:
 * 
 *      int enable=1;
 *      setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
 * 
 *      when doing development you often run into issues where you hold onto
 *      the port and then need to wait for linux to detect this issue and free
 *      the port up.  The code above tells linux to force allowing this process
 *      to use the specified port making your life a lot easier.
 * 
 *  Returns:
 * 
 *      server_socket:  Sockets are just file descriptors, if this function is
 *                      successful, it returns the server socket descriptor, 
 *                      which is just an integer.
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code is returned if the socket(),
 *                               bind(), or listen() call fails. 
 * 
 */
int boot_server(char *ifaces, int port){
    int svr_socket;
    int ret;
    
    struct sockaddr_in addr;

    // TODO set up the socket - this is very similar to the demo code

    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket == -1) {
        return ERR_RDSH_COMMUNICATION;
    }

    int enable=1;
    setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    
    /* Bind socket to socket name. */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ifaces);
    addr.sin_port = htons(port);

    ret = bind(svr_socket, (const struct sockaddr *) &addr,
               sizeof(struct sockaddr_in));
    if (ret == -1) {
        return ERR_RDSH_COMMUNICATION;
    }


    /*
     * Prepare for accepting connections. The backlog size is set
     * to 20. So while one request is being processed other requests
     * can be waiting.
     */
    ret = listen(svr_socket, 20);
    if (ret == -1) {
        return ERR_RDSH_COMMUNICATION;
    }

    return svr_socket;
}

/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connections.  It does this using
 *  the following logic
 * 
 *      1.  Starts a while(1) loop:
 *  
 *          a. Calls accept() to wait for a client connection. Recall that 
 *             the accept() function returns another socket specifically
 *             bound to a client connection. 
 *          b. Calls exec_client_requests() to handle executing commands
 *             sent by the client. It will use the socket returned from
 *             accept().
 *          c. Loops back to the top (step 2) to accept connecting another
 *             client.  
 * 
 *          note that the exec_client_requests() return code should be
 *          negative if the client requested the server to stop by sending
 *          the `stop-server` command.  If this is the case step 2b breaks
 *          out of the while(1) loop. 
 * 
 *      2.  After we exit the loop, we need to cleanup.  Dont forget to 
 *          free the buffer you allocated in step #1.  Then call stop_server()
 *          to close the server socket. 
 * 
 *  Returns:
 * 
 *      OK_EXIT:  When the client sends the `stop-server` command this function
 *                should return OK_EXIT. 
 * 
 *      ERR_RDSH_COMMUNICATION:  This error code terminates the loop and is
 *                returned from this function in the case of the accept() 
 *                function failing. 
 * 
 *      OTHERS:   See exec_client_requests() for return codes.  Note that positive
 *                values will keep the loop running to accept additional client
 *                connections, and negative values terminate the server. 
 * 
 */
int process_cli_requests(int svr_socket){
    int cli_socket;
    int rc = OK;
    // char* send_buff = (char*) malloc(RDSH_COMM_BUFF_SZ * sizeof(char));
    // char* recv_buff = (char*) malloc(RDSH_COMM_BUFF_SZ * sizeof(char));
    
    while(1){

        // TODO use the accept syscall to create cli_socket 
        printf("Waiting for client connection...\n");
        fflush(stdout);
        cli_socket = accept(svr_socket, NULL, NULL);
        if(cli_socket == -1) {
            perror("accept");
            return ERR_RDSH_COMMUNICATION;
        }

        printf("Client connected\n");
        fflush(stdout);
        // and then exec_client_requests(cli_socket)
        rc = exec_client_requests(cli_socket);

        if(rc < 0) {
            break;
        }

        if((Built_In_Cmds)rc == BI_CMD_EXIT) {
            send_message_eof(cli_socket);
            stop_server(cli_socket);
            continue;
        }

        // memset(send_buff,0,sizeof(send_buff));
        // memset(recv_buff,0,sizeof(recv_buff));
        
        
    }


    stop_server(svr_socket);
    return rc;
}

/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands. The function will
 *  loop and continue to accept and execute client commands.  There are 2 ways
 *  that this ongoing loop accepting client commands ends:
 * 
 *      1.  When the client executes the `exit` command, this function returns
 *          to process_cli_requests() so that we can accept another client
 *          connection. 
 *      2.  When the client executes the `stop-server` command this function
 *          returns to process_cli_requests() with a return code of OK_EXIT
 *          indicating that the server should stop. 
 * 
 *  Note that this function largely follows the implementation of the
 *  exec_local_cmd_loop() function that you implemented in the last 
 *  shell program deliverable. The main difference is that the command will
 *  arrive over the recv() socket call rather than reading a string from the
 *  keyboard. 
 * 
 *  This function also must send the EOF character after a command is
 *  successfully executed to let the client know that the output from the
 *  command it sent is finished.  Use the send_message_eof() to accomplish 
 *  this. 
 * 
 *  Of final note, this function must allocate a buffer for storage to 
 *  store the data received by the client. For example:
 *     io_buff = malloc(RDSH_COMM_BUFF_SZ);
 *  And since it is allocating storage, it must also properly clean it up
 *  prior to exiting.
 * 
 *  Returns:
 * 
 *      OK:       The client sent the `exit` command.  Get ready to connect
 *                another client. 
 *      OK_EXIT:  The client sent `stop-server` command to terminate the server
 * 
 *      ERR_RDSH_COMMUNICATION:  A catch all for any socket() related send
 *                or receive errors. 
 */
int exec_client_requests(int cli_socket) {
    int io_size;
    command_list_t *cmd_list;
    int rc;
    int cmd_rc;
    int last_rc;
    char *io_buff;


    io_buff = malloc(RDSH_COMM_BUFF_SZ);
    cmd_list = (command_list_t*) malloc(sizeof(command_list_t));
    if (io_buff == NULL || cmd_list == NULL) {
        return ERR_RDSH_SERVER;
    }

    rc = initialize_command_list(cmd_list);
    if(rc == ERR_MEMORY) return ERR_RDSH_SERVER;

    while(1) {
        int is_last_chunk;
        memset(io_buff, '.', RDSH_COMM_BUFF_SZ);
        
        // TODO use recv() syscall to get input
        while ((io_size= recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0)) > 0){
            if (io_size <= 0){
                return ERR_RDSH_COMMUNICATION;
            }
        
            //At this point we have some data, lets see if this is the last chunk
            is_last_chunk = ((char)io_buff[io_size-1] == '\0') ? 1 : 0;
            // printf("is last chunk %d; io_size: %d\n", is_last_chunk, io_size);
        
            if (is_last_chunk){
                io_buff[io_size-1] = '\0';
            }
            printf("recieved command: %.*s", (int)io_size, io_buff);
        
            if (is_last_chunk) {
                break;
            }
        }
        // rc = send_message_string(cli_socket, io_buff);
        // if (rc < 0) return ERR_RDSH_COMMUNICATION;
        // rc = send_message_eof(cli_socket);
        // if (rc < 0) return ERR_RDSH_COMMUNICATION;
        

        // TODO build up a cmd_list
        // printf("out of recv\n");
        // io_buff[strcspn(io_buff,"\n")] = '\0';
        char *newline = strchr(io_buff, '\n');
        if (newline) {
            *newline = '\0';
        }
        // printf("after strcspn\n");
        // printf("io_buff: %s\n", io_buff);
        rc = build_cmd_list(io_buff, cmd_list);

        // printf("rc after build: %d", rc);
        // printCommandList(cmd_list);
        fflush(NULL);

        switch(rc) {
            case WARN_NO_CMDS:
                //printf(CMD_WARN_NO_CMD);
                rc = send_message_eof(cli_socket);
                continue;
            case ERR_MEMORY:
                return ERR_RDSH_COMMUNICATION;
            case  ERR_TOO_MANY_COMMANDS:
                snprintf(io_buff, RDSH_COMM_BUFF_SZ, CMD_ERR_PIPE_LIMIT, CMD_MAX);
                send_message_string(cli_socket, io_buff);
                send_message_eof(cli_socket);
                continue;
            case ERR_CMD_OR_ARGS_TOO_BIG:
                snprintf(io_buff, RDSH_COMM_BUFF_SZ, "command to big\n");
                send_message_string(cli_socket, io_buff);
                send_message_eof(cli_socket);
                continue;
            case ERR_CMD_ARGS_BAD:
                snprintf(io_buff, RDSH_COMM_BUFF_SZ, CMD_ERR_ARGS_BAD);
                send_message_string(cli_socket, io_buff);
                send_message_eof(cli_socket);
                continue;
        }

        // TODO rsh_execute_pipeline to run your cmd_list
        Built_In_Cmds is_bi = rsh_match_command(cmd_list->commands[0].argv[0]);
        cmd_rc = rsh_built_in_cmd(&cmd_list->commands[0]);
        // printf("cmd_rc: %d, is_bi: %d", cmd_rc, is_bi);
        fflush(NULL);
        if(cmd_rc == BI_CMD_STOP_SVR) {
            free_cmd_list(cmd_list);
            free(io_buff);
            return OK_EXIT;
        }
        if(cmd_rc == BI_CMD_EXIT && is_bi != BI_NOT_BI) return BI_CMD_EXIT;
        if(cmd_rc == BI_CMD_DRAGON) {
            copy_dragon(io_buff);
            send_message_string(cli_socket, io_buff);
            send_message_eof(cli_socket);
            continue;
        }
        if(cmd_rc == BI_CMD_RC) {
            snprintf(io_buff, RDSH_COMM_BUFF_SZ, "rc: %d\n", last_rc);
            send_message_string(cli_socket, io_buff);
            send_message_eof(cli_socket);
            continue;
        }
        if(cmd_rc == BI_EXECUTED) {
            last_rc = cmd_rc; 
            send_message_eof(cli_socket); 
            continue;
        }
        // printf("after checks");
        fflush(NULL);

        // TODO send appropriate respones with send_message_string
        // - error constants for failures
        // - buffer contents from execute commands
        //  - etc.
        cmd_rc = rsh_execute_pipeline(cli_socket, cmd_list);
        last_rc = cmd_rc;
        // printf("after execute pipeline cmd_rc = %d\n", cmd_rc);

        usleep(100000);  

        // TODO send_message_eof when done
        fflush(NULL);
        send_message_eof(cli_socket);
    }

    return WARN_RDSH_NOT_IMPL;
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client

 *  Sends the EOF character to the client to indicate that the server is
 *  finished executing the command that it sent. 
 * 
 *  Returns:
 * 
 *      OK:  The EOF character was sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the EOF character. 
 */
int send_message_eof(int cli_socket){
    int send_len = (int)sizeof(RDSH_EOF_CHAR);
    int sent_len;
    sent_len = send(cli_socket, &RDSH_EOF_CHAR, send_len, 0);

    if (sent_len != send_len){
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}


/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.  Note this command executes both a send()
 *  to send the message and a send_message_eof() to send the EOF character to
 *  the client to indicate command execution terminated. 
 * 
 *  Returns:
 * 
 *      OK:  The message in buff followed by the EOF character was 
 *           sent successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() socket call returned an error or if
 *           we were unable to send the message followed by the EOF character. 
 */
int send_message_string(int cli_socket, char *buff){
    //TODO implement writing to cli_socket with send()
    int send_len = strlen(buff);
    int sent_len;
    sent_len = send(cli_socket, buff, send_len, 0);

    if (sent_len != send_len){
        return ERR_RDSH_COMMUNICATION;
    }

    return OK;
}


/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.  It should basically be a
 *  replica of the execute_pipeline() function from the last deliverable. 
 *  The only thing different is that you will be using the cli_sock as the
 *  main file descriptor on the first executable in the pipeline for STDIN,
 *  and the cli_sock for the file descriptor for STDOUT, and STDERR for the
 *  last executable in the pipeline.  See picture below:  
 * 
 *      
 *┌───────────┐                                                    ┌───────────┐
 *│ cli_sock  │                                                    │ cli_sock  │
 *└─────┬─────┘                                                    └────▲──▲───┘
 *      │   ┌──────────────┐     ┌──────────────┐     ┌──────────────┐  │  │    
 *      │   │   Process 1  │     │   Process 2  │     │   Process N  │  │  │    
 *      │   │              │     │              │     │              │  │  │    
 *      └───▶stdin   stdout├─┬──▶│stdin   stdout├─┬──▶│stdin   stdout├──┘  │    
 *          │              │ │   │              │ │   │              │     │    
 *          │        stderr├─┘   │        stderr├─┘   │        stderr├─────┘    
 *          └──────────────┘     └──────────────┘     └──────────────┘   
 *                                                      WEXITSTATUS()
 *                                                      of this last
 *                                                      process to get
 *                                                      the return code
 *                                                      for this function       
 * 
 *  Returns:
 * 
 *      EXIT_CODE:  This function returns the exit code of the last command
 *                  executed in the pipeline.  If only one command is executed
 *                  that value is returned.  Remember, use the WEXITSTATUS()
 *                  macro that we discussed during our fork/exec lecture to
 *                  get this value. 
 */

 void printCommandList(command_list_t *clist) {
    printf(CMD_OK_HEADER, clist->num);
    for(int i = 0; i < clist->num; i++) {
        for(int j = 0; j < clist->commands[i].argc; j++) {
            printf("<%d> %s ", i + 1, clist->commands[i].argv[j]);
        }
        printf("\n");
    }
}

int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
    if (clist == NULL || clist->num == 0) {
        return ERR_EXEC_CMD;
    }
    
    int pipes[CMD_MAX-1][2];
    pid_t pids[CMD_MAX];
    char *original_pointers[CMD_MAX];
    int status, last_status = 0;
    
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
    

    fcntl(cli_sock, F_SETFD, FD_CLOEXEC);
    
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            for (int j = 0; j < clist->num - 1; j++) {
                if (j < i) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }
            
            for (int j = 0; j < clist->num; j++) {
                clist->commands[j].argv[clist->commands[j].argc] = original_pointers[j];
            }
            return ERR_EXEC_CMD;
        } 
        else if (pids[i] == 0) {
            for (int fd = 3; fd < 1024; fd++) {
                if (fd != cli_sock && 
                    !(i > 0 && fd == pipes[i-1][0]) && 
                    !(i < clist->num - 1 && fd == pipes[i][1])) {
                    close(fd);
                }
            }
            
            if (i == 0) {
                int null_fd = open("/dev/null", O_RDONLY);
                if (null_fd >= 0) {
                    dup2(null_fd, STDIN_FILENO);
                    close(null_fd);
                }
            } else {
                dup2(pipes[i-1][0], STDIN_FILENO);
                close(pipes[i-1][0]);
            }
            
            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            } else {
                dup2(cli_sock, STDOUT_FILENO);
            }
            
            dup2(cli_sock, STDERR_FILENO);
            
            if (cli_sock > STDERR_FILENO) {
                close(cli_sock);
            }
            
            for (int j = 0; j < clist->num - 1; j++) {
                if (i != j && j < i) close(pipes[j][0]);
                if (i != j+1 && j >= i) close(pipes[j][1]);
            }
            
            setvbuf(stdout, NULL, _IONBF, 0);
            setvbuf(stderr, NULL, _IONBF, 0);
            
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            
            const char *err_msg = strerror(errno);
            dprintf(STDERR_FILENO, "Error executing %s: %s\n", 
                    clist->commands[i].argv[0], err_msg);
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &status, 0);
        if (i == clist->num - 1) {
            last_status = WEXITSTATUS(status);
        }
    }
    
    for (int i = 0; i < clist->num; i++) {
        clist->commands[i].argv[clist->commands[i].argc] = original_pointers[i];
    }
    
    return last_status;
}


/**************   OPTIONAL STUFF  ***************/
/****
 **** NOTE THAT THE FUNCTIONS BELOW ALIGN TO HOW WE CRAFTED THE SOLUTION
 **** TO SEE IF A COMMAND WAS BUILT IN OR NOT.  YOU CAN USE A DIFFERENT
 **** STRATEGY IF YOU WANT.  IF YOU CHOOSE TO DO SO PLEASE REMOVE THESE
 **** FUNCTIONS AND THE PROTOTYPES FROM rshlib.h
 **** 
 */

/*
 * rsh_match_command(const char *input)
 *      cli_socket:  The string command for a built-in command, e.g., dragon,
 *                   cd, exit-server
 *   
 *  This optional function accepts a command string as input and returns
 *  one of the enumerated values from the BuiltInCmds enum as output. For
 *  example:
 * 
 *      Input             Output
 *      exit              BI_CMD_EXIT
 *      dragon            BI_CMD_DRAGON
 * 
 *  This function is entirely optional to implement if you want to handle
 *  processing built-in commands differently in your implementation. 
 * 
 *  Returns:
 * 
 *      BI_CMD_*:   If the command is built-in returns one of the enumeration
 *                  options, for example "cd" returns BI_CMD_CD
 * 
 *      BI_NOT_BI:  If the command is not "built-in" the BI_NOT_BI value is
 *                  returned. 
 */
Built_In_Cmds rsh_match_command(const char *input)
{
    if (strcmp(input, "exit") == 0)
        return BI_CMD_EXIT;
    if (strcmp(input, "dragon") == 0)
        return BI_CMD_DRAGON;
    if (strcmp(input, "cd") == 0)
        return BI_CMD_CD;
    if (strcmp(input, "stop-server") == 0)
        return BI_CMD_STOP_SVR;
    if (strcmp(input, "rc") == 0)
        return BI_CMD_RC;
    return BI_NOT_BI;
}

/*
 * rsh_built_in_cmd(cmd_buff_t *cmd)
 *      cmd:  The cmd_buff_t of the command, remember, this is the 
 *            parsed version fo the command
 *   
 *  This optional function accepts a parsed cmd and then checks to see if
 *  the cmd is built in or not.  It calls rsh_match_command to see if the 
 *  cmd is built in or not.  Note that rsh_match_command returns BI_NOT_BI
 *  if the command is not built in. If the command is built in this function
 *  uses a switch statement to handle execution if appropriate.   
 * 
 *  Again, using this function is entirely optional if you are using a different
 *  strategy to handle built-in commands.  
 * 
 *  Returns:
 * 
 * 
 *      BI_NOT_BI:   Indicates that the cmd provided as input is not built
 *                   in so it should be sent to your fork/exec logic
 *      BI_EXECUTED: Indicates that this function handled the direct execution
 *                   of the command and there is nothing else to do, consider
 *                   it executed.  For example the cmd of "cd" gets the value of
 *                   BI_CMD_CD from rsh_match_command().  It then makes the libc
 *                   call to chdir(cmd->argv[1]); and finally returns BI_EXECUTED
 *      BI_CMD_*     Indicates that a built-in command was matched and the caller
 *                   is responsible for executing it.  For example if this function
 *                   returns BI_CMD_STOP_SVR the caller of this function is
 *                   responsible for stopping the server.  If BI_CMD_EXIT is returned
 *                   the caller is responsible for closing the client connection.
 * 
 *   AGAIN - THIS IS TOTALLY OPTIONAL IF YOU HAVE OR WANT TO HANDLE BUILT-IN
 *   COMMANDS DIFFERENTLY. 
 */
Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds ctype = BI_NOT_BI;
    ctype = rsh_match_command(cmd->argv[0]);

    switch (ctype)
    {
    case BI_CMD_DRAGON:
        return BI_CMD_DRAGON;
    case BI_CMD_EXIT:
        return BI_CMD_EXIT;
    case BI_CMD_STOP_SVR:
        return BI_CMD_STOP_SVR;
    case BI_CMD_RC:
        return BI_CMD_RC;
    case BI_CMD_CD:
        chdir(cmd->argv[1]);
        return BI_EXECUTED;
    default:
        return BI_NOT_BI;
    }
}