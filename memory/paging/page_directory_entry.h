#ifndef PAGE_DIRECTORY_ENTRY_H
#define PAGE_DIRECTORY_ENTRY_H

#include <stdint.h>
#define PAGE_TABLES_PER_DIRECTORY 0x400

typedef uint32_t pd_entry;

enum PAGE_PDE_FLAGS {
    PDE_PRESENT     = 0x1,
    PDE_WRITABLE    = 0x2,
    PDE_USER        = 0x4,
    PDE_PWT         = 0x8,
    PDE_PCD         = 0x10,
    PDE_ACCESSED    = 0x20,
    PDE_DIRTY       = 0x40,
    PDE_4MB         = 0x80,
    PDE_CPU_GLOBAL  = 0x100,
    PDE_LV4_GLOBAL  = 0x200,
    PDE_FRAME       = 0x7FFFF000
};

void            pd_entry_set_attribute(pd_entry *entry, uint32_t attribute);
void            pd_entry_unset_attribute(pd_entry *entry, uint32_t attribute);
void            pd_entry_set_frame(pd_entry *entry, uint32_t addr);
uint32_t        pd_entry_is_present(pd_entry entry);
uint32_t        pd_entry_is_user(pd_entry entry);
uint32_t        pd_entry_is_4mb(pd_entry entry);
uint32_t        pd_entry_is_writable(pd_entry entry);
uint32_t        pd_entry_frame_addr(pd_entry entry);

#endif
