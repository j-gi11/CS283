1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

    - I ensure that all child processes are complete by looping over the process ids that were created and waiting for their return status. Thi sensures that each process returns/finishes before moving onto the next one. If I forgot to call waitpid() on all the child processes, then I could end up with zombie processes. The shell would also accept new commands before the previuos commands finished executing.   

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

    - It is necessary to close unused pipes after calling dup2() because it could lead to resource wasting and termination issues. If pipes remain open issues of deadlocking could be raised. 

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

    - if cd was an external command, then the child processes working directory would only get updated. This, in turn, would not update the dsh's working directory. If cd was an external process some challenges would be recognizing the change in directory before other child processes execute. This could effect the command pipeline. 

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

    - I would modify my implementation by dynamically allocated the commands[] array in the command_list_t struct. I could do this by using a linked list, or making the size larger once the max size of the current memory allocatiion is reached. The biggest tradeoff is memory management, and how complex I want it to be. The more I have to dynamically allocate memory, the more complex and difficult it is to manage it efficiently. This could lead to memory leaks, and inefficient memory handling. 