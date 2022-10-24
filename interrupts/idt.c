#include "idt.h"
#include "../drivers/io.h"
#include "pic.h"
#include "pit.h"
#include "handlers.h"

static struct idtdesc	idt_entries[IDTSIZE];
struct idt		kidt;

void	init_idt_desc(uint16_t select, uint32_t offset, uint16_t type, struct idtdesc *desc)
{
	desc->offset0_15 = (offset & 0xFFFF);
	desc->select = select;
	desc->type = type;
	desc->offset16_31 = (offset >> 16) & 0xFFFF;
	return;
}

void	init_idt(void)
{
	for (uint32_t i = 0; i < 32; ++i) {
        if (i == 0) {
            init_idt_desc(0x08, (uint32_t) div_by_zero_handler, TRAPGATE, &idt_entries[i]);
        } else if (i == 13) {
            init_idt_desc(0x08, (uint32_t) general_protection_fault_handler, TRAPGATE, &idt_entries[i]);
        } else if (i == 8) {
            //init_idt_desc(0x08, (uint32_t) double_fault_handler, TRAPGATE, &idt_entries[i]);
            init_idt_desc(TSS_DOUBLE_FAULT, 0x0, TASKGATE, &idt_entries[i]);
        } else if (i == 14) {
            init_idt_desc(0x08, (uint32_t) page_fault_handler, TRAPGATE, &idt_entries[i]);
            //init_idt_desc(0x30, 0x0, TASKGATE, &idt_entries[i]);
        } else if (i == 10 || i == 11 || i == 12 ||
		    i == 17 || i == 21) {
			init_idt_desc(0x08, (uint32_t) ignore_int_error, TRAPGATE, &idt_entries[i]);
		} else {
			init_idt_desc(0x08, (uint32_t) ignore_int_no_error, TRAPGATE, &idt_entries[i]);
		}	
	}

	for (uint32_t i = 32; i < IDTSIZE; ++i) {
		if (i == 33)
			init_idt_desc(0x08, (uint32_t) keyboard_interrupt_handler, INTGATE, &idt_entries[33]);
        else if (i == 32)
            init_idt_desc(0x08, (uint32_t) timer_interrupt_handler, INTGATE, &idt_entries[32]);
        else if (i == 46)
            init_idt_desc(0x08, (uint32_t) ata_primary_interrupt_handler, INTGATE, &idt_entries[46]);
        else if (i == 47)
            init_idt_desc(0x08, (uint32_t) ata_secondary_interrupt_handler, INTGATE, &idt_entries[47]);
		else
			init_idt_desc(0x08, (uint32_t) ignore_int_no_error, INTGATE, &idt_entries[i]);
	}

	kidt.limit = IDTSIZE * 8 - 1;
	kidt.base = IDTBASE;

	for (uint32_t i = 0; i <= kidt.limit; ++i)
		((char *) kidt.base)[i] = ((char *) idt_entries)[i];
	load_idt(&kidt);
	
	init_pic();
    init_pit(1000);
	outb(PIC1_DATA, 0b11111000);
	outb(PIC2_DATA, 0b00111111);
	
	asm ("sti");
}
