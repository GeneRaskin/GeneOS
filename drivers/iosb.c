#include "io.h"

void    insl(uint16_t port, uint8_t *buffer, uint32_t quads) {
    for (uint32_t i = 0; i < quads; i++)
        buffer[i] = inb(port);
}

void    outsb(uint16_t port, uint8_t *buffer, uint32_t quads) {
    for (uint32_t i = 0; i < quads; i++)
        outb(port, buffer[i]);
}