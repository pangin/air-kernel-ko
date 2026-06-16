#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""check_fonts.py — structural validation of the generated font headers.

Validates (stdlib only, no compiler needed):
  source/font_kor.h
    * KOR_FIRST == 0xAC00, KOR_LAST == 0xD7A3, KOR_COUNT == 11172
    * KOR_GLYPHS holds exactly KOR_COUNT*24 == 268128 byte literals
    * glyph row contract: byte1 & 0x0F == 0 for every row (cols 12-15 empty)
    * the U+AC00 ('가') glyph is not blank
  source/font_cjk.h
    * CJK_INDEX: length == CJK_COUNT, strictly ascending, all <= 0xFFFF
    * CJK_GLYPHS holds exactly CJK_COUNT*24 byte literals
    * same byte1 & 0x0F == 0 row contract

Usage:  python3 tests/check_fonts.py [kernel-repo-root]
Exit:   0 = OK, 1 = check failures, 2 = missing files / parse errors.
"""

import os
import re
import sys

REPO = sys.argv[1] if len(sys.argv) > 1 else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

KOR_EXPECT_FIRST = 0xAC00
KOR_EXPECT_LAST = 0xD7A3
KOR_EXPECT_COUNT = 11172

_failures = []


def fail(msg):
    _failures.append(msg)
    print("FAIL: %s" % msg)


def ok(msg):
    print("  ok: %s" % msg)


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    return text


def read_header(name):
    path = os.path.join(REPO, "source", name)
    if not os.path.isfile(path):
        print("ERROR: %s not found — generate it with the "
              "ezflash-kernel-font-generator repo first." % path)
        sys.exit(2)
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return strip_comments(f.read())


def get_define(text, name, where):
    m = re.search(r"#define\s+%s\s+(0[xX][0-9A-Fa-f]+|\d+)\b" % re.escape(name), text)
    if not m:
        print("ERROR: #define %s not found in %s" % (name, where))
        sys.exit(2)
    return int(m.group(1), 0)


def get_array(text, name, where):
    """Extract the integer literals of a flat array initializer."""
    m = re.search(r"\b%s\s*\[[^\]]*\]\s*=\s*\{" % re.escape(name), text)
    if not m:
        print("ERROR: array %s not found in %s" % (name, where))
        sys.exit(2)
    end = text.find("}", m.end())
    if end < 0:
        print("ERROR: unterminated initializer for %s in %s" % (name, where))
        sys.exit(2)
    toks = re.findall(r"0[xX][0-9A-Fa-f]+|\d+", text[m.end():end])
    return [int(t, 0) for t in toks]


def check_row_contract(glyphs, name):
    """Every glyph row is 2 bytes; the low nibble of byte1 (cols 12-15)
    must be zero."""
    bad = []
    for pos in range(1, len(glyphs), 2):
        if glyphs[pos] & 0x0F:
            bad.append(pos)
    if bad:
        g, r = divmod(bad[0] // 2, 12)
        fail("%s: %d row(s) with non-zero cols 12-15 (byte1 & 0x0F), "
             "first at glyph %d row %d (byte offset %d)"
             % (name, len(bad), g, r, bad[0]))
    else:
        ok("%s: all rows honor the byte1 & 0x0F == 0 contract" % name)


def main():
    # ---------------- font_kor.h ----------------
    kor = read_header("font_kor.h")
    kf = get_define(kor, "KOR_FIRST", "font_kor.h")
    kl = get_define(kor, "KOR_LAST", "font_kor.h")
    kc = get_define(kor, "KOR_COUNT", "font_kor.h")

    print("font_kor.h: KOR_FIRST=0x%04X KOR_LAST=0x%04X KOR_COUNT=%d" % (kf, kl, kc))
    if (kf, kl, kc) != (KOR_EXPECT_FIRST, KOR_EXPECT_LAST, KOR_EXPECT_COUNT):
        fail("KOR defines mismatch: expected 0x%04X/0x%04X/%d"
             % (KOR_EXPECT_FIRST, KOR_EXPECT_LAST, KOR_EXPECT_COUNT))
    else:
        ok("KOR_FIRST/KOR_LAST/KOR_COUNT as expected")

    kor_glyphs = get_array(kor, "KOR_GLYPHS", "font_kor.h")
    if len(kor_glyphs) != KOR_EXPECT_COUNT * 24:
        fail("KOR_GLYPHS has %d byte literals, expected %d (= 11172*24)"
             % (len(kor_glyphs), KOR_EXPECT_COUNT * 24))
    else:
        ok("KOR_GLYPHS holds %d bytes (= 11172*24)" % len(kor_glyphs))

    if any(b < 0 or b > 0xFF for b in kor_glyphs):
        fail("KOR_GLYPHS contains literals outside 0..0xFF")

    if len(kor_glyphs) >= 24:
        if any(kor_glyphs[:24]):
            ok("U+AC00 ('가') sample glyph is non-blank")
        else:
            fail("U+AC00 ('가') glyph (KOR_GLYPHS[0..23]) is all zero")

    check_row_contract(kor_glyphs, "KOR_GLYPHS")

    # ---------------- font_cjk.h ----------------
    cjk = read_header("font_cjk.h")
    cc = get_define(cjk, "CJK_COUNT", "font_cjk.h")
    print("font_cjk.h: CJK_COUNT=%d" % cc)

    index = get_array(cjk, "CJK_INDEX", "font_cjk.h")
    glyphs = get_array(cjk, "CJK_GLYPHS", "font_cjk.h")

    if len(index) != cc:
        fail("CJK_INDEX has %d entries, expected CJK_COUNT=%d" % (len(index), cc))
    else:
        ok("CJK_INDEX length == CJK_COUNT (%d)" % cc)

    if any(v > 0xFFFF or v < 0 for v in index):
        fail("CJK_INDEX contains values outside u16 range")
    else:
        ok("CJK_INDEX values all fit u16")

    descending = [i for i in range(1, len(index)) if index[i] <= index[i - 1]]
    if descending:
        i = descending[0]
        fail("CJK_INDEX not strictly ascending: index[%d]=0x%04X >= index[%d]=0x%04X "
             "(%d violations)" % (i - 1, index[i - 1], i, index[i], len(descending)))
    else:
        ok("CJK_INDEX strictly ascending (bsearch-safe)")

    if len(glyphs) != cc * 24:
        fail("CJK_GLYPHS has %d byte literals, expected %d (= CJK_COUNT*24)"
             % (len(glyphs), cc * 24))
    else:
        ok("CJK_GLYPHS holds %d bytes (= CJK_COUNT*24)" % len(glyphs))

    if any(b < 0 or b > 0xFF for b in glyphs):
        fail("CJK_GLYPHS contains literals outside 0..0xFF")

    check_row_contract(glyphs, "CJK_GLYPHS")

    if index:
        print("CJK codepoint range: U+%04X .. U+%04X" % (index[0], index[-1]))

    print()
    if _failures:
        print("check_fonts: %d FAILURE(S)" % len(_failures))
        return 1
    print("check_fonts: all checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
