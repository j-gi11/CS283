1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Fork provides the security of keeping the original process running. If we call execvp() directly without creating a child process, then we ru the risk of execvp harming our current process. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the syscall fails, then it will return a negative error code. In my implementation I return the ERR_EXEC_CMD constant.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The $PATH env variable is the one in play here. execvp() looks in the bin directory for the commands to be executed, but it searches all of the $PATH dirs. 

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose is so that we can get the return status from the child process. Without calling wait we may have the parent process running without knowing the child exit status. The child may be running indefinitely or may have not worked. 

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  This function gets the exit staus of the child process from status code returned by wait(). It is important because we get to know how the child process terminates, either succesfully or not. 

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation checks for quotes and grabs every character until the next quote is found. It then stores the string, without the quote chars, in the argv array. It is necessary because the quotes perserve what is inside them. 

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  This was probably the hardest part of the assignment for me. The only old code I brought over was my trimWhiteSpace() function. The rest of the logic I rebuilt. So I didn't really make any changes to my old logic, I built new logic.  

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  The purpose is to notify a process of something that is happening. They differ from other forms because they are asychronus. 

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL: This is SIGNAL KILL, typically used to fully kill a process.
    SIGTERM: SIGNAL TERMINATE, typically used to ask a process to terminate. Unlike kill, the process can handle this request within it. 
    SIGINT: SIGNAL INTERRUPT, typically used to ask a process to interrupt it. It is also denoted by CTRL^C in STDIN.   
- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  The process is stopped until it recieves a SIGCONT. It cannot be caught or ignored by the process because then there would be no point of the signal. 