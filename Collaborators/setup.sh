#!/usr/bin/env bash

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

# ---------- 1. Install clang-format and clang-tidy ----------
echo "[1/3] Installing clang-format and clang-tidy..."

if command -v clang-format >/dev/null 2>&1 && command -v clang-tidy >/dev/null 2>&1; then
    echo "  Already installed, skipping."
else
    SUDO=""
    [ "$(id -u)" -ne 0 ] && SUDO="sudo"

    if command -v apt-get >/dev/null 2>&1; then
        $SUDO apt-get update
        $SUDO apt-get install -y --no-install-recommends clang-format clang-tidy
    elif command -v dnf >/dev/null 2>&1; then
        $SUDO dnf install -y clang-tools-extra
    elif command -v pacman >/dev/null 2>&1; then
        $SUDO pacman -S --needed --noconfirm clang
    elif command -v brew >/dev/null 2>&1; then
        brew install clang-format llvm
        echo "  Note: with Homebrew, clang-tidy is in \$(brew --prefix llvm)/bin."
        echo "  Add it to your PATH so CMake can find it."
    else
        echo "  No supported package manager found." >&2
        echo "  Install clang-format and clang-tidy manually, then re-run." >&2
        exit 1
    fi
fi

# ---------- 2. Create .vscode/settings.json ----------
echo "[2/3] Creating .vscode/settings.json..."

mkdir -p .vscode

cat > .vscode/settings.json << 'JSON'
{
    "[cpp]": {
        "editor.formatOnSave": true, // Enable format on save for C++ files
        "editor.defaultFormatter": "ms-vscode.cpptools" // Use the C/C++ extension's formatter
    },
    "C_Cpp.formatting": "clangFormat", // Use clang-format for formatting
    "C_Cpp.clang_format_style": "file:${workspaceFolder}/Norms/.clang-format" // Use the .clang-format file in the Norms directory for formatting rules
}
JSON

# ---------- 3. Enable git hooks ----------
echo "[3/3] Enabling git hooks (commit message check)..."
git config core.hooksPath .githooks
chmod +x .githooks/* 2>/dev/null || true

echo
echo "Done."
echo "Make sure the 'C/C++' extension (ms-vscode.cpptools) is installed in VS Code ( if you use VS Code )."