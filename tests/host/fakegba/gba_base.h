/* fakegba/gba_base.h — host-build stand-in for libgba's <gba_base.h>.
 *
 * Provides only what source/draw.c (and the headers it pulls in) needs to
 * compile on a host compiler: the GBA fixed-width typedefs and the section
 * attribute macros, which are meaningless on the host and expand to nothing.
 */
#ifndef FAKEGBA_GBA_BASE_H
#define FAKEGBA_GBA_BASE_H

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef volatile u8  vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8  vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

/* GBA memory-section attributes: no-ops on the host. */
#define IWRAM_CODE
#define EWRAM_CODE
#define IWRAM_DATA
#define EWRAM_DATA
#define EWRAM_BSS

#ifndef ALIGN
#define ALIGN(m) __attribute__((aligned(m)))
#endif

#ifndef BIT
#define BIT(number) (1 << (number))
#endif

#endif /* FAKEGBA_GBA_BASE_H */
