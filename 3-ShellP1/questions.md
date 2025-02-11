1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  It is a line by line processor. It will process every time a newline or EOF is found. This makes it perfect for line by line processing in a shell. 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  This is because we can allocate dynamically based on the size in the header. We use the SH_CMD_MAX * CMD_MAX to allocate the max size that we can have for the input command string. So if the header values change the code will adjust accordingly. 


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: when trying to run a command the shell may not recognize it if there is a trailing space in either direction. 

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  

    One redirect is the ">" function, or redirecting STDOUT. An example of this ls > out.txt. This command lists the current directory, but instead of printing to STDOUT the contents are written in out.txt file. And this redirect operator overwrites everything in this file. If the output file does not exist the shell will create it. All of these are potential challenges of implementation, we will have to validate files and permissions of those files.

    Another redirection we should implement is the "<" function, or the STDIN function. An example is echo < sample.txt. The command takes the contents of sample.txt and gives it to echo as STDIN. Some challenges may be validating the command that processes the STDIN. If the command that is passed does not take STDIN that could cause issues. 

    The last we should implement is the ">>" function, or append STDOUT. An example would be echo "append this" >> out.txt. This redirect appends the STDOUT of the command to the file specified. In this case it would be out.txt an issue here would be making sure we are actually appending and not overwriting. All the other issues of the ">" redirect would also apply.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  They key difference is that redirects handle output/input to files/utilities while a pipe handles I/O between commands. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It isolates what is an error vs what is just output. This distinction is very important in order to know what you are looking at. STDERR is also displayed immediately so you can see where your program went wrong vs STDOUT which is buffered.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  We should print the STDERR message as soon as it processes and then end the process that is running if applicable. We should not provide a way to merge them since they are meant to be different. They should both display, but at their respective times. (STDERR right when caught and STDOUT when called by the successful process)