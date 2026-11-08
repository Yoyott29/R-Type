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

# Waits until a UDP socket is bound on the address $1 (ip:port), at most $2 seconds
wait_bound() {
    local address="$1" tries=$(( ${2:-3} * 20 ))

    while (( tries-- > 0 )); do
        ss -lun 2>/dev/null | grep -qF " $address " && return 0
        sleep 0.05
    done
    return 1
}

start_server() {
    stdbuf -oL "$SERVER_BIN" "$LOCAL" "$PORT" < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_bound "$LOCAL:$PORT" || fail "server did not start: $(cat "$SERVER_LOG")"
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

# Checks that a binary given wrong arguments exits with 84 and, when it can print it, the message
expect_refused() {
    local message="$1"; shift
    run_binary "$@"; local code=$?
    [ "$code" -eq 84 ] || fail "exit code is $code, not 84"
    [ -z "$message" ] || grep -q -- "$message" <<< "$RUN_LOG" || fail "no '$message' message: $RUN_LOG"
}

# The client opens its window before reading its address: without a screen, the window error
# comes first, so the message is only checked when there is a screen
expect_client_refused() {
    local message="$1"; shift
    if [ -n "${DISPLAY:-}${WAYLAND_DISPLAY:-}" ]; then
        expect_refused "$message" "$CLIENT_BIN" "$@"
    else
        expect_refused "" "$CLIENT_BIN" "$@"
    fi
}

EDITOR_BIN="Gringo Engine/Gringo"
TNT_DIR="Gringo Engine/TrackNTrap"
need_editor() { [ -x "$EDITOR_BIN" ] || skip "the editor is not built (Gringo Engine/build_engine.sh)"; }
need_trackntrap() { [ -x "$TNT_DIR/trackntrap_server" ] || skip "TrackNTrap is not built"; }

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
    grep -q "Runtime error" <<< "$RUN_LOG" || fail "no 'Runtime error' message"
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
    wait_bound "127.0.0.1:$PORT" || fail "not bound on 127.0.0.1:$PORT: $(cat "$SERVER_LOG")"
}

# With no argument, the server listens on 127.0.0.1:4242
test_server_default_port() {
    ss -lun 2>/dev/null | grep -q ':4242 ' && skip "port 4242 already used on this machine"
    stdbuf -oL "$SERVER_BIN" < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_bound "127.0.0.1:4242" || fail "not bound on 127.0.0.1:4242: $(cat "$SERVER_LOG")"
}

# ============================================================
# Client arguments
# ============================================================

# The client refuses the port 0 with exit code 84
test_client_args_port_zero() { run_binary "$CLIENT_BIN" 0; [ $? -eq 84 ] || fail "port 0 accepted"; }

# The client refuses a port above 65535 with exit code 84
test_client_args_port_too_big() { run_binary "$CLIENT_BIN" 70000; [ $? -eq 84 ] || fail "port 70000 accepted"; }

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
    wait_log "$SERVER_LOG" "Player 1 connected" 5 || fail "the client did not connect"
}

# The real client keeps running while the server is up
test_client_stays_alive() {
    need_display
    start_server
    start_client
    sleep 3
    is_running "$CLIENT_PID" || fail "the client stopped: $(cat "$CLIENT_LOG")"
}

# A real client killed without warning is dropped by the server after the timeout
test_client_killed_is_dropped() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 1 connected" 5 || fail "the client did not connect"
    kill -9 "$CLIENT_PID"
    wait_log "$SERVER_LOG" "Player 1 timed out" 7 || fail "the killed client was not dropped"
}

# ============================================================
# More arguments
# ============================================================

test_server_args_port_six_digits() { expect_refused "Invalid port" "$SERVER_BIN" 012345; }
test_server_args_port_leading_space() { expect_refused "Invalid port" "$SERVER_BIN" " 4242"; }
test_server_args_port_plus_sign() { expect_refused "Invalid port" "$SERVER_BIN" +4242; }
test_server_args_port_decimal() { expect_refused "Invalid port" "$SERVER_BIN" 4242.5; }
test_server_args_port_hex() { expect_refused "Invalid port" "$SERVER_BIN" 0x1092; }
test_server_args_ip_three_parts() { expect_refused "Invalid ip" "$SERVER_BIN" 1.2.3 "$PORT"; }
test_server_args_ip_five_parts() { expect_refused "Invalid ip" "$SERVER_BIN" 1.2.3.4.5 "$PORT"; }
test_server_args_ip_v6() { expect_refused "Invalid ip" "$SERVER_BIN" ::1 "$PORT"; }
test_server_args_ip_hostname() { expect_refused "Invalid ip" "$SERVER_BIN" localhost "$PORT"; }
test_server_args_ip_empty() { expect_refused "Invalid ip" "$SERVER_BIN" "" "$PORT"; }
test_server_args_ip_trailing_dot() { expect_refused "Invalid ip" "$SERVER_BIN" 127.0.0.1. "$PORT"; }
test_server_args_ip_negative() { expect_refused "Invalid ip" "$SERVER_BIN" 127.0.0.-1 "$PORT"; }
test_server_args_bad_ip_and_port() { expect_refused "Invalid port" "$SERVER_BIN" nope nope; }

# The highest port is accepted
test_server_args_port_max() {
    ss -lun 2>/dev/null | grep -q ':65535 ' && skip "port 65535 already used on this machine"
    stdbuf -oL "$SERVER_BIN" "$LOCAL" 65535 < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_bound "$LOCAL:65535" || fail "not bound on port 65535: $(cat "$SERVER_LOG")"
}

