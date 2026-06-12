/* test_renderer.c — host-side unit tests for DrawHZText12 (source/draw.c).
 *
 * Build & run:   make -C tests/host test        (ASan + UBSan)
 *                make -C tests/host test-nosan  (fallback, no sanitizers)
 *
 * source/draw.c is #included below as a single translation unit so the test
 * can reach the font data (ASC_DATA / KOR_GLYPHS / CJK_INDEX / CJK_GLYPHS)
 * that draw.c pulls in via its own headers (asc126.h defines ASC_DATA with
 * external linkage, so a two-TU build would double-define it).
 *
 * The expected-pixel model below is written INDEPENDENTLY from the renderer
 * specification — it does not copy renderer internals:
 *
 *   ASCII  (<0x80) : ASC_DATA[c*12 + row]; bit 0x80 = leftmost of the 8
 *                    written columns; advance 6 px.
 *   UTF-8 2/3-byte : decoded codepoint -> 24-byte glyph (12 rows x 2 bytes);
 *                    byte0 bit 0x80 -> col 0 (v[x+0]), byte1 bit 0x80 ->
 *                    col 8 (v[x+8]); cols 12-15 always 0; advance 12 px.
 *                    Hangul U+AC00..U+D7A3 dense: KOR_GLYPHS[(cp-0xAC00)*24];
 *                    otherwise binary search of sorted CJK_INDEX.
 *   unmapped       : tofu (some non-empty mark inside the 12x12 cell),
 *                    advance 12 px.
 *
 * All tests use isDrawDirect=0 so the renderer writes to Vcache, which is
 * (u16*)pReadCache — the GBA VideoBuffer (0x6000000) is never dereferenced.
 * Screen stride is 240 u16 per row.
 */

#include <stdio.h>
#include <string.h>

/* ---- externs draw.c expects the rest of the kernel to provide --------- */
unsigned char pReadCache[0x20000] __attribute__((aligned(4)));
void wait_btn(void) {}

#include "draw.c" /* single TU: renderer + font data (found via -I../../source) */

/* ----------------------------------------------------------------------- */

#define VC ((const u16 *)pReadCache)
#define FB_W 240
#define FB_H 160
#define FB_PIXELS (FB_W * FB_H)
#define TEST_COLOR 0x7FFF

static u16 expected[FB_PIXELS];

static int g_case_fail;  /* failed checks in the current test case */
static int g_total_fail; /* failed checks overall */

#define CHECK(cond, ...)                                              \
    do {                                                              \
        if (!(cond)) {                                                \
            g_case_fail++;                                            \
            g_total_fail++;                                           \
            printf("    check failed (%s:%d): ", __FILE__, __LINE__); \
            printf(__VA_ARGS__);                                      \
            printf("\n");                                             \
        }                                                             \
    } while (0)

static void reset_buffers(void)
{
    memset(pReadCache, 0, sizeof pReadCache);
    memset(expected, 0, sizeof expected);
}

/* ---- independent expected-pixel model (from the spec, see header) ------ */

/* ASCII glyph: 12 bytes starting at ASC_DATA[ch*12]; 0x80 = leftmost col. */
static void exp_ascii(unsigned char ch, int x, int y, u16 c)
{
    int row, k;
    for (row = 0; row < 12; row++) {
        unsigned b = ASC_DATA[(unsigned)ch * 12 + row];
        for (k = 0; k < 8; k++)
            if (b & (0x80u >> k))
                expected[(y + row) * FB_W + x + k] = c;
    }
}

/* 24-byte glyph: row = 2 bytes; byte0 0x80 -> col0, byte1 0x80 -> col8. */
static void exp_glyph24(const unsigned char *g, int x, int y, u16 c)
{
    int row, k;
    for (row = 0; row < 12; row++) {
        unsigned b0 = g[row * 2];
        unsigned b1 = g[row * 2 + 1];
        for (k = 0; k < 8; k++) {
            if (b0 & (0x80u >> k))
                expected[(y + row) * FB_W + x + k] = c;
            if (b1 & (0x80u >> k))
                expected[(y + row) * FB_W + x + 8 + k] = c;
        }
    }
}

/* Independent binary search of CJK_INDEX (NOT the renderer's). */
static const unsigned char *ref_cjk_lookup(unsigned int cp)
{
    int lo = 0, hi = (int)CJK_COUNT - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (CJK_INDEX[mid] == cp)
            return &CJK_GLYPHS[mid * 24];
        if (CJK_INDEX[mid] < cp)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return 0;
}

