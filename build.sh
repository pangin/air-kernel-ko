#!/bin/bash
# build.sh — build the EZ-FLASH Air kernel (korean) and link ezairkernel.bin
set -e

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$REPO_ROOT"

KERNEL_LIMIT=$((0x100000))

# --- resolve devkitARM ---------------------------------------------------
if [ -z "$DEVKITARM" ]; then
    if [ -d /opt/devkitpro/devkitARM ]; then
        export DEVKITPRO=/opt/devkitpro
        export DEVKITARM=/opt/devkitpro/devkitARM
    else
        echo "error: devkitARM not found." >&2
        echo "Set DEVKITARM, or install devkitPro:" >&2
        echo "  sudo installer -pkg /tmp/devkitpro-pacman-installer.pkg -target /" >&2
        echo "  sudo dkp-pacman -Sy gba-dev" >&2
        echo "(installer pkg: https://github.com/devkitPro/pacman/releases/latest/download/devkitpro-pacman-installer.pkg)" >&2
        exit 1
    fi
fi
export DEVKITPRO="${DEVKITPRO:-$(dirname "$DEVKITARM")}"
echo "Using DEVKITARM=$DEVKITARM"

# --- build ---------------------------------------------------------------
make

KERNEL_GBA="$REPO_ROOT/$(basename "$REPO_ROOT").gba"   # Makefile: TARGET := $(notdir $(CURDIR))
if [ ! -f "$KERNEL_GBA" ]; then
    echo "error: expected kernel not found: $KERNEL_GBA" >&2
    exit 1
fi

# --- link kernel + image -------------------------------------------------
python3 "$REPO_ROOT/tools/link_kernel.py" "$KERNEL_GBA" "$REPO_ROOT/image.bin" -o "$REPO_ROOT/ezairkernel.bin"

# --- report --------------------------------------------------------------
KERNEL_SIZE=$(stat -f '%z' "$KERNEL_GBA" 2>/dev/null || stat -c '%s' "$KERNEL_GBA")
IMAGE_SIZE=$(stat -f '%z' "$REPO_ROOT/image.bin" 2>/dev/null || stat -c '%s' "$REPO_ROOT/image.bin")
OUT_SIZE=$(stat -f '%z' "$REPO_ROOT/ezairkernel.bin" 2>/dev/null || stat -c '%s' "$REPO_ROOT/ezairkernel.bin")

echo ""
echo "Artifacts:"
printf '  %-18s %9d bytes\n' "$(basename "$KERNEL_GBA")" "$KERNEL_SIZE"
printf '  %-18s %9d bytes\n' "image.bin" "$IMAGE_SIZE"
printf '  %-18s %9d bytes\n' "ezairkernel.bin" "$OUT_SIZE"
if [ "$KERNEL_SIZE" -gt "$KERNEL_LIMIT" ]; then
    echo "FAIL: kernel exceeds 1MB budget by $((KERNEL_SIZE - KERNEL_LIMIT)) bytes" >&2
    exit 1
fi
echo "Budget: kernel uses $KERNEL_SIZE / $KERNEL_LIMIT bytes ($((KERNEL_LIMIT - KERNEL_SIZE)) bytes free of 1MB)"
