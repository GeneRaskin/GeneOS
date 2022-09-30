#ifndef FB_ROUTINES_H
#define FB_ROUTINES_H

#define NUM_COLS 80
#define NUM_ROWS 25

#include <stdint.h>

void    fb_write_cell(uint32_t i, char c, uint8_t fg, uint8_t bg);
void    fb_move_cursor(uint16_t pos);
void    clear_fb(void);
void	clear_line(uint16_t line_num);
void	scroll_fb(void);
void	fb_clear_char(void);
void    fb_set_bg_color(uint32_t color);
void    fb_set_fg_color(uint32_t color);

extern uint32_t fb_bg_color;
extern uint32_t fb_fg_color;

#endif