/* Full-framebuffer comparison against the expected model. */
static void compare_fb(const char *what)
{
    int i, bad = 0;
    for (i = 0; i < FB_PIXELS; i++) {
        if (VC[i] != expected[i]) {
            if (bad < 4)
                printf("    mismatch %s: pixel (%d,%d) got 0x%04X want 0x%04X\n",
                       what, i % FB_W, i / FB_W, VC[i], expected[i]);
            bad++;
        }
    }
    CHECK(bad == 0, "%s: %d mismatching pixel(s)", what, bad);
}

static int region_nonzero(int x, int y, int w, int h)
{
    int row, col, n = 0;
    for (row = 0; row < h; row++)
        for (col = 0; col < w; col++)
            if (VC[(y + row) * FB_W + x + col] != 0)
                n++;
    return n;
}

/* ======================= test cases ===================================== */

/* 1. ASCII 'A' at (0,0). */
static void test_ascii_A(void)
{
    reset_buffers();
    DrawHZText12("A", 0, 0, 0, TEST_COLOR, 0);
    exp_ascii('A', 0, 0, TEST_COLOR);
    CHECK(region_nonzero(0, 0, 8, 12) > 0, "'A' rendered nothing");
    compare_fb("ascii 'A'");
}

/* 2. Hangul '가' (U+AC00, UTF-8 EA B0 80) — first dense glyph. */
static void test_hangul_first(void)
{
    reset_buffers();
    DrawHZText12("\xEA\xB0\x80", 0, 0, 0, TEST_COLOR, 0);
    exp_glyph24(&KOR_GLYPHS[(0xAC00 - KOR_FIRST) * 24], 0, 0, TEST_COLOR);
    CHECK(region_nonzero(0, 0, 12, 12) > 0, "U+AC00 rendered nothing");
    compare_fb("hangul U+AC00");
}

/* 3. Hangul '힣' (U+D7A3, UTF-8 ED 9E A3) — last dense glyph (bounds). */
static void test_hangul_last(void)
{
    reset_buffers();
    DrawHZText12("\xED\x9E\xA3", 0, 0, 0, TEST_COLOR, 0);
    exp_glyph24(&KOR_GLYPHS[(0xD7A3 - KOR_FIRST) * 24], 0, 0, TEST_COLOR);
    CHECK(region_nonzero(0, 0, 12, 12) > 0, "U+D7A3 rendered nothing");
    compare_fb("hangul U+D7A3");
}

/* 4. CJK '中' (U+4E2D, UTF-8 E4 B8 AD) — sparse table binary search. */
static void test_cjk_zhong(void)
{
    const unsigned char *g = ref_cjk_lookup(0x4E2D);
    CHECK(g != 0, "U+4E2D missing from CJK_INDEX (HZK12-derived font must contain it)");
    if (!g)
        return;
    reset_buffers();
    DrawHZText12("\xE4\xB8\xAD", 0, 0, 0, TEST_COLOR, 0);
    exp_glyph24(g, 0, 0, TEST_COLOR);
    CHECK(region_nonzero(0, 0, 12, 12) > 0, "U+4E2D rendered nothing");
    compare_fb("cjk U+4E2D");
}

/* 5. Mixed "A가B": 'A' at cols 0-5, '가' at 6-17, 'B' at 18-23. */
static void test_mixed_advance(void)
{
    reset_buffers();
    DrawHZText12("A\xEA\xB0\x80" "B", 0, 0, 0, TEST_COLOR, 0);
    exp_ascii('A', 0, 0, TEST_COLOR);
    exp_glyph24(&KOR_GLYPHS[0], 6, 0, TEST_COLOR);
    exp_ascii('B', 18, 0, TEST_COLOR);
    /* x-advance proof: each cell must contain ink of its own glyph. */
    CHECK(region_nonzero(0, 0, 6, 12) > 0, "'A' cell (cols 0-5) empty");
    CHECK(region_nonzero(6, 0, 12, 12) > 0, "'가' cell (cols 6-17) empty");
    CHECK(region_nonzero(18, 0, 6, 12) > 0, "'B' cell (cols 18-23) empty");
    compare_fb("mixed \"A가B\"");
}

/* 6. Robustness: malformed/truncated UTF-8, clipping, right edge.
 *    Run under ASan/UBSan; pReadCache is an instrumented global, so any
 *    write before/after it aborts the test binary. */
