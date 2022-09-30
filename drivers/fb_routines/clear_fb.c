#include "../io.h"
#include "fb_routines.h"

void	clear_fb(void)
{
	for (uint32_t i = 0; i < NUM_ROWS; ++i)
		clear_line(i);
    fb_move_cursor(0);
}
