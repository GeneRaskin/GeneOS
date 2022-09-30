#ifndef TSS_H
#define TSS_H

#include <stdint.h>
#include "gdt.h"

#define INTERRUPT_STACK_ADDR 0x00103000
#define INTERRUPT_STACK_SIZE 0x1000

struct tss_entry {
    uint32_t prevTss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap;
}__attribute__((packed));

extern struct tss_entry INTERRUPT_TSS;
extern struct tss_entry GARBAGE_TSS;
extern struct tss_entry USERMODE_TSS;

void tss_set_stack(uint32_t kernel_ss, uint32_t kernel_esp);
void install_garbage_tss(struct gdtdesc *desc, struct tss_entry *TSS);
void install_interrupt_tss(struct gdtdesc *desc, struct tss_entry *TSS,
        uint32_t kernel_ss, uint32_t kernel_esp, uint32_t routine_addr);
void install_usermode_tss(struct gdtdesc *desc, struct tss_entry *TSS,
        uint32_t kernel_ss, uint32_t kernel_esp);
void flush_tss(uint16_t selector);
void set_page_directory_tss(struct tss_entry *TSS, uint32_t page_directory_addr);

#endif
