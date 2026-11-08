#!/usr/bin/env bash
# Usage: ./netloss.sh [loss%] [port] [delay_ms] [jitter_ms]
# Simulates a bad network on localhost for the game port only (both directions).
# Runs until Ctrl+C, then removes the rule. Linux only, needs sudo.
# Example: ./netloss.sh 20 4242 40 10

LOSS="${1:-10}"
PORT="${2:-4242}"
DELAY="${3:-0}"
JITTER="${4:-0}"

if ! command -v tc >/dev/null; then
    echo "tc not found (install iproute2)" >&2
    exit 1
fi

cleanup() {
    trap - INT TERM EXIT
    sudo tc qdisc del dev lo root 2>/dev/null
    echo
    echo "Network simulation removed"
    exit 0
}

# Clear a leftover rule from a previous run that was killed
sudo tc qdisc del dev lo root 2>/dev/null

trap cleanup INT TERM EXIT

sudo tc qdisc add dev lo root handle 1: prio || exit 1
sudo tc qdisc add dev lo parent 1:3 handle 30: netem loss "${LOSS}%" delay "${DELAY}ms" "${JITTER}ms" || exit 1
sudo tc filter add dev lo parent 1: protocol ip u32 match ip dport "$PORT" 0xffff flowid 1:3 || exit 1
sudo tc filter add dev lo parent 1: protocol ip u32 match ip sport "$PORT" 0xffff flowid 1:3 || exit 1

echo "Port $PORT: ${LOSS}% loss, ${DELAY}ms ± ${JITTER}ms delay. Ctrl+C to stop."
while true; do
    sleep 1
done
