#!/usr/bin/env bash
# scripts/selftest_link_kernel.sh — self-test for tools/link_kernel.py.
#
# tools/link_kernel.py has no built-in --self-test, so this script
# synthesizes one:
#   1. builds a deterministic 512 KiB dummy kernel
#   2. links it against the real image.bin (1,074,848 bytes)
#   3. asserts: output == 2,123,424 bytes, kernel bytes intact, 0xFF padding
#      up to the 0x100000 boundary, image.bin appended verbatim
#   4. asserts the >1 MiB hard-limit rejection path exits non-zero
#
# Usage: scripts/selftest_link_kernel.sh   (no args; runs from any cwd)

set -eu

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

echo "== link_kernel.py self-test =="

# 1. deterministic 512 KiB dummy kernel
python3 - "$TMP/dummy.gba" <<'PY'
import sys
open(sys.argv[1], "wb").write(bytes(range(256)) * 2048)  # 0x80000 bytes
PY

# 2. link
python3 "$ROOT/tools/link_kernel.py" "$TMP/dummy.gba" "$ROOT/image.bin" \
    -o "$TMP/ezairkernel.bin"

# 3. verify layout
python3 - "$TMP/dummy.gba" "$ROOT/image.bin" "$TMP/ezairkernel.bin" <<'PY'
import sys
dummy, image, out = (open(p, "rb").read() for p in sys.argv[1:4])
assert len(image) == 1074848, "image.bin is %d bytes, expected 1074848" % len(image)
assert len(out) == 2123424, "output is %d bytes, expected 2123424" % len(out)
assert out[:len(dummy)] == dummy, "kernel bytes corrupted in output"
assert out[len(dummy):0x100000] == b"\xFF" * (0x100000 - len(dummy)), \
    "padding to 0x100000 is not all 0xFF"
assert out[0x100000:] == image, "image.bin tail does not match"
print("layout OK: kernel | 0xFF padding to 0x100000 | image.bin (tail verified)")
PY

# 4. over-limit kernel must be rejected
python3 - "$TMP/too_big.gba" <<'PY'
import sys
open(sys.argv[1], "wb").write(b"\x00" * (0x100000 + 1))
PY
if python3 "$ROOT/tools/link_kernel.py" "$TMP/too_big.gba" "$ROOT/image.bin" \
    -o "$TMP/should_not_exist.bin" >/dev/null 2>&1; then
    echo "FAIL: link_kernel.py accepted a kernel larger than 0x100000" >&2
    exit 1
fi
echo "over-limit rejection OK (kernel > 1 MiB refused)"

echo "link_kernel.py self-test PASSED"
