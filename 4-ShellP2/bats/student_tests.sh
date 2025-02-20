#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


@test "exe name to big (64chars)" {
    run "./dsh" <<EOF                
hhhhhhhhhhhhhhhhhhssssssssssssssssssssssssssssssssssssssssssssss arg1 
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2> Command to bigdsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}


@test "handle pwd with spaces" {
    current=$(pwd)

    run "./dsh" <<EOF                
           pwd          
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="${current}dsh2> dsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}



@test "exit right away" {
    current=$(pwd)

    run "./dsh" <<EOF                
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}


@test "enter nothing" {

    run "./dsh" <<EOF
              
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2> dsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}


@test "try echo with single quotes" {

    run "./dsh" <<EOF
echo 'hello world'
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="'hello world'dsh2> dsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}


@test "dont close quote" {

    run "./dsh" <<EOF
echo "this isnt closed
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2> Bad commanddsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "ls with invaid flag" {

    run "./dsh" <<EOF
ls -j
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="ls: invalid option -- 'j'Try 'ls --help' for more information.dsh2> dsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}


@test "Test SIGINT w/ ^C" {

    run "./dsh" <<^C
^C


    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh2> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output}"
    echo "${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}



