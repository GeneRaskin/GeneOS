#include "pic.h"

void	pic_acknowledge(uint32_t interrupt)
{
	if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
		return;
	}
	
	if (interrupt < PIC2_START_INTERRUPT) {
		outb(PIC1_COMMAND, PIC_ACK);
	}
	else {
		outb(PIC2_COMMAND, PIC_ACK);
	}
}

void	init_pic(void)
{
	uint8_t a1, a2;

	a1 = inb(PIC1_DATA);
	a2 = inb(PIC2_DATA);

	outb(PIC1_COMMAND, 0x11);
	outb(PIC2_COMMAND, 0x11);

	outb(PIC1_DATA, PIC1_START_INTERRUPT);
	outb(PIC2_DATA, PIC2_START_INTERRUPT);
	
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);

	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}
