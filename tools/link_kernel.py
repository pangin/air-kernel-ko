#!/usr/bin/env python3
"""link_kernel.py — stdlib-only replacement for the Windows-only Link_kernel_image.exe.

Pads the kernel .gba to exactly 0x100000 (1 MiB) with 0xFF and appends
image.bin verbatim, producing ezairkernel.bin (image.bin is linked at
0x08100000, i.e. immediately after the 1 MiB kernel slot).

Usage:
    python3 tools/link_kernel.py [kernel.gba] [image.bin] [-o ezairkernel.bin]

Defaults (relative to the repo root, i.e. the parent of this script's dir):
    kernel : air-kernel-ko.gba   (Makefile names TARGET after the directory)
    image  : image.bin
    output : ezairkernel.bin
"""

import argparse
import os
import sys

KERNEL_LIMIT = 0x100000  # hard limit: 1 MiB; image.bin is linked at 0x08100000

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Pad kernel .gba to 1 MiB and append image.bin "
                    "(replacement for Link_kernel_image.exe)."
    )
    parser.add_argument(
        "kernel", nargs="?",
        default=os.path.join(REPO_ROOT, "air-kernel-ko.gba"),
        help="kernel .gba (default: <repo>/air-kernel-ko.gba)",
    )
    parser.add_argument(
        "image", nargs="?",
        default=os.path.join(REPO_ROOT, "image.bin"),
        help="image.bin (default: <repo>/image.bin)",
    )
    parser.add_argument(
        "-o", "--output",
        default=os.path.join(REPO_ROOT, "ezairkernel.bin"),
        help="output file (default: <repo>/ezairkernel.bin)",
    )
    args = parser.parse_args()

    for path, label in ((args.kernel, "kernel"), (args.image, "image")):
        if not os.path.isfile(path):
            print(f"error: {label} file not found: {path}", file=sys.stderr)
            return 1

    with open(args.kernel, "rb") as f:
        kernel = f.read()
    with open(args.image, "rb") as f:
        image = f.read()

    kernel_size = len(kernel)
    if kernel_size > KERNEL_LIMIT:
        over = kernel_size - KERNEL_LIMIT
        print(
            f"error: kernel is too big: {kernel_size} bytes "
            f"(0x{kernel_size:X}) exceeds the hard limit 0x{KERNEL_LIMIT:X} "
            f"(1 MiB) by {over} bytes (0x{over:X}).\n"
            f"image.bin is linked at 0x08100000 — the kernel MUST fit in 1 MiB.",
            file=sys.stderr,
        )
        return 1

    padding = KERNEL_LIMIT - kernel_size
    total = KERNEL_LIMIT + len(image)

    with open(args.output, "wb") as f:
        f.write(kernel)
        f.write(b"\xFF" * padding)
        f.write(image)

    out_size = os.path.getsize(args.output)
    assert out_size == total, (
        f"output size mismatch: wrote {out_size}, expected {total}"
    )

    print("link_kernel.py summary")
    print("-" * 58)
    print(f"  kernel   : {args.kernel}")
    print(f"             {kernel_size:>9,} bytes (0x{kernel_size:X})")
    print(f"  headroom : {padding:>9,} bytes free to 1 MiB limit "
          f"(0x{padding:X})")
    print(f"  image    : {args.image}")
    print(f"             {len(image):>9,} bytes (0x{len(image):X})")
    print(f"  output   : {args.output}")
    print(f"             {total:>9,} bytes (0x{total:X}) "
          f"= 0x{KERNEL_LIMIT:X} + 0x{len(image):X}")
    print("-" * 58)
    print("OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
