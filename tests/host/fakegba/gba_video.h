/* fakegba/gba_video.h — host-build stand-in for libgba's <gba_video.h>.
 *
 * draw.c does not currently include this header, but it is provided so the
 * harness keeps compiling if the in-flight renderer rewrite adds it.
 * Never dereference REG_DISPCNT / VRAM on the host — tests must only use
 * isDrawDirect=0 so the renderer writes to Vcache (pReadCache).
 */
#ifndef FAKEGBA_GBA_VIDEO_H
#define FAKEGBA_GBA_VIDEO_H

#include "gba_base.h"

#define MODE_0 0x0000
#define MODE_1 0x0001
#define MODE_2 0x0002
#define MODE_3 0x0003
#define MODE_4 0x0004
#define MODE_5 0x0005

#define BG0_ON (1 << 8)
#define BG1_ON (1 << 9)
#define BG2_ON (1 << 10)
#define BG3_ON (1 << 11)

#define RGB5(r, g, b)  ((r) | ((g) << 5) | ((b) << 10))
#define RGB8(r, g, b)  (((r) >> 3) | (((g) >> 3) << 5) | (((b) >> 3) << 10))

/* On hardware this writes REG_DISPCNT; a no-op on the host. */
#define SetMode(mode) ((void)(mode))

#endif /* FAKEGBA_GBA_VIDEO_H */
