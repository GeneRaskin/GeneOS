#ifndef IO_H
#define IO_H

#include <stdint.h>

/* Global cursor position */

extern uint16_t	fb_pos;

/* FB address pointer */

extern char		*fb;

/* Dev addresses */

#define FB_ADDRESS		0x000B8000;

/* the I/O ports */
/* Framebuffer I/O ports */
#define FB_COMMAND_PORT			0x3D4
#define FB_DATA_PORT			0x3D5	/* FB position port */

/* Serial ports */
#define SERIAL_COM1_BASE		0x3F8	/* COM1 base port*/
#define	SERIAL_DATA_PORT(base)		(base)
#define SERIAL_FIFO_COMMAND_PORT(base)	(base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)	(base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base)	(base + 4)
#define SERIAL_LINE_STATUS_PORT(base)	(base + 5)

/* PIC ports */
#define PIC1_PORT_A	0x20
#define PIC2_PORT_A	0xA0
#define PIC1_COMMAND	PIC1_PORT_A
#define PIC1_DATA	(PIC1_PORT_A + 1)
#define PIC2_COMMAND	PIC2_PORT_A
#define PIC2_DATA	(PIC2_PORT_A + 1)

/* PIT ports */
#define PIT_COMMAND_PORT 0x43
#define PIT_REGISTER_CNT0 0x40

/* Keyboard data port */
#define KBD_DATA_PORT	0x60

/* the I/O port commands */
/* Framebuffer command port commands */
#define FB_HIGH_BYTE_COMMAND	14
#define FB_LOW_BYTE_COMMAND	15

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the
 * data port, then the lowest 8 bits will follow
 */
#define SERIAL_LINE_ENABLE_DLAB	0x80

/* PIC commands */
#define PIC_ACK			0x20

/** outb, outbl:
 *  Sends the given data to the given I/O port. Defined in io.s
 *
 *  @param port The I/O port to send the data to
 *  @param data The data to send to the I/O port/ 
 */

void	outb(uint16_t port, uint8_t data);
void    outbl(uint16_t port, uint32_t data);

/** inb, inbl:
 * Read a byte from an I/O port
 *
 * @param port	The address of the I/O port
 * @return	The read byte/dword
 */

uint8_t	    inb(uint16_t port);
uint32_t    inbl(uint16_t port);

#endif
