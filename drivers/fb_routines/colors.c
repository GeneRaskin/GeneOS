#include "fb_routines.h"

uint32_t fb_fg_color;
uint32_t fb_bg_color;

void fb_set_bg_color(uint32_t color) {
    fb_bg_color = color;
}

void fb_set_fg_color(uint32_t color) {
    fb_fg_color = color;
}
