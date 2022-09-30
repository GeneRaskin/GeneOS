#include "serial_write.h"
#include "../io.h"
#include "serial.h"

int	serial_write(char *buf, uint32_t len, uint16_t com)
{
	uint32_t	i = 0;
	while (serial_is_transmit_fifo_empty(com) == 0)
		;
	for (;i < len && buf[i]; ++i) {
		outb(com, buf[i]);
	}
	return (i);
}
