#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""check_lang.py — static checks for source/lang.c (UTF-8 EN/ZH/KO rewrite).

Checks (stdlib only):
  A. lang.c decodes as strict UTF-8 (no GB2312 leftovers).
  B. LoadEnglish / LoadChinese / LoadKorean assign the exact same set of
     gl_* globals (no language can miss or invent a binding).
  C. Pixel-width budgets for strings bound to width-critical globals.
     Width rule (matches DrawHZText12): each byte < 0x80 -> 6 px, each
     decoded 2/3-byte UTF-8 sequence -> 12 px.
       * settings-row globals: <= 72 px
       * help-screen globals:  <= 148 px

Usage:  python3 tests/check_lang.py [path/to/lang.c]
Exit:   0 = OK, 1 = check failures (violations table printed),
        2 = missing file / internal parse error.
"""

import os
import re
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LANG_C = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "source", "lang.c")

LOADERS = ("LoadEnglish", "LoadChinese", "LoadKorean")

SETTINGS_BUDGET = 72
HELP_BUDGET = 148

BUDGET = {}
for _n in ("gl_time", "gl_addon", "gl_reset", "gl_cheat", "gl_language",
           "gl_save", "gl_auto_save", "gl_modeB_INITstr", "gl_modeB_RUMBLE",
           "gl_modeB_LINK", "gl_set_btn", "gl_ok_btn"):
    BUDGET[_n] = SETTINGS_BUDGET
for _n in ("gl_LRSTART_help", "gl_LRSELECT_help", "gl_SELECT_help",
           "gl_L_A_help", "gl_LSTART_help", "gl_online_manual"):
    BUDGET[_n] = HELP_BUDGET


# --------------------------------------------------------------------------
def strip_comments(src):
    """Remove // and /* */ comments without touching string literals."""
    out = []
    i, n = 0, len(src)
    in_str = in_chr = False
    while i < n:
        c = src[i]
        if in_str:
            out.append(c)
            if c == "\\" and i + 1 < n:
                out.append(src[i + 1])
                i += 2
                continue
            if c == '"':
                in_str = False
            i += 1
        elif in_chr:
            out.append(c)
            if c == "\\" and i + 1 < n:
                out.append(src[i + 1])
                i += 2
                continue
            if c == "'":
                in_chr = False
            i += 1
        elif src.startswith("//", i):
            while i < n and src[i] != "\n":
                i += 1
        elif src.startswith("/*", i):
            j = src.find("*/", i + 2)
            i = n if j < 0 else j + 2
            out.append(" ")
        else:
            if c == '"':
                in_str = True
            elif c == "'":
                in_chr = True
            out.append(c)
            i += 1
    return "".join(out)


def extract_body(src, fname):
    """Return the brace-matched body of `void fname(void) { ... }`."""
    m = re.search(r"\bvoid\s+%s\s*\(\s*void\s*\)\s*\{" % re.escape(fname), src)
    if not m:
        return None
    i = m.end()  # position right after the opening '{'
    depth = 1
    in_str = in_chr = False
    start = i
    n = len(src)
    while i < n and depth > 0:
        c = src[i]
        if in_str:
            if c == "\\":
                i += 1
            elif c == '"':
                in_str = False
        elif in_chr:
            if c == "\\":
                i += 1
            elif c == "'":
                in_chr = False
        elif c == '"':
            in_str = True
        elif c == "'":
            in_chr = True
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
        i += 1
    if depth != 0:
        return None
    return src[start:i - 1]


ASSIGN_RE = re.compile(
    r"\b(gl_\w+)\s*=\s*(?:\(\s*(?:const\s+)?char\s*\*{1,2}\s*\)\s*)?(\w+)\s*;")


def assignments(body):
    """Map of gl_* -> rhs identifier inside a loader body."""
    return {m.group(1): m.group(2) for m in ASSIGN_RE.finditer(body)}


STRDEF_RE = re.compile(
    r"\b(?:static\s+)?const\s+char\s+(\w+)\s*\[[^\]]*\]\s*=\s*"
    r"((?:\"(?:\\.|[^\"\\])*\"\s*)+);")


def string_table(src):
    """Map of identifier -> raw (still escaped) literal text, with adjacent
    string literal concatenation applied."""
    table = {}
    for m in STRDEF_RE.finditer(src):
        pieces = re.findall(r"\"((?:\\.|[^\"\\])*)\"", m.group(2))
        table[m.group(1)] = "".join(pieces)
    return table


_SIMPLE_ESCAPES = {
    "n": 0x0A, "t": 0x09, "r": 0x0D, "a": 0x07, "b": 0x08,
    "f": 0x0C, "v": 0x0B, "0": 0x00, "\\": 0x5C, '"': 0x22,
    "'": 0x27, "?": 0x3F,
}


