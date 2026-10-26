#!/usr/bin/env bash
# Usage: ./tests.sh
# Runs every test, prints a summary, exits 1 if at least one failed.
# The tests only use the compiled binaries (./r-type_server and ./r-type_client): they start them,
# check their arguments, logs and exit codes.

cd "$(dirname "$0")/.." || exit 1

if { [ -t 1 ] || [ -n "${CI:-}" ]; } && [ -z "${NO_COLOR:-}" ]; then
    GREEN=$'\e[32m' RED=$'\e[31m' YELLOW=$'\e[33m' BLUE=$'\e[1;34m' BOLD=$'\e[1m' DIM=$'\e[2m' RESET=$'\e[0m'
else
    GREEN="" RED="" YELLOW="" BLUE="" BOLD="" DIM="" RESET=""
fi

section() { echo "${BLUE}== $1 ==${RESET}"; }

passed=0
failed=0
skipped=0
failed_names=()
test_index=0
PORT_BASE=$((20000 + $$ % 20000))

run_test() {
    local name="$1"; shift
    local status

    test_index=$((test_index + 1))
    PORT=$((PORT_BASE + test_index))
    ( trap cleanup EXIT; "$@" ) > /tmp/test_output.log 2>&1
    status=$?
    if [ "$status" -eq 0 ]; then
        echo "  ${GREEN}[PASS]${RESET} $name"
        passed=$((passed + 1))
    elif [ "$status" -eq 77 ]; then
        echo "  ${YELLOW}[SKIP]${RESET} $name ${DIM}($(tail -n 1 /tmp/test_output.log))${RESET}"
        skipped=$((skipped + 1))
    else
        echo "  ${RED}[FAIL]${RESET} ${BOLD}$name${RESET}"
        sed "s/^/         ${RED}/; s/\$/${RESET}/" /tmp/test_output.log | tail -n 20
        failed=$((failed + 1))
        failed_names+=("$name")
    fi
}

# ============================================================
# Helpers
# ============================================================

SERVER_BIN=./r-type_server
CLIENT_BIN=./r-type_client
LOCAL=127.0.0.1

SERVER_PID=""
CLIENT_PID=""
SERVER_LOG=$(mktemp)
CLIENT_LOG=$(mktemp)
trap 'rm -f "$SERVER_LOG" "$CLIENT_LOG"' EXIT

cleanup() {
    local job

    for job in $(jobs -p); do
        pkill -9 -P "$job" 2>/dev/null
        kill -9 "$job" 2>/dev/null
    done
    [ -n "$SERVER_PID" ] && kill -9 "$SERVER_PID" 2>/dev/null
    [ -n "$CLIENT_PID" ] && kill -9 "$CLIENT_PID" 2>/dev/null
}

fail() { echo "FAIL: $*"; exit 1; }
skip() { echo "$*"; exit 77; }

# Waits until a process exits (at most $2 seconds, then kills it) and returns its exit code
wait_exit() {
    local pid="$1" watchdog code

    ( sleep "${2:-3}"; kill -9 "$pid" ) 2>/dev/null &
    watchdog=$!
    wait "$pid"
    code=$?
    pkill -P "$watchdog" 2>/dev/null
    kill "$watchdog" 2>/dev/null
    wait "$watchdog" 2>/dev/null
    return "$code"
}

is_running() {
    local state

    state=$(ps -o stat= -p "$1" 2>/dev/null)
    [ -n "$state" ] && [[ "$state" != Z* ]]
}

wait_log() {
    local file="$1" text="$2" tries=$(( ${3:-3} * 20 ))

    while (( tries-- > 0 )); do
        grep -qF -- "$text" "$file" && return 0
        sleep 0.05
    done
    grep -qF -- "$text" "$file"
}

start_server() {
    stdbuf -oL "$SERVER_BIN" "$LOCAL" "$PORT" < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_log "$SERVER_LOG" "Server listening on port" || fail "server did not start: $(cat "$SERVER_LOG")"
}

