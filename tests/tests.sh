#!/usr/bin/env bash
# Usage: ./tests.sh
# Runs every test, prints a summary, exits 1 if at least one failed.
# The tests only use the compiled binaries (./r-type_server and ./r-type_client): they start them,
# talk to them over UDP with /dev/udp, and check what comes back.

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
PLAYER=0
ENEMY=1
BULLET=2
UP=1
DOWN=2
LEFT=4
RIGHT=8
CHARGE=16
VERSION=03
ENTITY_SIZE=25

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
check() { awk "BEGIN { exit !($1) }" || fail "${2:-expected $1}"; }

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

# Opens a new UDP socket to the server in the variable named $1 (a new client, a new source port)
open_client() { eval "exec {$1}<>/dev/udp/$LOCAL/$PORT"; }

# printf alone splits its output at each 0x0a byte, which would cut the packet in two datagrams:
# dd collects the whole packet and sends it in one write
send_bytes() {
    local fd="$1" format="" byte
    shift

    for byte in "$@"; do
        format+="\\x$byte"
    done
    printf "$format" | dd bs=65536 iflag=fullblock 2>/dev/null >&"$fd"
}

send_input() { send_bytes "$1" 30 00 00 00 00 "$(printf '%02x' "$2")" 00 00 00 00; }
disconnect_client() { send_bytes "$1" 11; }

# Decodes the raw server packets into one line each:
# ACCEPT <id> | REJECT <reason> | LEFT <id> | STARTED <tick> | SNAPSHOT <tick> <count> <type>:<x>:<y>...
# A snapshot split in several packets of the same tick is printed as one SNAPSHOT line
PARSE='
function hex(s,   i, value) {
    value = 0
    for (i = 1; i <= length(s); i++)
        value = value * 16 + index("0123456789abcdef", substr(s, i, 1)) - 1
    return value
}
function f32(o,   bits, negative, e, m, value) {
    bits = hex(b[o] b[o + 1] b[o + 2] b[o + 3])
    negative = bits >= 2147483648
    if (negative)
        bits -= 2147483648
    e = int(bits / 8388608)
    m = bits - e * 8388608
    value = e == 0 ? m * 2 ^ (-149) : (1 + m / 8388608) * 2 ^ (e - 127)
    return negative ? -value : value
}
function flush() {
    if (stick != "")
        print "SNAPSHOT " stick " " scount sentities
    stick = ""
}
{ for (i = 1; i <= NF; i++) b[n++] = $i }
END {
    p = 0
    while (p < n) {
        t = b[p]
        if (t == "c0") {
            count = hex(b[p + 5] b[p + 6])
            if (p + 7 + ENTITY_SIZE * count > n) { flush(); print "BROKEN"; exit }
            tick = hex(b[p + 1] b[p + 2] b[p + 3] b[p + 4])
            if (tick != stick) {
                flush()
                stick = tick
                scount = 0
                sentities = ""
            }
            scount += count
            for (k = 0; k < count; k++) {
                o = p + 7 + ENTITY_SIZE * k
                sentities = sentities " " hex(b[o + 8]) ":" sprintf("%g", f32(o)) ":" sprintf("%g", f32(o + 4))
            }
            p += 7 + ENTITY_SIZE * count
        } else if (t == "90" || t == "91" || t == "92") {
            flush()
            print (t == "90" ? "ACCEPT " : t == "91" ? "REJECT " : "LEFT ") hex(b[p + 1])
            p += 2
        } else if (t == "a0") {
            flush()
            print "STARTED " hex(b[p + 1] b[p + 2] b[p + 3] b[p + 4])
            p += 5
        } else {
            flush()
            print "UNKNOWN " t
            exit
        }
    }
    flush()
}'

# Prints every packet the socket $1 receives during $2 seconds
listen() { timeout "$2" dd bs=2048 <&"$1" 2>/dev/null | od -An -tx1 -v | awk -v ENTITY_SIZE="$ENTITY_SIZE" "$PARSE"; }

# Sends CONNECT (version $2, default $VERSION) and prints the answer: "ACCEPT <id>" or "REJECT <reason>"
connect_client() {
    send_bytes "$1" 10 "${2:-$VERSION}"
    listen "$1" 0.3 | grep -m 1 -E '^(ACCEPT|REJECT) '
}

# Sends the input bits $2 every 10 ms during $3 seconds and prints the packets received meanwhile
hold() {
    local fd="$1" bits="$2" pid

    ( while :; do send_input "$fd" "$bits"; sleep 0.01; done ) &
    pid=$!
    listen "$fd" "$3"
    kill "$pid" 2>/dev/null
    wait "$pid" 2>/dev/null
}

