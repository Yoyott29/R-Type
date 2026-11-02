#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

cmake -S "$ROOT_DIR" -B "$ROOT_DIR/build" \
    -DBUILD_GRINGO=OFF \
    -DBUILD_RTYPE=ON
cmake --build "$ROOT_DIR/build" --target r-type_client r-type_server
