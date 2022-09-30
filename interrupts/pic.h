#ifndef PIC_H
#define PIC_H

#include "../drivers/io.h"

#define PIC1_START_INTERRUPT    0x20
#define PIC2_START_INTERRUPT    0x28                                          
#define PIC2_END_INTERRUPT      PIC2_START_INTERRUPT + 7 

void	pic_acknowledge(uint32_t interrupt);
void	init_pic(void);

#endif

