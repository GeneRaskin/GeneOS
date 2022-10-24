#include "handlers.h"
#include "../drivers/keyboard/keyboard.h"
#include "pic.h"
#include "pit.h"
#include "../cstd/include/libft.h"
#include "../drivers/ata/ata.h"

uint32_t tick = 0;

void    timer_interrupt(struct cpu_state cpu, uint32_t interrupt, struct stack_state_no_error stack) {
    (void) cpu;
    (void) stack;
    tick++;
    pic_acknowledge(interrupt);
}

void	keyboard_interrupt(struct cpu_state cpu, uint32_t interrupt, struct stack_state_no_error stack)
{
    (void) cpu;
    (void) stack;
    write_to_console();
    pic_acknowledge(interrupt);
}

void    divide_by_zero_exception(struct cpu_state cpu, struct stack_state_no_error stack) {
    (void) cpu;
    (void) stack;
    get_regs();
    kpanic("division by zero");
}

void    double_fault_exception(struct cpu_state cpu, struct stack_state_error stack) {
    (void) cpu;
    (void) stack;
    get_regs();
    kpanic("double fault");
}

void    general_protection_fault_exception(struct cpu_state cpu, struct stack_state_error stack) {
    (void) cpu;
    (void) stack;
    get_regs();
    kpanic("general protection fault");
}

void    page_fault_exception(struct cpu_state cpu, struct stack_state_error stack) {
    (void) cpu;
    (void) stack;
    get_regs();
    kpanic("page fault");
}

void    ata_primary_interrupt(struct cpu_state cpu, uint32_t interrupt,
                      struct stack_state_no_error stack) {
    (void) cpu;
    (void) stack;
    inb(ATA_PRIMARY_IO + ATA_REG_ALTSTATUS);
    pic_acknowledge(interrupt);
}

void    ata_secondary_interrupt(struct cpu_state cpu, uint32_t interrupt,
                              struct stack_state_no_error stack) {
    (void) cpu;
    (void) stack;
    inb(ATA_SECONDARY_IO + ATA_REG_ALTSTATUS);
    pic_acknowledge(interrupt);
}