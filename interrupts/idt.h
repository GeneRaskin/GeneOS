#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "../debug/debug.h"

struct idt {
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed));

struct idtdesc {
	uint16_t offset0_15; /* reserved for task gate */
	uint16_t select; /* tss descriptor offset in the GDT for task gate */
	uint16_t type; /* first 8 bits reserved for task gate */
	uint16_t offset16_31; /* reserved for task gate */
} __attribute__ ((packed));

#define IDTBASE		0x00000000
#define IDTSIZE		0x100

#define INTGATE		0x8E00
#define TRAPGATE	0xEF00
#define TASKGATE    0x8500

/* TSS segments offsets */
#define TSS_DOUBLE_FAULT 0x30

void	init_idt_desc(uint16_t select, uint32_t offset, uint16_t type, struct idtdesc *desc);

void	init_idt(void);

void	init_pic(void);

void	load_idt(struct idt *kidt);	

#endif