stop_server() {
    kill -"${1:-INT}" "$SERVER_PID" 2>/dev/null
    wait_exit "$SERVER_PID" 3
}

start_client() {
    stdbuf -oL "$CLIENT_BIN" "$LOCAL" "$PORT" < /dev/null > "$CLIENT_LOG" 2>&1 &
    CLIENT_PID=$!
}

need_display() {
    [ -n "${DISPLAY:-}${WAYLAND_DISPLAY:-}" ] || skip "no display to open the client window"
}

# Runs a binary that is expected to exit by itself, keeps its output in $RUN_LOG
RUN_LOG=""
run_binary() {
    local code

    RUN_LOG=$(timeout 5 "$@" < /dev/null 2>&1)
    code=$?
    return "$code"
}

# ============================================================
# Binaries
# ============================================================

# The server binary is built at the root of the repository and can be executed
test_server_binary_exists() { [ -x "$SERVER_BIN" ] || fail "missing $SERVER_BIN"; }

# The client binary is built at the root of the repository and can be executed
test_client_binary_exists() { [ -x "$CLIENT_BIN" ] || fail "missing $CLIENT_BIN"; }

# ============================================================
# Server arguments
# ============================================================

# A port that is not a number is refused with exit code 84 and an error message
test_server_args_port_not_a_number() {
    run_binary "$SERVER_BIN" abc; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Invalid port" <<< "$RUN_LOG" || fail "no 'Invalid port' message"
}

# The port 0 is refused with exit code 84
test_server_args_port_zero() { run_binary "$SERVER_BIN" 0; [ $? -eq 84 ] || fail "port 0 accepted"; }

# A port above 65535 is refused with exit code 84
test_server_args_port_too_big() { run_binary "$SERVER_BIN" 65536; [ $? -eq 84 ] || fail "port 65536 accepted"; }

# A negative port is refused with exit code 84
test_server_args_port_negative() { run_binary "$SERVER_BIN" -5; [ $? -eq 84 ] || fail "port -5 accepted"; }

# A port followed by letters is refused with exit code 84
test_server_args_port_with_letters() { run_binary "$SERVER_BIN" 42a; [ $? -eq 84 ] || fail "port 42a accepted"; }

# An empty port is refused with exit code 84
test_server_args_port_empty() { run_binary "$SERVER_BIN" ""; [ $? -eq 84 ] || fail "empty port accepted"; }

# An ip with numbers above 255 is refused with exit code 84 and an error message
test_server_args_invalid_ip() {
    run_binary "$SERVER_BIN" 999.1.1.1 "$PORT"; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Invalid ip" <<< "$RUN_LOG" || fail "no 'Invalid ip' message"
}

# An ip made of letters is refused with exit code 84
test_server_args_ip_letters() { run_binary "$SERVER_BIN" abc "$PORT"; [ $? -eq 84 ] || fail "ip abc accepted"; }

# More than two arguments are refused with exit code 84 and the usage
test_server_args_too_many() {
    run_binary "$SERVER_BIN" "$LOCAL" "$PORT" extra; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Usage" <<< "$RUN_LOG" || fail "no usage printed"
}

# An ip that does not belong to this machine makes the server exit with 84
test_server_bind_unknown_ip() {
    run_binary "$SERVER_BIN" 10.255.255.1 "$PORT"; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Fatal error" <<< "$RUN_LOG" || fail "no 'Fatal error' message"
}

# A second server on a port already used exits with 84
test_server_port_already_used() {
    start_server
    run_binary "$SERVER_BIN" "$LOCAL" "$PORT"; [ $? -eq 84 ] || fail "second server did not exit with 84"
}

# With only a port, the server listens on the default address 127.0.0.1
test_server_default_address() {
    stdbuf -oL "$SERVER_BIN" "$PORT" < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_log "$SERVER_LOG" "Server listening on ip : 127.0.0.1" || fail "$(cat "$SERVER_LOG")"
}

