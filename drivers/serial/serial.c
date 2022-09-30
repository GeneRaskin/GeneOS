#include "../io.h"

/** serial_configure_baud_rate:
 * Sets the speed of the data being sent. The default speed of a serial
 * port is 115200 bits/s. The argument is a divisor of that number, hence
 * the resulting speed becomes (115200 / divisor) bits/s.
 *
 * @param com		The COM port to configure
 * @param divisor	The divisor
 */

void	serial_configure_baud_rate(uint16_t com, uint16_t divisor)
{
	outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
	outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
	outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);	
}

/** serial_configure_line:
 * Configures the line of the given serial port. The port is set to have a
 * data length of 8 bits, no parity bits, one stop bit and break control
 * disabled.
 *
 * @param com	The serial port to configure
 */

void	serial_configure_line(uint16_t com)
{
	outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/** serial_configure_buffer:
 * Configures the buffer of the given serial port. The value is set such as
 * to enable FIFO, clear both receiver and transmission FIFO queues, and use
 * 14 bytes as size of queue.
 *
 * @param com	The serial port to configure
 */ 

void	serial_configure_buffer(uint16_t com)
{
	outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

/** serial_configure_modem:
 * Configures the modem of the given serial port. We set the RTS and DTR bits
 * to 1 (the least significant 2 bits of the modem configuration byte).
 *
 * @param com	The serial port to configure
 */
void	serial_configure_modem(uint16_t com)
{
	outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

/** serial_is_transmit_fifo_empty:
 * Checks whether the transmit FIFO queue is empty or not for the given COM port.
 *
 * @param com	The COM port
 * @return	0 if the transmit FIFO queue is not empty
 * 		1 if the transmit FIFO queue is empty
 */

int	serial_is_transmit_fifo_empty(uint16_t com)
{
	return inb(SERIAL_LINE_STATUS_PORT(com) & 0x20);
}
