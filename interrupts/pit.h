#ifndef PIT_H
#define PIT_H

#include <stdint.h>

extern uint32_t tick;

void    init_pit(uint32_t freq);
void    sys_sleep(uint32_t ms);

#endif
