//
// Created by eugene on 08/09/22.
//

#include "tss.h"
#include "cstd/include/libft.h"
#include "memory/paging/paging.h"

struct tss_entry INTERRUPT_TSS;
struct tss_entry USERMODE_TSS;
struct tss_entry GARBAGE_TSS;

/*void tss_set_stack(uint32_t kernel_ss, uint32_t kernel_esp) {
    TSS.ss0 = kernel_ss;
    TSS.esp0 = kernel_esp;
}*/

void install_garbage_tss(struct  gdtdesc *desc, struct tss_entry *TSS) {
    uint32_t base = (uint32_t) TSS;
    init_gdt_desc(base, base + sizeof(struct tss_entry), 0x89, 0x0, desc);
    ft_memset((void *) TSS, 0, sizeof(struct tss_entry));
}

void set_page_directory_tss(struct tss_entry *TSS, uint32_t page_directory_addr) {
    TSS->cr3 = page_directory_addr;
}

void install_usermode_tss(struct gdtdesc *desc, struct tss_entry *TSS,
        uint32_t kernel_ss, uint32_t kernel_esp) {
    uint32_t base = (uint32_t) TSS;
    init_gdt_desc(base, base + sizeof(struct tss_entry), 0x89, 0x0, desc);
    ft_memset((void *)TSS, 0, sizeof(struct tss_entry));
    TSS->ss0 = kernel_ss;
    TSS->esp0 = kernel_esp;
}

void install_interrupt_tss(struct gdtdesc *desc, struct tss_entry *TSS,
        uint32_t kernel_ss, uint32_t kernel_esp, uint32_t routine_addr) {
    uint32_t base = (uint32_t) TSS;
    /* install the TSS descriptor */
    init_gdt_desc(base, base + sizeof(struct tss_entry), 0x89, 0x0, desc);
    /* init the TSS struct */
    ft_memset((void *)TSS, 0, sizeof(struct tss_entry));
    /* set stack and segments */
    TSS->ss0 = kernel_ss;
    TSS->esp0 = kernel_esp;
    TSS->esp = kernel_esp;
    TSS->cs = 0x08;
    TSS->ss = kernel_ss;
    TSS->es = 0x10;
    TSS->ds = 0x10;
    TSS->fs = 0x10;
    TSS->gs = 0x10;
    TSS->eip = routine_addr;
}