# 0.0.0.0 listens on every interface
test_server_args_any_address() {
    stdbuf -oL "$SERVER_BIN" 0.0.0.0 "$PORT" < /dev/null > "$SERVER_LOG" 2>&1 &
    SERVER_PID=$!
    wait_bound "0.0.0.0:$PORT" || fail "not bound on 0.0.0.0:$PORT: $(cat "$SERVER_LOG")"
}

# A bad argument is refused at once: the server does not open its socket first
test_server_args_refused_quickly() {
    local start end
    start=$(date +%s%N)
    run_binary "$SERVER_BIN" abc
    end=$(date +%s%N)
    (( (end - start) / 1000000 < 2000 )) || fail "took $(( (end - start) / 1000000 )) ms"
}

test_client_args_port_not_a_number() { expect_client_refused "Invalid port" abc; }
test_client_args_port_negative() { expect_client_refused "Invalid port" -1; }
test_client_args_port_empty() { expect_client_refused "Invalid port" ""; }
test_client_args_port_letters() { expect_client_refused "Invalid port" 42a; }
test_client_args_invalid_ip() { expect_client_refused "Invalid ip" 999.1.1.1 "$PORT"; }
test_client_args_ip_letters() { expect_client_refused "Invalid ip" abc "$PORT"; }
test_client_args_too_many() { expect_client_refused "Usage" "$LOCAL" "$PORT" extra; }

# ============================================================
# More real client tests (need a screen, skipped otherwise)
# ============================================================

# Without a server, the real client stays open on its lobby instead of crashing
test_client_without_server() {
    need_display
    start_client
    sleep 7
    is_running "$CLIENT_PID" || fail "the client stopped: $(cat "$CLIENT_LOG")"
}

# Two real clients join the same server
test_client_two_players() {
    need_display
    start_server
    start_client
    stdbuf -oL "$CLIENT_BIN" "$LOCAL" "$PORT" < /dev/null > /dev/null 2>&1 &
    wait_log "$SERVER_LOG" "Player 2 connected" 5 || fail "the second client did not connect"
}

# A real client closed with SIGINT tells the server it leaves
test_client_sigint_leaves() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 1 connected" 5 || fail "the client did not connect"
    kill -INT "$CLIENT_PID"
    wait_log "$SERVER_LOG" "Player 1 left" 3 || fail "the server did not see the client leave"
}

# ============================================================
# Editor command line
# ============================================================

# --build without a project prints the usage and exits with 84
test_editor_usage() { need_editor; expect_refused "Usage" "$EDITOR_BIN" --build; }

# --build on a missing project exits with 84
test_editor_missing_project() { need_editor; expect_refused "" "$EDITOR_BIN" --build /nonexistent/Nope.gringo; }

# --build on a file that is not a project exits with 84 instead of crashing
test_editor_broken_project() {
    need_editor
    local file
    file=$(mktemp --suffix=.gringo)
    echo "{ this is not json" > "$file"
    run_binary "$EDITOR_BIN" --build "$file"; local code=$?
    rm -f "$file"
    [ "$code" -eq 84 ] || fail "exit code is $code, not 84"
}

# ============================================================
# TrackNTrap (the second game, if it is built)
# ============================================================

test_tnt_server_bad_port() { need_trackntrap; expect_refused "Invalid port" "$TNT_DIR/trackntrap_server" abc; }

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
run_test "server: port of six digits" test_server_args_port_six_digits
run_test "server: port with a space" test_server_args_port_leading_space
run_test "server: port with a plus sign" test_server_args_port_plus_sign
run_test "server: decimal port" test_server_args_port_decimal
run_test "server: hexadecimal port" test_server_args_port_hex
run_test "server: ip of three parts" test_server_args_ip_three_parts
run_test "server: ip of five parts" test_server_args_ip_five_parts
run_test "server: IPv6 address" test_server_args_ip_v6
run_test "server: host name" test_server_args_ip_hostname
run_test "server: empty ip" test_server_args_ip_empty
run_test "server: ip with a trailing dot" test_server_args_ip_trailing_dot
run_test "server: negative ip part" test_server_args_ip_negative
run_test "server: bad ip and bad port" test_server_args_bad_ip_and_port
run_test "server: port 65535 accepted" test_server_args_port_max
run_test "server: 0.0.0.0 accepted" test_server_args_any_address
run_test "server: refused at once" test_server_args_refused_quickly

section "Client arguments"
run_test "client: port 0" test_client_args_port_zero
run_test "client: port too big" test_client_args_port_too_big
run_test "client: no display" test_client_without_display
run_test "client: port not a number" test_client_args_port_not_a_number
run_test "client: negative port" test_client_args_port_negative
run_test "client: empty port" test_client_args_port_empty
run_test "client: port with letters" test_client_args_port_letters
run_test "client: invalid ip" test_client_args_invalid_ip
run_test "client: ip with letters" test_client_args_ip_letters
run_test "client: too many arguments" test_client_args_too_many

section "Server start and shutdown"
run_test "runs with closed stdin" test_server_runs_with_closed_stdin
run_test "SIGINT exits with 0" test_server_sigint_exit_code
run_test "SIGTERM exits with 0" test_server_sigterm_exit_code
run_test "stops in less than 1 s" test_server_stops_quickly
run_test "port reusable after stop" test_server_port_reusable

section "Real client"
run_test "client connects" test_client_connects
run_test "client stays alive" test_client_stays_alive
run_test "killed client dropped" test_client_killed_is_dropped
run_test "client without a server" test_client_without_server
run_test "two real clients" test_client_two_players
run_test "client leaves on SIGINT" test_client_sigint_leaves

section "Editor"
run_test "--build without project" test_editor_usage
run_test "--build on a missing project" test_editor_missing_project
run_test "--build on a broken project" test_editor_broken_project

section "TrackNTrap"
run_test "server: bad port" test_tnt_server_bad_port

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
