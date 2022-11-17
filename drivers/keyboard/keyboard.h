#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../io.h"

#define LEFT_SHIFT 0x2A
#define RIGHT_SHIFT 0x36
#define ENTER 0x1C
#define BACKSPACE 0x0E
#define SPACEBAR 0x39

#define LINE_BUFFER_SIZE 100
extern uint32_t prompt_width;

extern char             line_buffer[LINE_BUFFER_SIZE];
extern volatile uint8_t enter_pressed;
extern uint32_t         line_length;

void	write_to_console(void);
char	translate(uint8_t code, int uppercase);

#endif
