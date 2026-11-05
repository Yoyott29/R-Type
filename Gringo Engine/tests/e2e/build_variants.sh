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

rm -f "$F"/NetDemo/netdemo*
check "net builds"            "$G" --build "$F/NetDemo/NetDemo.gringo"
check "net -> server"         test -x "$F/NetDemo/netdemo_server"
check "net -> client"         test -x "$F/NetDemo/netdemo_client"
check "net has no plain exe"  test ! -e "$F/NetDemo/netdemo"
# A fixture executable finds its plugins in <its folder>/plugins and the engine library through
# LD_LIBRARY_PATH: its RUNPATH only covers projects next to "Gringo Engine"
ln -sfn "$PWD/Gringo Engine/plugins" "$F/NetDemo/plugins"
export LD_LIBRARY_PATH="Gringo Engine${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
check "server runs 3 s"       test "$(code timeout 3 "$F/NetDemo/netdemo_server" 47042)" = 124
"$F/NetDemo/netdemo_server" 47043 >/dev/null 2>&1 & pid=$!; sleep 3
cpu=100   # a server that already exited fails the check
if kill -0 "$pid" 2>/dev/null; then
    ticks=$(awk '{print $14 + $15}' "/proc/$pid/stat"); kill "$pid"
    cpu=$(( ticks * 100 / ($(getconf CLK_TCK) * 3) ))
fi
check "server < 20% CPU"      test "$cpu" -lt 20
check "r-type builds"         "$G" --build R-Type/R-Type.gringo
check "r-type_server"         test -x R-Type/r-type_server
check "r-type_client"         test -x R-Type/r-type_client

exit "$failed"
