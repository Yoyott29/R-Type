#!/usr/bin/env bash
# Run from the repo root: "Gringo Engine/tests/e2e/build_variants.sh"
set -u

failed=0
check() {
    local name="$1"
    shift
    if "$@" >/dev/null 2>&1; then
        echo "PASS: $name"
    else
        echo "FAIL: $name"
        failed=1
    fi
}
code() { "$@" >/dev/null 2>&1; echo $?; }

"Gringo Engine/build_engine.sh" || exit 1
G="Gringo Engine/Gringo"
F="Gringo Engine/tests/fixtures"
rm -f "$F"/NetDemoSolo/netdemo*

check "usage error -> 84" test "$(code "$G" --build)" = 84
check "missing file -> 84" test "$(code "$G" --build "$F/Nope/Nope.gringo")" = 84
check "solo builds" "$G" --build "$F/NetDemoSolo/NetDemo.gringo"
check "solo -> netdemo" test -x "$F/NetDemoSolo/netdemo"
check "solo has no variant" test ! -e "$F/NetDemoSolo/netdemo_server"

exit "$failed"
