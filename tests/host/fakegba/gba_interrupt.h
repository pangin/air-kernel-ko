/* fakegba/gba_interrupt.h — host-build stand-in for libgba's
 * <gba_interrupt.h>.  Interrupt plumbing becomes a no-op on the host. */
#ifndef FAKEGBA_GBA_INTERRUPT_H
#define FAKEGBA_GBA_INTERRUPT_H

#include "gba_base.h"

typedef enum irqMASKS {
    IRQ_VBLANK = (1 << 0),
    IRQ_HBLANK = (1 << 1),
    IRQ_VCOUNT = (1 << 2),
    IRQ_TIMER0 = (1 << 3),
    IRQ_TIMER1 = (1 << 4),
    IRQ_TIMER2 = (1 << 5),
    IRQ_TIMER3 = (1 << 6),
    IRQ_SERIAL = (1 << 7),
    IRQ_DMA0   = (1 << 8),
    IRQ_DMA1   = (1 << 9),
    IRQ_DMA2   = (1 << 10),
    IRQ_DMA3   = (1 << 11),
    IRQ_KEYPAD = (1 << 12),
    IRQ_GAMEPAK = (1 << 13)
} irqMASK;

static inline void irqInit(void) {}
static inline void irqEnable(int mask) { (void)mask; }
static inline void irqDisable(int mask) { (void)mask; }

#endif /* FAKEGBA_GBA_INTERRUPT_H */
