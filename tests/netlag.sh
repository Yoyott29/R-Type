#!/usr/bin/env bash
# Usage: ./netlag.sh [delay_ms] [jitter_ms] [port]
# Adds latency on localhost for the game port only (both directions, so ping = 2 x delay).
# Runs until Ctrl+C, then removes the rule. Linux only, needs sudo.
# Example: ./netlag.sh 100 20 4242

DELAY="${1:-100}"
JITTER="${2:-0}"
PORT="${3:-4242}"

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
sudo tc qdisc add dev lo parent 1:3 handle 30: netem delay "${DELAY}ms" "${JITTER}ms" || exit 1
sudo tc filter add dev lo parent 1: protocol ip u32 match ip dport "$PORT" 0xffff flowid 1:3 || exit 1
sudo tc filter add dev lo parent 1: protocol ip u32 match ip sport "$PORT" 0xffff flowid 1:3 || exit 1

echo "Port $PORT: ${DELAY}ms ± ${JITTER}ms delay each way. Ctrl+C to stop."
while true; do
    sleep 1
done
