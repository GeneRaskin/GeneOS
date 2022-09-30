#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include "../memory/kheap/kheap.h"

struct cpu_state {
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} __attribute__ ((packed));

struct reg_values {
    struct cpu_state selectors_and_general;
    uint32_t cs;
    uint32_t eflags;
} __attribute__ ((packed));

struct stack_state_error {
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__ ((packed));

struct stack_state_no_error {
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__ ((packed));

void    kpanic(char *error_msg);

void        get_regs();
uint32_t    get_ebp();

void    printf_regs_values();
void    printf_reg(char *reg_name, uint32_t reg_value);
void    get_stack_trace(uint32_t max_frames);
void    print_heap(heap_t *heap);
void    print_heap_index(heap_t *heap);

#endif
