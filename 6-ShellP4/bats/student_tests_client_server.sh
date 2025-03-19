setup() {
  ./dsh -s > server.log 2>&1 &
  server_pid=$!
  sleep 1
}

teardown() {
  kill $server_pid 2>/dev/null
  rm -f server.log
}



@test "simple command" {
  run ./dsh -c <<EOF
ls
EOF
  echo "Client output:"
  echo "$output"
  [ "$status" -eq 0 ]
}


@test "exit right away" {
  run ./dsh -c <<EOF
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>cmdloopreturned0"
  echo "Output: $output"
  echo "Stripped Output: ${stripped_output}"
  [ "$stripped_output" = "$expected_output" ]
  [ "$status" -eq 0 ]
}


@test "exit with custom ip" {
  run ./dsh -c -i 127.0.0.2<<EOF
exit
EOF
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="socketclientmode:addr:127.0.0.2:1234dsh4>cmdloopreturned0"
  echo "Output: $output"
  echo "Stripped Output: ${stripped_output}"
  [ "$stripped_output" = "$expected_output" ]
  [ "$status" -eq 0 ]
}


@test "one pipe" {
  run ./dsh -c <<EOF
ls | grep dsh
exit
EOF
  # Adjust expected output as needed.
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>dshdsh_cli.cdshlib.cdshlib.hdsh4>cmdloopreturned0"
  echo "Output: $output"
  echo "Stripped Output: ${stripped_output}"
  [ "$stripped_output" = "$expected_output" ]
  [ "$status" -eq 0 ]
}

@test "test unclosed quotes" {
    run ./dsh -c <<EOF
    echo "hello single quote' not closed here
    exit
EOF
    [ "$status" -eq 0 ]
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    expected_output="quoteunclosed"
    echo "${stripped_output} -> ${expected_output}"
    [[ "$stripped_output" == *"$expected_output"* ]]
}

@test "stop server" {
    run ./dsh -c <<EOF
    stop-server
EOF
    # [ "$status" -eq 0 ]

    server_log_output=$(cat server.log)
    expected_output="${output}${server_log_output}"

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v ')
    stripped_log_output=$(echo "$server_log_output" | tr -d '\t\n\r\f\v ')
    stripped_full_output="${stripped_output}${stripped_log_output}"

    expected_output="recievedcommand:stop-servercmdloopreturned-7"

    echo "${stripped_full_output} -> ${expected_output}"

    [[ "$stripped_full_output" == *"$expected_output"* ]]
}


@test "max pipes" {
    run ./dsh -c <<EOF
    echo "This is a test" | tr 'a-z' 'A-Z' | rev | sed 's/T/TT/g' | awk '{print $1}' | grep T | wc -l | cat
    exit
EOF
    [ "$status" -eq 0 ]

  # Adjust expected output as needed.
  stripped_output=$(echo "$output" | tr -d '[:space:]')
  expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>1dsh4>dsh4>cmdloopreturned-52"
  echo "Output: $output"
  echo "Stripped Output: ${stripped_output}"
  [ "$stripped_output" = "$expected_output" ]
}


