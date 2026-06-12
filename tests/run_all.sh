#!/usr/bin/env bash
# tests/run_all.sh — full offline test battery for the Korean kernel.
#
# Runs, in order (sequential, fail-fast, summary at the end):
#   1. tests/check_fonts.py            font header structure (needs python3)
#   2. tests/check_lang.py             lang.c UTF-8 / parity / width budgets
#   3. tests/host  make test           DrawHZText12 host tests (ASan/UBSan)
#   4. scripts/selftest_link_kernel.sh link_kernel.py packaging self-test
#
# Prerequisites: python3, a host C compiler (cc/clang/gcc), make, and the
# generated font headers committed at source/font_kor.h + source/font_cjk.h.
#
# Usage: tests/run_all.sh    (no args; runs from any cwd)

set -u

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

NAMES=()
STATUSES=()
OVERALL=0

summary() {
    echo ""
    echo "==== summary ===="
    i=0
    while [ "$i" -lt "${#NAMES[@]}" ]; do
        printf '  %-36s %s\n' "${NAMES[$i]}" "${STATUSES[$i]}"
        i=$((i + 1))
    done
}

run_step() {
    name="$1"
    shift
    echo ""
    echo "==== ${name} ===="
    if "$@"; then
        NAMES+=("$name")
        STATUSES+=("PASS")
    else
        NAMES+=("$name")
        STATUSES+=("FAIL")
        OVERALL=1
        summary
        echo ""
        echo "FAIL-FAST: aborted at \"${name}\""
        exit 1
    fi
}

run_step "check_fonts.py (font headers)"      python3 "$ROOT/tests/check_fonts.py"
run_step "check_lang.py (language tables)"    python3 "$ROOT/tests/check_lang.py"
run_step "host renderer tests (ASan/UBSan)"   make -C "$ROOT/tests/host" test
run_step "link_kernel.py self-test"           bash "$ROOT/scripts/selftest_link_kernel.sh"

summary
echo ""
echo "ALL TESTS PASSED"
exit "$OVERALL"
