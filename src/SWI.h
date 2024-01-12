#ifndef SWI_H
#define SWI_H

#include <LPC210x.H>
#include <inttypes.h>

uint32_t __swi(0xFC) read_IRQ_bit(void);
void __swi(0xFF) enable_irq (void);
void __swi(0xFE) disable_irq(void);
void __swi(0xFD) disable_fiq(void);



#endif
