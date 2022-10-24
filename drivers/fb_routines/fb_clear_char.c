#include "fb_routines.h"
#include "../keyboard/keyboard.h"
#include "../io.h"

void	fb_clear_char(void)
{
	if (fb_pos % NUM_COLS == 0 || fb_pos % NUM_COLS == 7)
		return;
    line_length--;
	fb_write_cell(fb_pos * 2 - 2, 0, fb_fg_color, fb_fg_color);
	fb_move_cursor(fb_pos - 1);
}