# Prints the newest complete snapshot received by the socket $1 (the very last one can be cut by the timeout)
last_snapshot() { listen "$1" "${2:-0.15}" | grep '^SNAPSHOT' | tail -n 2 | head -n 1; }

# Counts the entities of type $2 in the snapshot line $1
count_type() {
    awk -v type="$2" '{ n = 0; for (i = 4; i <= NF; i++) { split($i, e, ":"); if (e[1] == type) n++ } print n }' <<< "$1"
}

# Prints "x y" of the first entity of type $2 in the snapshot line $1
first_xy() {
    awk -v type="$2" '{ for (i = 4; i <= NF; i++) { split($i, e, ":"); if (e[1] == type) { print e[2], e[3]; exit } } }' <<< "$1"
}

# Prints "x y" of the last (newest) entity of type $2 in the snapshot line $1
last_xy() {
    awk -v type="$2" '{ for (i = 4; i <= NF; i++) { split($i, e, ":"); if (e[1] == type) xy = e[2] " " e[3] } print xy }' <<< "$1"
}

player_x() { first_xy "$1" "$PLAYER" | cut -d' ' -f1; }
player_y() { first_xy "$1" "$PLAYER" | cut -d' ' -f2; }

connected_clients() {
    local name answer

    for name in "$@"; do
        open_client "$name"
        answer=$(connect_client "${!name}")
        [[ "$answer" == ACCEPT* ]] || fail "$name was not accepted: '$answer'"
    done
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

# The server stops cleanly while clients are connected and playing
test_server_shutdown_with_clients() {
    start_server
    connected_clients A B
    hold "$A" "$RIGHT" 0.2 > /dev/null
    stop_server INT; local code=$?
    [ "$code" -eq 0 ] || fail "exit code is $code"
}

# The port is free again right after the server stops
test_server_port_reusable() {
    start_server
    stop_server INT
    start_server
}

# ============================================================
# Connection
# ============================================================

# The first client is accepted with the player id 0
test_connect_first_client_id_0() {
    start_server
    open_client A
    [ "$(connect_client "$A")" = "ACCEPT 0" ] || fail "first client not accepted as 0"
}

# The server prints the id and address of a new player
test_connect_logs_player() {
    start_server
    connected_clients A
    wait_log "$SERVER_LOG" "Player 0 connected: $LOCAL:" || fail "no connection log"
}

# The second client is accepted with the player id 1
test_connect_second_client_id_1() {
    start_server
    connected_clients A
    open_client B
    [ "$(connect_client "$B")" = "ACCEPT 1" ] || fail "second client not accepted as 1"
}

# Four clients get the ids 0, 1, 2 and 3
test_connect_four_clients() {
    local id name
    start_server
    for id in 0 1 2 3; do
        name="C$id"
        open_client "$name"
        [ "$(connect_client "${!name}")" = "ACCEPT $id" ] || fail "client $id not accepted"
    done
}

# A fifth client is rejected with SERVER_FULL (0x01)
test_connect_fifth_rejected() {
    start_server
    connected_clients A B C D
    open_client E
    [ "$(connect_client "$E")" = "REJECT 1" ] || fail "fifth client not rejected with SERVER_FULL"
}

# A client with a newer protocol version is rejected with WRONG_VERSION (0x02)
test_connect_wrong_version() {
    start_server
    open_client A
    [ "$(connect_client "$A" 04)" = "REJECT 2" ] || fail "version 4 not rejected"
}

# A client with the protocol version 0 is rejected with WRONG_VERSION
test_connect_version_zero() {
    start_server
    open_client A
    [ "$(connect_client "$A" 00)" = "REJECT 2" ] || fail "version 0 not rejected"
}

# A client rejected for its version can connect with the right version
test_connect_after_wrong_version() {
    start_server
    open_client A
    connect_client "$A" 02 > /dev/null
    [ "$(connect_client "$A")" = "ACCEPT 0" ] || fail "right version refused after a wrong one"
}

# A CONNECT of one byte (no version) gets no answer
test_connect_too_short_ignored() {
    start_server
    open_client A
    send_bytes "$A" 10
    [ -z "$(listen "$A" 0.3)" ] || fail "the server answered a 1 byte CONNECT"
}

# A CONNECT of three bytes gets no answer
test_connect_too_long_ignored() {
    start_server
    open_client A
    send_bytes "$A" 10 01 00
    [ -z "$(listen "$A" 0.3)" ] || fail "the server answered a 3 byte CONNECT"
}

# A client sending CONNECT twice keeps the same id
test_connect_twice_same_id() {
    start_server
    connected_clients A
    open_client B
    connect_client "$B" > /dev/null
    [ "$(connect_client "$B")" = "ACCEPT 1" ] || fail "id changed on the second CONNECT"
}

# An INPUT from an unknown client is answered with NOT_CONNECTED (0x04)
test_connect_input_before_connect() {
    start_server
    open_client A
    send_input "$A" "$RIGHT"
    listen "$A" 0.3 | grep -q "^REJECT 4" || fail "no NOT_CONNECTED answer"
}

# A DISCONNECT from an unknown client is answered with NOT_CONNECTED
test_connect_disconnect_before_connect() {
    start_server
    open_client A
    disconnect_client "$A"
    listen "$A" 0.3 | grep -q "^REJECT 4" || fail "no NOT_CONNECTED answer"
}

# An unknown packet from an unknown client is answered with NOT_CONNECTED
test_connect_unknown_packet_before_connect() {
    start_server
    open_client A
    send_bytes "$A" 55 01
    listen "$A" 0.3 | grep -q "^REJECT 4" || fail "no NOT_CONNECTED answer"
}

# A freed id is given back to the next client (lowest free id first)
test_connect_lowest_free_id() {
    start_server
    connected_clients A B C
    disconnect_client "$B"
    sleep 0.1
    open_client D
    [ "$(connect_client "$D")" = "ACCEPT 1" ] || fail "the freed id 1 was not reused"
}

# ============================================================
# Snapshots
# ============================================================

# A connected client receives snapshots right away
test_snapshot_after_connect() {
    start_server
    connected_clients A
    [ -n "$(last_snapshot "$A" 0.3)" ] || fail "no snapshot received"
}

# Every snapshot has a size that matches its entity count
test_snapshot_well_formed() {
    start_server
    connected_clients A
    local packets
    packets=$(listen "$A" 0.5)
    grep -q "^SNAPSHOT" <<< "$packets" || fail "no snapshot"
    ! grep -qE "^(BROKEN|UNKNOWN)" <<< "$packets" || fail "malformed packet: $packets"
}

# The snapshot of a single client contains exactly one player
test_snapshot_one_player() {
    start_server
    connected_clients A
    [ "$(count_type "$(last_snapshot "$A")" "$PLAYER")" = 1 ] || fail "not exactly one player"
}

# The first player appears at its spawn point (100, 200)
test_snapshot_first_spawn_point() {
    start_server
    connected_clients A
    [ "$(first_xy "$(last_snapshot "$A")" "$PLAYER")" = "100 200" ] || fail "player not at (100, 200)"
}

# The four players spawn 100 pixels apart, from y = 200 to y = 500
test_snapshot_spawn_points() {
    start_server
    connected_clients A B C D
    sleep 0.1
    local heights
    heights=$(last_snapshot "$A" | tr ' ' '\n' | grep "^$PLAYER:" | cut -d: -f3 | sort -n | tr '\n' ' ')
    [ "$heights" = "200 300 400 500 " ] || fail "spawn heights are '$heights'"
}

# Every client sees every player
test_snapshot_all_clients_see_all_players() {
    local name
    start_server
    connected_clients A B C
    sleep 0.1
    for name in A B C; do
        [ "$(count_type "$(last_snapshot "${!name}")" "$PLAYER")" = 3 ] || fail "$name does not see 3 players"
    done
}

# The server sends about 60 snapshots per second
test_snapshot_rate() {
    start_server
    connected_clients A
    local count
    count=$(listen "$A" 1 | grep -c "^SNAPSHOT")
    check "$count >= 50 && $count <= 70" "received $count snapshots in 1 s"
}

# The snapshot ticks always go up
test_snapshot_ticks_increase() {
    start_server
    connected_clients A
    listen "$A" 0.5 | awk '$1 == "SNAPSHOT" { if (seen && $2 <= last) exit 1; last = $2; seen = 1 } END { exit !seen }' \
        || fail "ticks do not go up"
}

# Snapshots only contain known entity types (player, enemy, bullet, obstacle, other)
test_snapshot_known_types() {
    start_server
    connected_clients A
    hold "$A" "$CHARGE" 0.3 | grep "^SNAPSHOT" | tr ' ' '\n' | grep ':' | cut -d: -f1 | sort -u | grep -qv '^[0-4]$' \
        && fail "unknown entity type in a snapshot"
    return 0
}

# A client that never connected receives no snapshot
test_snapshot_not_sent_to_strangers() {
    start_server
    connected_clients A
    open_client B
    ! listen "$B" 0.3 | grep -q "^SNAPSHOT" || fail "a stranger received snapshots"
}

# A client rejected because the server is full receives no snapshot
test_snapshot_not_sent_to_rejected() {
    start_server
    connected_clients A B C D
    open_client E
    connect_client "$E" > /dev/null
    ! listen "$E" 0.3 | grep -q "^SNAPSHOT" || fail "a rejected client received snapshots"
}

# A client rejected for its version receives no snapshot
test_snapshot_not_sent_to_wrong_version() {
    start_server
    open_client A
    connect_client "$A" 02 > /dev/null
    ! listen "$A" 0.3 | grep -q "^SNAPSHOT" || fail "a wrong version client received snapshots"
}

# A client that sent DISCONNECT stops receiving snapshots
test_snapshot_stops_after_disconnect() {
    start_server
    connected_clients A
    disconnect_client "$A"
    listen "$A" 0.2 > /dev/null
    ! listen "$A" 0.3 | grep -q "^SNAPSHOT" || fail "snapshots still received after DISCONNECT"
}

# ============================================================
# Movement
# ============================================================

# Holding RIGHT moves the player to the right
test_move_right() {
    start_server
    connected_clients A
    hold "$A" "$RIGHT" 0.4 > /dev/null
    check "$(player_x "$(last_snapshot "$A")") > 150" "the player did not move right"
}

# Holding LEFT after RIGHT moves the player back to the left
test_move_left() {
    start_server
    connected_clients A
    local before
    hold "$A" "$RIGHT" 0.5 > /dev/null
    before=$(player_x "$(last_snapshot "$A")")
    hold "$A" "$LEFT" 0.3 > /dev/null
    check "$(player_x "$(last_snapshot "$A")") < $before" "the player did not move left"
}

# Holding UP moves the player up (y goes down)
test_move_up() {
    start_server
    connected_clients A
    hold "$A" "$UP" 0.3 > /dev/null
    check "$(player_y "$(last_snapshot "$A")") < 170" "the player did not move up"
}

# Holding DOWN moves the player down (y goes up)
test_move_down() {
    start_server
    connected_clients A
    hold "$A" "$DOWN" 0.3 > /dev/null
    check "$(player_y "$(last_snapshot "$A")") > 230" "the player did not move down"
}

# Holding DOWN and RIGHT at once moves the player diagonally
test_move_diagonal() {
    start_server
    connected_clients A
    hold "$A" $((DOWN | RIGHT)) 0.3 > /dev/null
    local position
    position=$(first_xy "$(last_snapshot "$A")" "$PLAYER")
    check "${position% *} > 130 && ${position#* } > 230" "the player did not move diagonally: $position"
}

# The player moves at about 400 pixels per second
test_move_speed() {
    start_server
    connected_clients A
    hold "$A" "$RIGHT" 1 > /dev/null
    local x
    x=$(player_x "$(last_snapshot "$A" 0.05)")
    check "$x > 430 && $x < 610" "the player is at x = $x after 1 s"
}

# Sending no direction stops the player
test_move_stop() {
    start_server
    connected_clients A
    local stopped
    hold "$A" "$RIGHT" 0.2 > /dev/null
    hold "$A" 0 0.1 > /dev/null
    stopped=$(player_x "$(last_snapshot "$A")")
    hold "$A" 0 0.3 > /dev/null
    [ "$(player_x "$(last_snapshot "$A")")" = "$stopped" ] || fail "the player kept moving"
}

# UP and DOWN at the same time cancel each other
test_move_up_down_cancel() {
    start_server
    connected_clients A
    hold "$A" $((UP | DOWN)) 0.3 > /dev/null
    [ "$(player_y "$(last_snapshot "$A")")" = 200 ] || fail "the player moved vertically"
}

# LEFT and RIGHT at the same time cancel each other
test_move_left_right_cancel() {
    start_server
    connected_clients A
    hold "$A" $((LEFT | RIGHT)) 0.3 > /dev/null
    [ "$(player_x "$(last_snapshot "$A")")" = 100 ] || fail "the player moved horizontally"
}

# A player holding LEFT stops at the left edge of the screen
test_move_left_edge() {
    start_server
    connected_clients A
    hold "$A" "$LEFT" 0.8 > /dev/null
    [ "$(player_x "$(last_snapshot "$A")")" = 51.5 ] || fail "the player left the screen on the left"
}

# A player holding UP stops at the top edge of the screen
test_move_top_edge() {
    start_server
    connected_clients A
    hold "$A" "$UP" 1 > /dev/null
    [ "$(player_y "$(last_snapshot "$A")")" = 25 ] || fail "the player left the screen on the top"
}

# A player holding DOWN stops at the bottom edge of the screen
test_move_bottom_edge() {
    start_server
    connected_clients A
    hold "$A" "$DOWN" 3.2 > /dev/null
    [ "$(player_y "$(last_snapshot "$A")")" = 1055 ] || fail "the player left the screen on the bottom"
}

# An INPUT with a wrong size is ignored and does not move the player
test_move_wrong_size_input_ignored() {
    start_server
    connected_clients A
    local i
    for i in $(seq 30); do
        send_bytes "$A" 30 00 00 00 00 "$(printf '%02x' "$RIGHT")"
        sleep 0.01
    done
    [ "$(player_x "$(last_snapshot "$A")")" = 100 ] || fail "a short INPUT moved the player"
}

# Moving one player does not move the others
test_move_only_own_player() {
    start_server
    connected_clients A B
    hold "$B" "$RIGHT" 0.4 > /dev/null
    local players
    players=$(last_snapshot "$A" | tr ' ' '\n' | grep "^$PLAYER:")
    grep -q "^$PLAYER:100:200$" <<< "$players" || fail "player 0 moved: $players"
}

# ============================================================
# Shooting
# ============================================================

# Pressing fire creates a bullet
test_shoot_creates_bullet() {
    start_server
    connected_clients A
    hold "$A" "$CHARGE" 0.05 > /dev/null
    [ "$(count_type "$(last_snapshot "$A")" "$BULLET")" = 1 ] || fail "no bullet after shooting"
}

# The bullet starts in front of the player, at the same height
test_shoot_bullet_position() {
    start_server
    connected_clients A
    hold "$A" "$CHARGE" 0.05 > /dev/null
    local bullet
    bullet=$(first_xy "$(last_snapshot "$A")" "$BULLET")
    check "${bullet% *} >= 150 && ${bullet#* } == 200" "bullet at $bullet"
}

# The bullet flies to the right
test_shoot_bullet_moves_right() {
    start_server
    connected_clients A
    local start
    hold "$A" "$CHARGE" 0.05 > /dev/null
    start=$(first_xy "$(last_snapshot "$A")" "$BULLET" | cut -d' ' -f1)
    hold "$A" 0 0.2 > /dev/null
    check "$(first_xy "$(last_snapshot "$A")" "$BULLET" | cut -d' ' -f1) > $start + 50" "the bullet did not move right"
}

# Without sending any input, no bullet appears
test_shoot_no_input_no_bullet() {
    start_server
    connected_clients A
    [ "$(count_type "$(last_snapshot "$A" 0.5)" "$BULLET")" = 0 ] || fail "a bullet appeared without input"
}

# Holding fire shoots at most one bullet every 0.2 s
test_shoot_cooldown() {
    start_server
    connected_clients A
    hold "$A" "$CHARGE" 1.2 > /dev/null
    local bullets
    bullets=$(count_type "$(last_snapshot "$A")" "$BULLET")
    check "$bullets >= 5 && $bullets <= 7" "$bullets bullets after holding fire 1.2 s"
}

# A bullet shot after moving up starts at the new height of the player
test_shoot_follows_player_height() {
    start_server
    connected_clients A
    hold "$A" "$UP" 0.3 > /dev/null
    hold "$A" 0 0.3 > /dev/null
    local y
    y=$(player_y "$(last_snapshot "$A")")
    hold "$A" "$CHARGE" 0.05 > /dev/null
    [ "$(last_xy "$(last_snapshot "$A")" "$BULLET" | cut -d' ' -f2)" = "$y" ] || fail "the bullet is not at the player height"
}

# Two players can shoot at the same time
test_shoot_two_players() {
    start_server
    connected_clients A B
    send_input "$A" "$CHARGE"
    send_input "$B" "$CHARGE"
    sleep 0.1
    [ "$(count_type "$(last_snapshot "$A")" "$BULLET")" = 2 ] || fail "not 2 bullets"
}

# A bullet that leaves the screen on the right is destroyed: every input shoots, so the farthest
# bullet must stay near the right edge instead of flying away
test_shoot_bullet_despawns() {
    start_server
    connected_clients A
    hold "$A" "$CHARGE" 0.05 > /dev/null
    hold "$A" 0 4.3 > /dev/null
    local farthest
    farthest=$(last_snapshot "$A" | tr ' ' '\n' | awk -F: -v type="$BULLET" '$1 == type && $2 > max { max = $2 } END { print max + 0 }')
    check "$farthest > 1500 && $farthest < 2100" "the farthest bullet is at x = $farthest"
}

# ============================================================
# Disconnect and timeout
# ============================================================

# A DISCONNECT is announced to the other players with PLAYER_LEFT
test_disconnect_notifies_others() {
    start_server
    connected_clients A B
    disconnect_client "$B"
    listen "$A" 0.3 | grep -q "^LEFT 1" || fail "player 0 was not told that player 1 left"
}

# The player who sent DISCONNECT also receives its own PLAYER_LEFT
test_disconnect_notifies_leaver() {
    start_server
    connected_clients A
    disconnect_client "$A"
    listen "$A" 0.3 | grep -q "^LEFT 0" || fail "the leaver got no PLAYER_LEFT"
}

# A disconnected player disappears from the snapshots
test_disconnect_removes_player() {
    start_server
    connected_clients A B
    disconnect_client "$B"
    sleep 0.1
    [ "$(count_type "$(last_snapshot "$A")" "$PLAYER")" = 1 ] || fail "the player is still in the game"
}

# The slot of a disconnected player is free: a full server accepts a new client
test_disconnect_frees_slot() {
    start_server
    connected_clients A B C D
    disconnect_client "$C"
    sleep 0.1
    open_client E
    [ "$(connect_client "$E")" = "ACCEPT 2" ] || fail "the slot of player 2 was not freed"
}

# A client that left cannot come back from the same address
test_disconnect_cannot_reconnect() {
    start_server
    connected_clients A
    disconnect_client "$A"
    sleep 0.1
    [[ "$(connect_client "$A")" == REJECT* ]] || fail "a client that left was accepted again"
}

# A client silent for more than 2 s is dropped and the others are told
test_timeout_silent_client_dropped() {
    start_server
    connected_clients A B
    hold "$B" 0 2.6 | grep -q "^LEFT 0" || fail "the silent player 0 was not dropped"
}

# The server prints when a player times out
test_timeout_logged() {
    start_server
    connected_clients A B
    hold "$B" 0 2.6 > /dev/null
    grep -q "Player 0 timed out" "$SERVER_LOG" || fail "no timeout log"
}

# A client silent for only 1 s is not dropped
test_timeout_not_too_early() {
    start_server
    connected_clients A B
    ! hold "$B" 0 1.2 | grep -q "^LEFT 0" || fail "player 0 was dropped after 1 s"
}

# The body of a crashed (silent) player is removed from the game
test_timeout_body_removed() {
    start_server
    connected_clients A B
    hold "$B" 0 2.6 > /dev/null
    [ "$(count_type "$(last_snapshot "$B")" "$PLAYER")" = 1 ] || fail "the silent player is still in the game"
}

# A client that keeps sending inputs is never dropped
test_timeout_active_client_kept() {
    start_server
    connected_clients A
    ! hold "$A" 0 3 | grep -q "^LEFT" || fail "an active player was dropped"
    [ "$(count_type "$(last_snapshot "$A")" "$PLAYER")" = 1 ] || fail "the active player disappeared"
}

# The slot of a timed out player is given to the next client
test_timeout_frees_slot() {
    start_server
    connected_clients A B C D
    local pids=()
    hold "$B" 0 2.6 > /dev/null &
    pids+=($!)
    hold "$C" 0 2.6 > /dev/null &
    pids+=($!)
    hold "$D" 0 2.6 > /dev/null
    wait "${pids[@]}"
    open_client E
    [ "$(connect_client "$E")" = "ACCEPT 0" ] || fail "the slot of the silent player 0 was not freed"
}

# ============================================================
# Game
# ============================================================

# Two players passing through each other are not eliminated
test_game_players_overlap() {
    start_server
    connected_clients A B
    ! hold "$B" "$UP" 0.4 | grep -q "^LEFT" || fail "a player was eliminated by another player"
    [ "$(count_type "$(last_snapshot "$A")" "$PLAYER")" = 2 ] || fail "a player disappeared"
}

# Enemies appear on the right of the screen within 6 seconds
test_game_enemies_spawn() {
    start_server
    connected_clients A
    local tries=12 enemy=""
    while [ -z "$enemy" ] && (( tries-- > 0 )); do
        enemy=$(first_xy "$(hold "$A" 0 0.5 | grep '^SNAPSHOT' | tail -n 1)" "$ENEMY")
    done
    [ -n "$enemy" ] || fail "no enemy after 6 s"
    check "${enemy% *} > 1800" "the first enemy is at x = ${enemy% *}"
}

# Enemies fly to the left
test_game_enemies_move_left() {
    start_server
    connected_clients A
    local tries=12 enemy=""
    while [ -z "$enemy" ] && (( tries-- > 0 )); do
        enemy=$(first_xy "$(hold "$A" 0 0.5 | grep '^SNAPSHOT' | tail -n 1)" "$ENEMY")
    done
    [ -n "$enemy" ] || fail "no enemy after 6 s"
    hold "$A" 0 0.3 > /dev/null
    check "$(first_xy "$(last_snapshot "$A")" "$ENEMY" | cut -d' ' -f1) < ${enemy% *}" "the enemy did not move left"
}

# Enemies come in waves of several enemies
test_game_enemy_waves() {
    start_server
    connected_clients A
    local most=0 count line
    while read -r line; do
        count=$(count_type "$line" "$ENEMY")
        (( count > most )) && most=$count
    done < <(hold "$A" 0 7 | grep '^SNAPSHOT')
    check "$most >= 3" "at most $most enemies at once in 7 s"
}

# ============================================================
# Robustness
# ============================================================

# An unknown packet from a connected client is ignored
test_robust_unknown_packet() {
    start_server
    connected_clients A
    send_bytes "$A" 55 01 02 03
    sleep 0.1
    is_running "$SERVER_PID" || fail "the server stopped"
    [ -n "$(last_snapshot "$A")" ] || fail "no more snapshots"
}

# A one byte garbage packet does not stop the server
test_robust_one_byte_packet() {
    start_server
    open_client A
    send_bytes "$A" ff
    sleep 0.1
    is_running "$SERVER_PID" || fail "the server stopped"
    open_client B
    [ "$(connect_client "$B")" = "ACCEPT 0" ] || fail "the server no longer accepts clients"
}

# A packet bigger than the server buffer does not stop the server
test_robust_huge_packet() {
    start_server
    connected_clients A
    head -c 1500 /dev/urandom >&"$A"
    sleep 0.1
    is_running "$SERVER_PID" || fail "the server stopped"
}

# A flood of random packets does not stop the server
test_robust_garbage_flood() {
    start_server
    open_client A
    local i
    for i in $(seq 300); do
        head -c $((i % 40 + 1)) /dev/urandom >&"$A"
    done
    sleep 0.2
    is_running "$SERVER_PID" || fail "the server stopped"
    open_client B
    [[ "$(connect_client "$B")" == ACCEPT* ]] || fail "the server no longer accepts clients"
}

# A flood of inputs from a connected client does not stop the server
test_robust_input_flood() {
    start_server
    connected_clients A
    local i
    for i in $(seq 1000); do
        send_input "$A" "$RIGHT"
    done
    sleep 0.1
    is_running "$SERVER_PID" || fail "the server stopped"
}

# The server keeps running when a client socket disappears without DISCONNECT
test_robust_client_vanishes() {
    start_server
    connected_clients A B
    exec {A}>&-
    hold "$B" 0 0.5 > /dev/null
    is_running "$SERVER_PID" || fail "the server stopped"
}

# The server still accepts clients after many players came and left
test_robust_many_sessions() {
    local round
    start_server
    for round in 1 2 3 4 5 6; do
        open_client "S$round"
        local name="S$round"
        [[ "$(connect_client "${!name}")" == ACCEPT* ]] || fail "client $round refused"
        disconnect_client "${!name}"
        sleep 0.05
    done
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

# The real client stops with code 0 and a timeout message when the server stops
test_client_server_stops() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 0 connected" 5 || fail "the client did not connect"
    stop_server INT
    wait_exit "$CLIENT_PID" 5; local code=$?
    [ "$code" -eq 0 ] || fail "client exit code $code"
    grep -q "Timeout: No snapshot received" "$CLIENT_LOG" || fail "no timeout message"
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

# The real client stops with a message when the server is full
test_client_rejected_when_full() {
    need_display
    start_server
    connected_clients A B C D
    hold "$A" 0 6 > /dev/null &
    hold "$B" 0 6 > /dev/null &
    hold "$C" 0 6 > /dev/null &
    hold "$D" 0 6 > /dev/null &
    start_client
    wait_exit "$CLIENT_PID" 5
    grep -q "server full" "$CLIENT_LOG" || fail "no 'server full' message: $(cat "$CLIENT_LOG")"
}

# A real client killed without warning is dropped by the server after the timeout
test_client_killed_is_dropped() {
    need_display
    start_server
    start_client
    wait_log "$SERVER_LOG" "Player 0 connected" 5 || fail "the client did not connect"
    kill -9 "$CLIENT_PID"
    wait_log "$SERVER_LOG" "Player 0 timed out" 4 || fail "the killed client was not dropped"
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
run_test "stops with clients connected" test_server_shutdown_with_clients
run_test "port reusable after stop" test_server_port_reusable

section "Connection"
run_test "first client gets id 0" test_connect_first_client_id_0
run_test "logs the new player" test_connect_logs_player
run_test "second client gets id 1" test_connect_second_client_id_1
run_test "four clients get ids 0 to 3" test_connect_four_clients
run_test "fifth client rejected" test_connect_fifth_rejected
run_test "wrong version rejected" test_connect_wrong_version
run_test "version 0 rejected" test_connect_version_zero
run_test "right version after a wrong one" test_connect_after_wrong_version
run_test "CONNECT too short ignored" test_connect_too_short_ignored
run_test "CONNECT too long ignored" test_connect_too_long_ignored
run_test "CONNECT twice keeps the id" test_connect_twice_same_id
run_test "INPUT before CONNECT rejected" test_connect_input_before_connect
run_test "DISCONNECT before CONNECT rejected" test_connect_disconnect_before_connect
run_test "unknown packet before CONNECT rejected" test_connect_unknown_packet_before_connect
run_test "lowest free id reused" test_connect_lowest_free_id

section "Snapshots"
run_test "snapshot after connect" test_snapshot_after_connect
run_test "snapshots well formed" test_snapshot_well_formed
run_test "one player in the snapshot" test_snapshot_one_player
run_test "first spawn point" test_snapshot_first_spawn_point
run_test "four spawn points" test_snapshot_spawn_points
run_test "every client sees every player" test_snapshot_all_clients_see_all_players
run_test "about 60 snapshots per second" test_snapshot_rate
run_test "ticks increase" test_snapshot_ticks_increase
run_test "only known entity types" test_snapshot_known_types
run_test "no snapshot to strangers" test_snapshot_not_sent_to_strangers
run_test "no snapshot to rejected clients" test_snapshot_not_sent_to_rejected
run_test "no snapshot to wrong version" test_snapshot_not_sent_to_wrong_version
run_test "no snapshot after DISCONNECT" test_snapshot_stops_after_disconnect

section "Movement"
run_test "move right" test_move_right
run_test "move left" test_move_left
run_test "move up" test_move_up
run_test "move down" test_move_down
run_test "move diagonally" test_move_diagonal
run_test "speed about 400 px/s" test_move_speed
run_test "no input stops the player" test_move_stop
run_test "UP + DOWN cancel" test_move_up_down_cancel
run_test "LEFT + RIGHT cancel" test_move_left_right_cancel
run_test "left edge" test_move_left_edge
run_test "top edge" test_move_top_edge
run_test "bottom edge" test_move_bottom_edge
run_test "wrong size INPUT ignored" test_move_wrong_size_input_ignored
run_test "only the own player moves" test_move_only_own_player

section "Shooting"
run_test "fire creates a bullet" test_shoot_creates_bullet
run_test "bullet starts in front of the player" test_shoot_bullet_position
run_test "bullet moves right" test_shoot_bullet_moves_right
run_test "no input, no bullet" test_shoot_no_input_no_bullet
run_test "fire cooldown" test_shoot_cooldown
run_test "bullet at the player height" test_shoot_follows_player_height
run_test "two players shoot" test_shoot_two_players
run_test "bullet destroyed off screen" test_shoot_bullet_despawns

section "Disconnect and timeout"
run_test "DISCONNECT notifies the others" test_disconnect_notifies_others
run_test "DISCONNECT notifies the leaver" test_disconnect_notifies_leaver
run_test "DISCONNECT removes the player" test_disconnect_removes_player
run_test "DISCONNECT frees the slot" test_disconnect_frees_slot
run_test "left client cannot come back" test_disconnect_cannot_reconnect
run_test "silent client dropped" test_timeout_silent_client_dropped
run_test "timeout logged" test_timeout_logged
run_test "no timeout after 1 s" test_timeout_not_too_early
run_test "crashed player body removed" test_timeout_body_removed
run_test "active client kept" test_timeout_active_client_kept
run_test "timeout frees the slot" test_timeout_frees_slot

section "Game"
run_test "players overlap without dying" test_game_players_overlap
run_test "enemies spawn" test_game_enemies_spawn
run_test "enemies move left" test_game_enemies_move_left
run_test "enemy waves" test_game_enemy_waves

section "Robustness"
run_test "unknown packet ignored" test_robust_unknown_packet
run_test "one byte packet" test_robust_one_byte_packet
run_test "huge packet" test_robust_huge_packet
run_test "garbage flood" test_robust_garbage_flood
run_test "input flood" test_robust_input_flood
run_test "client vanishes" test_robust_client_vanishes
run_test "many sessions" test_robust_many_sessions

section "Real client"
run_test "client connects" test_client_connects
run_test "client stays alive" test_client_stays_alive
run_test "client stops when the server stops" test_client_server_stops
run_test "client logs its connection" test_client_logs_connection
run_test "client rejected when full" test_client_rejected_when_full
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