# With no argument, the server listens on 127.0.0.1:4242
test_server_default_port() {
    ss -lun 2>/dev/null | grep -q ':4242 ' && skip "port 4242 already used on this machine"
    stdbuf -oL "$SERVER_BIN" < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_log "$SERVER_LOG" "Server listening on port : 4242" || fail "$(cat "$SERVER_LOG")"
}

# ============================================================
# Client arguments
# ============================================================

# The client refuses a port that is not a number with exit code 84
test_client_args_port_not_a_number() {
    run_binary "$CLIENT_BIN" abc; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Invalid port" <<< "$RUN_LOG" || fail "no 'Invalid port' message"
}

# The client refuses the port 0 with exit code 84
test_client_args_port_zero() { run_binary "$CLIENT_BIN" 0; [ $? -eq 84 ] || fail "port 0 accepted"; }

# The client refuses a port above 65535 with exit code 84
test_client_args_port_too_big() { run_binary "$CLIENT_BIN" 70000; [ $? -eq 84 ] || fail "port 70000 accepted"; }

# The client refuses an invalid ip with exit code 84
test_client_args_invalid_ip() {
    run_binary "$CLIENT_BIN" not.an.ip 4242; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Invalid ip" <<< "$RUN_LOG" || fail "no 'Invalid ip' message"
}

# The client refuses more than two arguments with exit code 84 and the usage
test_client_args_too_many() {
    run_binary "$CLIENT_BIN" "$LOCAL" 4242 extra; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Usage" <<< "$RUN_LOG" || fail "no usage printed"
}

# Without a screen, the client exits with 84 and a clear error instead of crashing
test_client_without_display() {
    run_binary env -u DISPLAY -u WAYLAND_DISPLAY "$CLIENT_BIN" "$LOCAL" "$PORT"
    local code=$?
    [ "$code" -eq 84 ] || fail "exit code is $code, not 84"
    grep -q "Could not open the window" <<< "$RUN_LOG" || fail "no window error message"
}

# ============================================================
# Server start and shutdown
# ============================================================

# The server prints the ip it listens on
test_server_logs_ip() { start_server; grep -q "Server listening on ip : $LOCAL" "$SERVER_LOG" || fail "no ip log"; }

# The server prints the port it listens on
test_server_logs_port() { start_server; grep -q "Server listening on port : $PORT" "$SERVER_LOG" || fail "no port log"; }

# The server keeps running with a closed stdin (nohup, docker, CI)
test_server_runs_with_closed_stdin() {
    start_server
    sleep 1
    is_running "$SERVER_PID" || fail "the server stopped by itself"
}

# Ctrl+C (SIGINT) stops the server with exit code 0
test_server_sigint_exit_code() {
    start_server
    stop_server INT; local code=$?
    [ "$code" -eq 0 ] || fail "exit code is $code"
}

# The server prints 'Closing server' when it stops
test_server_logs_closing() {
    start_server
    stop_server INT
    grep -q "Closing server" "$SERVER_LOG" || fail "no 'Closing server' log"
}

# SIGTERM (kill, docker stop) stops the server with exit code 0
test_server_sigterm_exit_code() {
    start_server
    stop_server TERM; local code=$?
    [ "$code" -eq 0 ] || fail "exit code is $code"
}

# The server stops in less than one second
test_server_stops_quickly() {
    start_server
    kill -INT "$SERVER_PID"
    wait_exit "$SERVER_PID" 1 || fail "the server did not stop within 1 s"
}

# The port is free again right after the server stops
test_server_port_reusable() {
    start_server
    stop_server INT
    start_server
}

# ============================================================
# Real client (needs a screen, skipped otherwise)
# ============================================================

# The real client connects to the server
test_client_connects() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 0 connected" 5 || fail "the client did not connect"
}

# The real client keeps running while the server is up
test_client_stays_alive() {
    need_display
    start_server
    start_client
    sleep 3
    is_running "$CLIENT_PID" || fail "the client stopped: $(cat "$CLIENT_LOG")"
}

