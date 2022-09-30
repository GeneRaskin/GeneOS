#ifndef SERIAL_H
#define SERIAL_H

void	serial_configure_baud_rate(uint16_t com, uint16_t divisor);
void	serial_configure_line(uint16_t com);
void	serial_configure_buffer(uint16_t com);
void	serial_configure_modem(uint16_t com);
int     serial_is_transmit_fifo_empty(uint16_t com);

#endif