def unescape_to_bytes(lit):
    """Convert escaped C string literal text into the bytes the compiler
    would emit (source charset is UTF-8)."""
    out = bytearray()
    i, n = 0, len(lit)
    while i < n:
        c = lit[i]
        if c != "\\":
            out.extend(c.encode("utf-8"))
            i += 1
            continue
        i += 1
        if i >= n:
            break
        e = lit[i]
        if e in _SIMPLE_ESCAPES:
            out.append(_SIMPLE_ESCAPES[e])
            i += 1
        elif e == "x":
            j = i + 1
            while j < n and lit[j] in "0123456789abcdefABCDEF":
                j += 1
            if j > i + 1:
                out.append(int(lit[i + 1:j], 16) & 0xFF)
            i = j
        elif e in "01234567":
            j = i
            while j < n and j < i + 3 and lit[j] in "01234567":
                j += 1
            out.append(int(lit[i:j], 8) & 0xFF)
            i = j
        else:
            out.extend(e.encode("utf-8"))
            i += 1
    return bytes(out)


def pixel_width(data):
    """Renderer width model: <0x80 byte = 6 px; decoded 2/3-byte UTF-8
    sequence = 12 px; anything else is skipped by the renderer (0 px)."""
    w = 0
    i = 0
    strays = 0
    while i < len(data):
        b = data[i]
        if b < 0x80:
            w += 6
            i += 1
        elif 0xC0 <= b <= 0xDF and i + 1 < len(data):
            w += 12
            i += 2
        elif 0xE0 <= b <= 0xEF and i + 2 < len(data):
            w += 12
            i += 3
        else:
            strays += 1
            i += 1
    return w, strays


def preview(data, limit=28):
    try:
        s = data.decode("utf-8")
    except UnicodeDecodeError:
        s = repr(data)
    return s if len(s) <= limit else s[:limit - 1] + "…"


# --------------------------------------------------------------------------
def main():
    if not os.path.isfile(LANG_C):
        print("ERROR: %s not found" % LANG_C)
        return 2

    with open(LANG_C, "rb") as f:
        raw = f.read()

    failures = 0

    # ---- A. strict UTF-8 ----
    try:
        text = raw.decode("utf-8")
        print("ok: lang.c is strict UTF-8 (%d bytes)" % len(raw))
    except UnicodeDecodeError as e:
        line = raw[:e.start].count(b"\n") + 1
        print("FAIL: lang.c is NOT valid UTF-8 at byte %d (line %d): %s"
              % (e.start, line, e.reason))
        print("      (legacy GB2312 content must be converted)")
        return 1  # the remaining checks need decodable text

    src = strip_comments(text)

    # ---- B. loader parity ----
    bodies = {}
    for fn in LOADERS:
        body = extract_body(src, fn)
        if body is None:
            print("FAIL: function %s(void) not found in lang.c" % fn)
            failures += 1
        else:
            bodies[fn] = body
    if len(bodies) != len(LOADERS):
        print("\ncheck_lang: %d FAILURE(S)" % failures)
        return 1

    assign = {fn: assignments(bodies[fn]) for fn in LOADERS}
    all_keys = set()
    for fn in LOADERS:
        all_keys |= set(assign[fn])
    parity_ok = True
    for fn in LOADERS:
        missing = sorted(all_keys - set(assign[fn]))
        if missing:
            parity_ok = False
            failures += 1
            print("FAIL: %s misses %d binding(s): %s"
                  % (fn, len(missing), ", ".join(missing)))
    if parity_ok:
        print("ok: %s assign the identical set of %d gl_* globals"
              % ("/".join(LOADERS), len(all_keys)))

    # ---- C. width budgets ----
    table = string_table(src)
    violations = []
    checked = 0
    for fn in LOADERS:
        for glob in sorted(BUDGET):
            budget = BUDGET[glob]
            rhs = assign[fn].get(glob)
            if rhs is None:
                violations.append((fn, glob, "-", "NOT ASSIGNED", 0, budget))
                continue
            lit = table.get(rhs)
            if lit is None:
                violations.append((fn, glob, rhs, "UNRESOLVED SYMBOL", 0, budget))
                continue
            data = unescape_to_bytes(lit)
            width, strays = pixel_width(data)
            checked += 1
            if strays:
                violations.append((fn, glob, rhs,
                                   "MALFORMED UTF-8 (%d stray byte(s))" % strays,
                                   width, budget))
            elif width > budget:
                violations.append((fn, glob, rhs, preview(data), width, budget))

    print("ok: width-checked %d budgeted strings across %d loaders"
          % (checked, len(LOADERS)))

    if violations:
        failures += len(violations)
        print("\nWIDTH/RESOLUTION VIOLATIONS:")
        print("  %-12s %-18s %-18s %5s %6s  %s"
              % ("LOADER", "GLOBAL", "SYMBOL", "PX", "BUDGET", "STRING"))
        for fn, glob, rhs, s, width, budget in violations:
            print("  %-12s %-18s %-18s %5d %6d  \"%s\""
                  % (fn, glob, rhs, width, budget, s))

    print()
    if failures:
        print("check_lang: %d FAILURE(S)" % failures)
        return 1
    print("check_lang: all checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
