#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct gdt {
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed));

struct gdtdesc {
	uint16_t lim0_15;
	uint16_t base0_15;
	uint8_t base16_23;
	uint8_t access;
	uint8_t lim16_19:4;
	uint8_t other:4;
	uint8_t base24_31;
} __attribute__ ((packed));

#define GDTBASE 0x00000800
#define GDTSIZE	0x7

/* init_gdt_desc:
 * This function adds a segment descriptor entry into the Global Descriptor Table.
 * The format of the entry is as follows:
 * | Limit 0:7  | Limit 8:15  | Base 0:7            | Base 7:15  |
 * | Base 16:23 | Access Byte | Flags | Limit 16:19 | Base 24:31 |
 *
 * @param base		a 32-bit value containing the linear address where the segment begins.
 * @param limit		a 20-bit value, tells the maximum addressable unit, either in 1 byte units,
 * 			or in 4KiB pages (depends on the granularity flag).
 * @param access	set access byte of the segment. |7|6|5|4|3|2 |1 |0|
 * 							|P|DPL|S|E|DC|RW|A|
 * 			P: present bit. Must be set (1) for any valid segment.
 * 			DPL: descriptor privilege level field. 0 = highest privilege 
 * 			(kernel mode); 3 = user mode
 * 			S: descriptor type bit.
 * 			E: executable bit.
 * 			DC: direction bit/conforming bit.
 * 			RW: readable bit/writable bit.
 * 			A: Accessed bit (reserved for the CPU). Set it to 0.
 * @param other		Set up auxillary flags. |3|2 |1|0       |
 * 						|G|DB|L|Reserved|
 * 			G: is the granulariy flag, indicates the size the limit value is
 * 			scaled by. If set to 1, we get page granularity (4 KiB blocks).
 * 			DB: size flag. If set to 1, it defines a 32-bit protected
 * 			mode segment. Otherwise, it defines a 16-bit protected mode
 * 			segment.
 * 			L: long-mode code flag. If set to 1, it defines a 64-bit code
 * 			segment. If set to 1, DB should be clear. In other cases, L
 * 			should be clear.
 * @param desc		a pointer to the GDT struct
 */

void	init_gdt_desc(uint32_t base, uint32_t limit, uint8_t access, uint8_t other, struct gdtdesc *desc);

/* init_gdt:
 * initializes the GDT table.
 */

void	init_gdt(void);

/* load_gdt:
 * load the initialized GDT table.
 *
 * @param gdt	a pointer to the table to be loaded
 */

void	load_gdt(struct gdt *kgdt);

#endif
