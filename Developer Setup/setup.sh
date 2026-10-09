#!/usr/bin/env bash
# Usage: ./collaborators/setup.sh
# Supported: Linux, and Windows (run from Git Bash).
# - installs clang-format (pinned version, same as CI) via pip/pipx
# - installs clang-tidy via the system package manager
# - creates .vscode/settings.json (format on save)
# - enables the repository git hooks

set -euo pipefail

# Keep in sync with .github/workflows/tests.yml
CLANG_FORMAT_VERSION="21.1.8"

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

case "$(uname -s)" in
    Linux*)                OS="linux" ;;
    MINGW*|MSYS*|CYGWIN*)  OS="windows" ;;
    *)
        echo "Unsupported OS: $(uname -s). Only Linux and Windows are supported." >&2
        exit 1
        ;;
esac

SUDO=""
if [ "$OS" = "linux" ] && [ "$(id -u)" -ne 0 ]; then
    SUDO="sudo"
fi

# ---------- 1. clang-format (pinned version) ----------
echo "[1/4] Installing clang-format ${CLANG_FORMAT_VERSION}..."

current_cf=""
if command -v clang-format >/dev/null 2>&1; then
    current_cf="$(clang-format --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1 || true)"
fi

if [ "$current_cf" = "$CLANG_FORMAT_VERSION" ]; then
    echo "  clang-format ${CLANG_FORMAT_VERSION} already installed, skipping."
else
    PY=""
    for candidate in python3 python; do
        if command -v "$candidate" >/dev/null 2>&1; then PY="$candidate"; break; fi
    done

    if command -v pipx >/dev/null 2>&1; then
        pipx install --force "clang-format==${CLANG_FORMAT_VERSION}"
    elif [ -n "$PY" ]; then
        "$PY" -m pip install --user "clang-format==${CLANG_FORMAT_VERSION}" \
            || "$PY" -m pip install --user --break-system-packages "clang-format==${CLANG_FORMAT_VERSION}"
    else
        echo "  Python (or pipx) not found." >&2
        echo "  Install Python 3, then re-run, or install clang-format ${CLANG_FORMAT_VERSION} manually." >&2
        exit 1
    fi

    hash -r
    new_cf=""
    if command -v clang-format >/dev/null 2>&1; then
        new_cf="$(clang-format --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1 || true)"
    fi
    if [ "$new_cf" != "$CLANG_FORMAT_VERSION" ]; then
        echo
        echo "  clang-format ${CLANG_FORMAT_VERSION} is installed but your shell finds version '${new_cf:-none}'."
        if [ "$OS" = "linux" ]; then
            echo "  Add ~/.local/bin to the START of your PATH, then restart your terminal and VS Code."
        else
            echo "  Add the Python 'Scripts' folder to the START of your PATH, then restart your terminal and VS Code."
        fi
    fi
fi

# ---------- 2. clang-tidy ----------
echo "[2/4] Installing clang-tidy..."

if command -v clang-tidy >/dev/null 2>&1; then
    echo "  Already installed, skipping."
elif [ "$OS" = "linux" ]; then
    if command -v apt-get >/dev/null 2>&1; then
        $SUDO apt-get update
        $SUDO apt-get install -y --no-install-recommends clang-tidy
    elif command -v dnf >/dev/null 2>&1; then
        $SUDO dnf install -y clang-tools-extra
    elif command -v pacman >/dev/null 2>&1; then
        $SUDO pacman -S --needed --noconfirm clang
    else
        echo "  No supported package manager (apt, dnf, pacman) found." >&2
        echo "  Install clang-tidy manually, then re-run." >&2
        exit 1
    fi
else
    # Windows: the LLVM package ships clang-tidy
    if command -v winget >/dev/null 2>&1; then
        winget install -e --id LLVM.LLVM \
            --accept-source-agreements --accept-package-agreements
    elif command -v choco >/dev/null 2>&1; then
        choco install -y llvm
    else
        echo "  Neither winget nor choco found." >&2
        echo "  Install LLVM from https://github.com/llvm/llvm-project/releases," >&2
        echo "  tick 'Add LLVM to the system PATH', then re-run." >&2
        exit 1
    fi

    if ! command -v clang-tidy >/dev/null 2>&1; then
        echo
        echo "  LLVM is installed but not on your PATH yet."
        echo "  Add 'C:\\Program Files\\LLVM\\bin' to your PATH (AFTER the Python Scripts folder,"
        echo "  so the pinned clang-format wins), then restart your terminal and VS Code."
    fi
fi

# ---------- 3. Create .vscode/settings.json ----------
echo "[3/4] Creating .vscode/settings.json..."

mkdir -p .vscode
if [ -f .vscode/settings.json ]; then
    cp .vscode/settings.json .vscode/settings.json.bak
    echo "  Existing file backed up to .vscode/settings.json.bak"
fi

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

# ---------- 4. Enable git hooks ----------
echo "[4/4] Enabling git hooks (commit message check)..."
git config core.hooksPath .githooks
chmod +x .githooks/* 2>/dev/null || true

echo
echo "Done."
echo "Make sure the 'C/C++' extension (ms-vscode.cpptools) is installed in VS Code."