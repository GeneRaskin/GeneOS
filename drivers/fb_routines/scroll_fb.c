#include "../io.h"
#include "fb_routines.h"

void	scroll_fb(void)
{
	for (uint32_t i = 0; i < NUM_ROWS - 1; ++i)
	{
		for (uint32_t j = 0; j < NUM_COLS; j++)
		{
			uint32_t curr_pos = (NUM_COLS * i + j) * 2;
			fb[curr_pos] = fb[curr_pos + NUM_COLS * 2];
			fb[curr_pos + 1] = fb[curr_pos + NUM_COLS * 2 + 1];
		}	
	}
	clear_line(NUM_ROWS - 1);
}
