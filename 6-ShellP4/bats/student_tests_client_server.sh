setup() {
    # Start server in background and save PID
    ./dsh -s &
    SERVER_PID=$!
    # Give the server a moment to start up
    sleep 3
}

# Teardown function to kill the server after each test
teardown() {
    # Kill server if it's running
    if [ -n "$SERVER_PID" ]; then
        kill $SERVER_PID 2>/dev/null || true
        wait $SERVER_PID 2>/dev/null || true
    fi
}


@test "client-server single command with unknown arg" {
    # Run the client and send command through a pipe
    run bash -c "echo -e 'ls -nonexistent\nexit' | ./dsh -c"
    
    # Check exit status
    [ "$status" -eq 0 ]
    
    # Process output (remove whitespace for easier comparison)
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="ls:invalidoption--'e'Try'ls--help'formoreinformation."
    
    # Output for debugging
    echo "${stripped_output} -> *${expected_output}"
    
    # Check if output contains the expected error message
    [[ "$stripped_output" == "$expected_output"* ]]
}

test "client-server command with invalid arg in pipeline" {
    # Run the client and send pipeline command
    run bash -c "echo 'ls -a | wc -nonexistent' | ./dsh -c"
    
    # Check exit status
    [ "$status" -eq 0 ]
    
    # Process output (remove whitespace for easier comparison)
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="wc:invalidoption--'n'Try'wc--help'formoreinformation."
    
    # Output for debugging
    echo "${stripped_output} -> ${expected_output}"
    
    # Check if output contains the expected error message
    [[ "$stripped_output" == *"$expected_output"* ]]
}

@test "client-server multiple commands in sequence" {
    # Create a temp file with multiple commands
    CMDS_FILE=$(mktemp)
    echo "echo 'First command'" >> $CMDS_FILE
    echo "ls -l /etc/passwd" >> $CMDS_FILE
    echo "exit" >> $CMDS_FILE
    
    # Run client with commands from file
    run bash -c "cat $CMDS_FILE | ./dsh -c"
    
    # Clean up
    rm -f $CMDS_FILE
    
    # Check for expected output fragments
    [[ "$output" == *"First command"* ]]
    [[ "$output" == *"/etc/passwd"* ]]
}

@test "client-server output redirection" {
    # Create a temp directory for test files
    TEST_DIR=$(mktemp -d)
    OUTPUT_FILE="$TEST_DIR/outfile.txt"
    
    # Run command with output redirection through client
    run bash -c "echo 'echo test output > $OUTPUT_FILE' | ./dsh -c"
    
    # Check if the file was created with correct content
    [ -f "$OUTPUT_FILE" ]
    file_content=$(cat "$OUTPUT_FILE")
    [ "$file_content" = "test output" ]
    
    # Clean up
    rm -rf "$TEST_DIR"
}

@test "client-server handles large output" {
    # Run a command that generates larger output
    run bash -c "echo 'cat /etc/services' | ./dsh -c"
    
    # Check that we got substantial output
    [ ${#output} -gt 1000 ]
    
    # Check for expected content in the services file
    [[ "$output" == *"ftp"* ]]
    [[ "$output" == *"ssh"* ]]
    [[ "$output" == *"http"* ]]
}

@test "client disconnection handling" {
    # Start a client, send a command and disconnect
    echo "echo exit" | ./dsh -c
    
    # Wait a moment
    sleep 1
    
    # Verify server is still running by connecting again
    run bash -c "echo 'echo Server still alive' | ./dsh -c"
    
    [ "$status" -eq 0 ]
    [[ "$output" == *"Server still alive"* ]]
}

@test "multiple clients sequential" {
    # First client
    run bash -c "echo 'echo Client 1' | ./dsh -c"
    [ "$status" -eq 0 ]
    [[ "$output" == *"Client 1"* ]]
    
    # Second client
    run bash -c "echo 'echo Client 2' | ./dsh -c"
    [ "$status" -eq 0 ]
    [[ "$output" == *"Client 2"* ]]
}