# The real client stops with code 0 and a disconnect message when the server stops
test_client_server_stops() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 0 connected" 5 || fail "the client did not connect"
    stop_server INT
    wait_exit "$CLIENT_PID" 5; local code=$?
    [ "$code" -eq 0 ] || fail "client exit code $code"
    grep -q "Disconnected by the server" "$CLIENT_LOG" || fail "no disconnect message"
}

# The real client prints the address it connects to and its player id
test_client_logs_connection() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 0 connected" 5 || fail "the client did not connect"
    stop_server INT
    wait_exit "$CLIENT_PID" 5
    grep -q "Connecting to $LOCAL:$PORT" "$CLIENT_LOG" || fail "no 'Connecting to' log"
    grep -q "Connected to $LOCAL:$PORT as player 0" "$CLIENT_LOG" || fail "no 'Connected to' log"
}

# A real client killed without warning is dropped by the server after the timeout
test_client_killed_is_dropped() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 0 connected" 5 || fail "the client did not connect"
    kill -9 "$CLIENT_PID"
    wait_log "$SERVER_LOG" "Player 0 timed out" 7 || fail "the killed client was not dropped"
}

# ============================================================
# Run
# ============================================================

section "Build"
run_test "build" bash -c "cmake -B build && cmake --build build"

section "Binaries"
run_test "server binary exists" test_server_binary_exists
run_test "client binary exists" test_client_binary_exists

section "Server arguments"
run_test "server: port not a number" test_server_args_port_not_a_number
run_test "server: port 0" test_server_args_port_zero
run_test "server: port too big" test_server_args_port_too_big
run_test "server: negative port" test_server_args_port_negative
run_test "server: port with letters" test_server_args_port_with_letters
run_test "server: empty port" test_server_args_port_empty
run_test "server: invalid ip" test_server_args_invalid_ip
run_test "server: ip with letters" test_server_args_ip_letters
run_test "server: too many arguments" test_server_args_too_many
run_test "server: unknown ip" test_server_bind_unknown_ip
run_test "server: port already used" test_server_port_already_used
run_test "server: default address" test_server_default_address
run_test "server: default port" test_server_default_port

section "Client arguments"
run_test "client: port not a number" test_client_args_port_not_a_number
run_test "client: port 0" test_client_args_port_zero
run_test "client: port too big" test_client_args_port_too_big
run_test "client: invalid ip" test_client_args_invalid_ip
run_test "client: too many arguments" test_client_args_too_many
run_test "client: no display" test_client_without_display

section "Server start and shutdown"
run_test "logs the ip" test_server_logs_ip
run_test "logs the port" test_server_logs_port
run_test "runs with closed stdin" test_server_runs_with_closed_stdin
run_test "SIGINT exits with 0" test_server_sigint_exit_code
run_test "logs Closing server" test_server_logs_closing
run_test "SIGTERM exits with 0" test_server_sigterm_exit_code
run_test "stops in less than 1 s" test_server_stops_quickly
run_test "port reusable after stop" test_server_port_reusable

section "Real client"
run_test "client connects" test_client_connects
run_test "client stays alive" test_client_stays_alive
run_test "client stops when the server stops" test_client_server_stops
run_test "client logs its connection" test_client_logs_connection
run_test "killed client dropped" test_client_killed_is_dropped

echo
echo "${BOLD}================================${RESET}"
echo " ${GREEN}Passed:  $passed${RESET}"
if [ "$failed" -ne 0 ]; then echo " ${RED}Failed:  $failed${RESET}"; else echo " Failed:  $failed"; fi
if [ "$skipped" -ne 0 ]; then echo " ${YELLOW}Skipped: $skipped${RESET}"; else echo " Skipped: $skipped"; fi
echo " ${BOLD}Total:   $((passed + failed + skipped))${RESET}"
echo "${BOLD}================================${RESET}"

if [ "$failed" -ne 0 ]; then
    echo "${RED}Failed tests:${RESET}"
    printf "  ${RED}-${RESET} %s\n" "${failed_names[@]}"
    exit 1
fi
