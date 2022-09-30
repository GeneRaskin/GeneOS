#ifndef CPU_H
#define CPU_H

#include <stdint.h>

extern char vendor[32];

int32_t i86_cpu_init();
void    i86_cpu_shutdown();
void    i86_cpu_get_vendor();
void    i86_cpu_flush_caches();
void    i86_cpu_flush_caches_write();
void    i86_cpu_flush_tlb_entry(uint32_t addr);

#endif
