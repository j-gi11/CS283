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

#tests from last assn to assure that single commands are working as expected.

@test "Change directory" {
    current=$(pwd)

    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF                
cd dsh-test
pwd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="/tmp/dsh-testdsh3>dsh3>dsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Change directory - no args" {
    current=$(pwd)

    cd /tmp
    mkdir -p dsh-test

    run "${current}/dsh" <<EOF                
cd
pwd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="/tmpdsh3>dsh3>dsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}


@test "Which which ... which?" {
    run "./dsh" <<EOF                
which which
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="/usr/bin/whichdsh3>dsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "It handles quoted spaces" {
    run "./dsh" <<EOF                
   echo " hello     world     " 
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    # Expected output with all whitespace removed for easier matching
    expected_output=" hello     world     dsh3> dsh3> cmd loop returned 0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

#New tests
@test "piping with silent middle command" {
    run "./dsh" <<EOF
    echo "test" | grep -v "nothing" | cat
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="testdsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "non-existent commands" {
    run "./dsh" <<EOF
    nonexistentcommand
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="dsh3> error: execution faileddsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "handles quoted and unquoted args" {
    run "./dsh" <<EOF
    echo hello "beautiful world" program
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="hello beautiful world programdsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "commands with multiple arguments" {
    run "./dsh" <<EOF
    echo hello world program shell test
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="hello world program shell testdsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "handles too many pipes" {
    run "./dsh" <<EOF
    ls | grep a | sort | uniq | wc -l | tr -d ' ' | sed 's/$//' | cat | head | tail | grep .
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="dsh3> error: piping limited to 8 commandsdsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "piping with quoted args" {
    run "./dsh" <<EOF
    echo "hello world" | grep "hello"
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="hello worlddsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "empty input" {
    run "./dsh" <<EOF

EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="dsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "single quotes in double quotes" {
    run "./dsh" <<EOF
echo "hello 'world' test"

EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="hello 'world' testdsh3> dsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "simple pipe" {
    run "./dsh" <<EOF
    ls | grep d
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="dragon.cdshdsh_cli.cdshlib.cdshlib.hquestions.mddsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "2 simple pipes" {
    run "./dsh" <<EOF
    ls | grep d | wc -l
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="6dsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "max cmds - 7 pipes" {
    run "./dsh" <<EOF
    echo "This is a test" | tr 'a-z' 'A-Z' | rev | sed 's/T/TT/g' | awk '{print $1}' | grep T | wc -l | cat
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="1dsh3> dsh3> cmd loop returned 0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "another pipe test" {
    run "./dsh" <<EOF
    echo "banana apple apple orange banana" | tr ' ' '\n' | sort | uniq
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="applebananaorangedsh3>dsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "multiple spaces" {
    run "./dsh" <<EOF
    echo hello     world   shell
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="helloworldshelldsh3>dsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "trailing spaces" {
    run "./dsh" <<EOF
    echo hello world     
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="helloworlddsh3>dsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "pipe test w quotes in quotes" {
    run "./dsh" <<EOF
    echo "outer 'inner' text" | sed 's/inner/INNER/'
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="outer'INNER'textdsh3>dsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}

@test "test unclosed quotes" {
    run "./dsh" <<EOF
    echo "hello single quote' not closed here
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="dsh3>error:quoteuncloseddsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "testing a single command with unknown arg" {
    run "./dsh" <<EOF
    ls -nonexistent
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="ls:invalidoption--'e'Try'ls--help'formoreinformation.dsh3>dsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}


@test "command with invalid arg in pipeline" {
    run "./dsh" <<EOF
ls -a | wc -nonexistent
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="wc:invalidoption--'n'Try'wc--help'formoreinformation.dsh3>dsh3>cmdloopreturned0"
    echo "${stripped_output} -> ${expected_output}"
    [ "$stripped_output" = "$expected_output" ]
}