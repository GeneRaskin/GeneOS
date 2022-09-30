#include "../io.h"
#include "fb_write.h"
#include "../fb_routines/fb_routines.h"
#include "../fb_routines/colors.h"

void	fb_write_cell(uint32_t i, char c, uint8_t fg, uint8_t bg)
{
	fb[i] = c;
	fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0f);
}

void	fb_move_cursor(uint16_t pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT, pos & 0x00FF);
	fb_pos = pos;
}

int	fb_write(char *buf, uint32_t len)
{
	uint32_t	i = 0;
	for (; buf[i] && i < len; ++i)
	{
		if (fb_pos + 1 >= NUM_COLS * NUM_ROWS)
		{
			scroll_fb();
			fb_move_cursor(fb_pos - (fb_pos % NUM_COLS));
		}
		if (buf[i] == '\n')
		{
			if (fb_pos / NUM_COLS  == NUM_ROWS - 1)
			{
				scroll_fb();
				fb_move_cursor(fb_pos - (fb_pos % NUM_COLS));
			}
			else
				fb_move_cursor((fb_pos / NUM_COLS + 1) * NUM_COLS);
		}
		else
		{
			fb_write_cell(fb_pos * 2, buf[i], fb_fg_color, fb_bg_color);
			fb_move_cursor(fb_pos + 1);
		}
	}
	return (i);
}
