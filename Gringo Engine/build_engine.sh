#!/usr/bin/env bash
set -euo pipefail

ENGINE_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

cmake -S "$ENGINE_DIR" -B "$ENGINE_DIR/build"
cmake --build "$ENGINE_DIR/build" --target Gringo
