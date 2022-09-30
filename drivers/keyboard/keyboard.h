#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../io.h"

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define ENTER 0x1C
#define BACKSPACE 0x0E
#define SPACEBAR 0x39

void	write_to_console(void);
char	translate(uint8_t code, int uppercase);

#endif
