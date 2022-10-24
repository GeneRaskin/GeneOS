#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdint.h>
#include "../debug/debug.h"

void    timer_interrupt_handler(void);
void	keyboard_interrupt_handler(void);
void	ignore_int_no_error(void);
void	ignore_int_error(void);
void    div_by_zero_handler(void);
void    double_fault_handler(void);
void    general_protection_fault_handler(void);
void    page_fault_handler(void);
void    ata_primary_interrupt_handler(void);
void    ata_secondary_interrupt_handler(void);

void	keyboard_interrupt(struct cpu_state cpu, uint32_t interrupt, struct stack_state_no_error stack);
void    timer_interrupt(struct cpu_state cpu, uint32_t interrupt, struct stack_state_no_error stack);
void    divide_by_zero_exception(struct cpu_state cpu, struct stack_state_no_error stack);
void    general_protection_fault_exception(struct cpu_state cpu, struct stack_state_error stack);
void    double_fault_exception(struct cpu_state cpu, struct stack_state_error stack);
void    page_fault_exception(struct cpu_state cpu, struct stack_state_error stack);
void    ata_primary_interrupt(struct cpu_state cpu, uint32_t interrupt,
            struct stack_state_no_error stack);
void    ata_secondary_interrupt(struct cpu_state cpu, uint32_t interrupt,
            struct stack_state_no_error stack);

#endif
