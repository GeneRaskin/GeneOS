#ifndef PAGE_TABLE_ENTRY_H
#define PAGE_TABLE_ENTRY_H

#include <stdint.h>
#define ENTRIES_PER_PAGE_TABLE 0x400

enum PAGE_PTE_FLAGS {
    PTE_PRESENT         = 0x1,
    PTE_WRITABLE        = 0x2,
    PTE_USER            = 0x4,
    PTE_WRITETHROUGH    = 0x8,
    PTE_NOT_CACHEABLE   = 0x10,
    PTE_ACCESSED        = 0x20,
    PTE_DIRTY           = 0x40,
    PTE_PAT             = 0x80,
    PTE_CPU_GLOBAL      = 0x100,
    PTE_LV4_GLOBAL      = 0x200,
    PTE_FRAME           = 0x7FFFF000
};

typedef uint32_t pt_entry;

void            pt_entry_set_attribute(pt_entry *entry, uint32_t attribute);
void            pt_entry_unset_attribute(pt_entry *entry, uint32_t attribute);
void            pt_entry_set_frame(pt_entry *entry, uint32_t addr);
uint32_t        pt_entry_is_present(pt_entry entry);
uint32_t        pt_entry_is_writable(pt_entry entry);
uint32_t        pt_entry_frame_addr(pt_entry entry);
uint32_t        pt_entry_is_user(pt_entry entry);

#endif
