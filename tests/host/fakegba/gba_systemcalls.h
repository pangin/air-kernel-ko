/* fakegba/gba_systemcalls.h — host-build stand-in for libgba's
 * <gba_systemcalls.h>.  BIOS calls become no-ops on the host. */
#ifndef FAKEGBA_GBA_SYSTEMCALLS_H
#define FAKEGBA_GBA_SYSTEMCALLS_H

#include "gba_base.h"

static inline void VBlankIntrWait(void) {}
static inline void Halt(void) {}
static inline void Stop(void) {}

#endif /* FAKEGBA_GBA_SYSTEMCALLS_H */
