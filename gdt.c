#include "gdt.h"
#include "tss.h"
#include "interrupts/handlers.h"

static struct gdtdesc	gdt_entries[GDTSIZE];
struct gdt		        kgdt;

void    init_gdt_desc(uint32_t base, uint32_t limit, uint8_t access, uint8_t other, struct gdtdesc *desc)
{
        desc->lim0_15 = (limit & 0xFFFF);
        desc->base0_15 = (base & 0xFFFF);
        desc->base16_23 = (base >> 16) & 0xFF;
        desc->access = access;
        desc->lim16_19 = (limit >> 16) & 0xF;
        desc->other = (other & 0xF);
        desc->base24_31 = (base >> 24) & 0xFF;
        return;
}

void	init_gdt(void)
{
	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &gdt_entries[0]); // null descriptor
	init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &gdt_entries[1]); // kernel code
	init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &gdt_entries[2]); // kernel data

	init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &gdt_entries[3]); // user code
	init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &gdt_entries[4]); // user data

    install_garbage_tss(&gdt_entries[5], &GARBAGE_TSS); // garbage TSS
    install_interrupt_tss(&gdt_entries[6], &INTERRUPT_TSS, 0x10,
                          INTERRUPT_STACK_ADDR + INTERRUPT_STACK_SIZE,
                          (uint32_t) double_fault_handler); // install TSS for double fault

	kgdt.limit = GDTSIZE * 8 - 1;
	kgdt.base = GDTBASE;
	
	for (uint32_t i = 0; i <= kgdt.limit; ++i)
		((char *) kgdt.base)[i] = ((char *) gdt_entries)[i];
	
	load_gdt(&kgdt);
    flush_tss(0x28);
}
