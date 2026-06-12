/* fakegba/gba_dma.h — host-build stand-in for libgba's <gba_dma.h>.
 *
 * libgba's dmaCopy signature is dmaCopy(source, dest, size) — note the
 * src-then-dst argument order (opposite of memcpy).  memmove is used instead
 * of memcpy because kernel code occasionally DMA-copies within pReadCache
 * itself (e.g. Clear() with isDrawDirect=0), which would be an overlapping
 * memcpy (UB, and flagged by ASan).
 */
#ifndef FAKEGBA_GBA_DMA_H
#define FAKEGBA_GBA_DMA_H

#include <string.h>
#include "gba_base.h"

static inline void dmaCopy(const void *source, void *dest, u32 size)
{
    memmove(dest, source, size);
}

#endif /* FAKEGBA_GBA_DMA_H */
