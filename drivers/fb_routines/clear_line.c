#include "fb_routines.h"

void    clear_line(uint16_t line_num)
{
	for (uint32_t i = 0; i < NUM_COLS; ++i)
	{
		uint16_t pos = 2 * NUM_COLS * line_num + 2 * i;
		fb_write_cell(pos, 0, fb_fg_color, fb_fg_color);
	}
}