static void test_robustness(void)
{
    char long_str[101];

    /* lone continuation byte: skipped, draws nothing */
    reset_buffers();
    DrawHZText12("\x80", 0, 0, 0, TEST_COLOR, 0);
    compare_fb("lone continuation 0x80");

    /* truncated 2-byte sequence at end of string */
    reset_buffers();
    DrawHZText12("\xC3", 0, 0, 0, TEST_COLOR, 0);
    compare_fb("truncated 2-byte 0xC3");

    /* truncated 3-byte sequence at end of string */
    reset_buffers();
    DrawHZText12("\xEA\xB0", 0, 0, 0, TEST_COLOR, 0);
    compare_fb("truncated 3-byte 0xEA 0xB0");

    /* 4-byte lead (U+1F4A9): all four bytes skipped, draws nothing */
    reset_buffers();
    DrawHZText12("\xF0\x9F\x92\xA9", 0, 0, 0, TEST_COLOR, 0);
    compare_fb("4-byte lead 0xF0 9F 92 A9");

    /* len parameter clips a longer string: only the first 10 'A's render */
    memset(long_str, 'A', 100);
    long_str[100] = '\0';
    reset_buffers();
    DrawHZText12(long_str, 10, 0, 60, TEST_COLOR, 0);
    {
        int i;
        for (i = 0; i < 10; i++)
            exp_ascii('A', i * 6, 60, TEST_COLOR);
    }
    compare_fb("len=10 clip of 100-char string");

    /* unclipped long string + right-edge probes: assert no crash only
     * (the renderer may legally bleed within the Vcache bounds) */
    reset_buffers();
    DrawHZText12(long_str, 0, 0, 80, TEST_COLOR, 0);   /* 100 chars, len=0   */
    DrawHZText12("\xEA\xB0\x80", 0, 228, 100, TEST_COLOR, 0); /* hangul @ x=228 */
    DrawHZText12("\xED\x9E\xA3" "A", 0, 236, 120, TEST_COLOR, 0); /* x=236  */
    DrawHZText12("A\xEA\xB0\x80", 0, 230, 148, TEST_COLOR, 0); /* near y max */
    CHECK(1, "unreachable"); /* reached without sanitizer abort == pass */
}

/* 7. Tofu: unmapped codepoint renders something and advances 12 px. */
static void test_tofu_advance(void)
{
    const unsigned int cp = 0x1E3A; /* Latin Ext. Additional — not in fonts */
    int bad = 0, row, col;

    CHECK(!(cp >= KOR_FIRST && cp <= KOR_LAST), "U+1E3A inside Hangul block?!");
    CHECK(ref_cjk_lookup(cp) == 0, "U+1E3A unexpectedly present in CJK_INDEX");

    reset_buffers();
    /* U+1E3A = UTF-8 E1 B8 BA, followed by 'A' to observe the advance */
    DrawHZText12("\xE1\xB8\xBA" "A", 0, 24, 24, TEST_COLOR, 0);

    /* tofu must leave some ink inside its 12x12 cell */
    CHECK(region_nonzero(24, 24, 12, 12) > 0, "tofu cell (24,24) is empty");

    /* 'A' must start exactly at x+12 = 36: compare its 8-column cell */
    exp_ascii('A', 36, 24, TEST_COLOR);
    for (row = 0; row < 12; row++)
        for (col = 0; col < 8; col++) {
            int idx = (24 + row) * FB_W + 36 + col;
            if (VC[idx] != expected[idx])
                bad++;
        }
    CHECK(bad == 0, "'A' after tofu not at x=36 (advance != 12): %d bad px", bad);
    CHECK(region_nonzero(36, 24, 8, 12) > 0, "'A' after tofu rendered nothing");
}

/* ======================= runner ========================================= */

struct test_case {
    const char *name;
    void (*fn)(void);
};

int main(void)
{
    static const struct test_case cases[] = {
        { "ascii_A",        test_ascii_A },
        { "hangul_first",   test_hangul_first },
        { "hangul_last",    test_hangul_last },
        { "cjk_zhong",      test_cjk_zhong },
        { "mixed_advance",  test_mixed_advance },
        { "robustness",     test_robustness },
        { "tofu_advance",   test_tofu_advance },
    };
    unsigned i, failed_cases = 0;

    printf("DrawHZText12 host tests (Vcache only, isDrawDirect=0)\n");
    printf("KOR_COUNT=%u CJK_COUNT=%u\n\n",
           (unsigned)KOR_COUNT, (unsigned)CJK_COUNT);

    for (i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        g_case_fail = 0;
        cases[i].fn();
        if (g_case_fail == 0) {
            printf("PASS %s\n", cases[i].name);
        } else {
            printf("FAIL %s (%d failed check(s))\n", cases[i].name, g_case_fail);
            failed_cases++;
        }
    }

    printf("\n%u/%u cases passed, %d failed check(s) total\n",
           (unsigned)(sizeof cases / sizeof cases[0]) - failed_cases,
           (unsigned)(sizeof cases / sizeof cases[0]), g_total_fail);
    return g_total_fail ? 1 : 0;
}
