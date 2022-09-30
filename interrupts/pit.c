#include "pit.h"
#include "../drivers/io.h"

/* Initialize the programmable interrupt timer*/
void init_pit(uint32_t freq) {
    uint32_t divisor = 1193180 / freq;
    /* Send the command word */
    outb(PIT_COMMAND_PORT, 0x36);
    /* Split the divisor into low and high bytes */
    uint8_t lbyte = divisor & 0xFF;
    uint8_t hbyte = (divisor >> 8) & 0xFF;
    /* Send the frequency divisor to the counter 0 */
    outb(PIT_REGISTER_CNT0, lbyte);
    outb(PIT_REGISTER_CNT0, hbyte);
}
