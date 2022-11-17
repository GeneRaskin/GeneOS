#include "keyboard.h"
#include "../fb_write/fb_write.h"
#include "../fb_routines/fb_routines.h"

char                line_buffer[LINE_BUFFER_SIZE] = {0};
uint32_t            line_length = 0;
volatile uint8_t    enter_pressed = 0;

const char      ASCIITable[] = {
        0, 0, '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=', 0, 0,
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
        0, 0, 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'', '`', 0, '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/', 0, '*',
        0, ' '
};

int	is_left_shift_pressed = 0;
int	is_right_shift_pressed = 0;

uint8_t	read_scan_code(void)
{
	return inb(KBD_DATA_PORT);
}

void	write_to_console(void)
{
	uint8_t	code;
	char		ascii;

	code = read_scan_code();
	switch (code){
		case LEFT_SHIFT:
			is_left_shift_pressed = 1;
			return;
		case LEFT_SHIFT + 0x80:
			is_left_shift_pressed = 0;
			return;
		case RIGHT_SHIFT:
			is_right_shift_pressed = 1;
			return;
		case RIGHT_SHIFT + 0x80:
			is_right_shift_pressed = 0;
			return;
		case ENTER:
            line_buffer[line_length] = 0;
			fb_write("\n", 1);
            enter_pressed = 1;
			return;
		case SPACEBAR:
			fb_write(" ", 1);
            if (line_length >= LINE_BUFFER_SIZE)
                line_length = 0;
            line_buffer[line_length++] = ' ';
			return;
		case BACKSPACE:
			fb_clear_char();
			return;
	}
	
	ascii = translate(code, is_right_shift_pressed | is_left_shift_pressed);
	if (ascii != 0)
		fb_write(&ascii, 1);
}

char	translate(uint8_t code, int uppercase)
{
    char    translated;
	if (code > 58)
		return 0;
    if (line_length >= LINE_BUFFER_SIZE) {
        line_length = 0;
    }
	if (uppercase)
	{
        if (code == 12)
            translated = '_';
        else if (code == 39)
            translated = ':';
        else
            translated = ASCIITable[code] - 32;
        line_buffer[line_length++] = translated;
		return translated;
	}
    translated = ASCIITable[code];
    line_buffer[line_length++] = translated;
	return translated;
}
