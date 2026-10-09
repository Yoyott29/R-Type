#!/usr/bin/env bash
# Usage: ./tests.sh
# Runs every test, prints a summary, exits 1 if at least one failed.

passed=0
failed=0
failed_names=()

run_test() {
    local name="$1"; shift
    if "$@" > /tmp/test_output.log 2>&1; then
        echo "  [PASS] $name"
        passed=$((passed + 1))
    else
        echo "  [FAIL] $name"
        sed 's/^/         /' /tmp/test_output.log | tail -n 20
        failed=$((failed + 1))
        failed_names+=("$name")
    fi
}

echo "== Build =="
run_test "build" bash -c "cmake -B build && cmake --build build"

echo "== Tests =="
# Add your tests below, one line each:
#run_test "server binary exists"  test -x build/r-type_server
run_test "client binary exists"  test -x ./r-type_client

echo
echo "================================"
echo " Passed: $passed"
echo " Failed: $failed"
echo " Total:  $((passed + failed))"
echo "================================"

if [ "$failed" -ne 0 ]; then
    echo "Failed tests:"
    printf '  - %s\n' "${failed_names[@]}"
    exit 1
fi