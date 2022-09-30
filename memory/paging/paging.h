#ifndef PAGING_H
#define PAGING_H

#include "page_table_entry.h"
#include "page_directory_entry.h"
#include "../physmemmngr/physical_memory_manager.h"
#define KERNEL_VIRTUAL_ADDR 0xC0000000

typedef struct _page_table {
    pt_entry entries[ENTRIES_PER_PAGE_TABLE];
} page_table;

typedef struct _page_directory_table {
    pd_entry entries[PAGE_TABLES_PER_DIRECTORY];
} page_directory_table;

#define PAGE_DIRECTORY_TABLE_INDEX(x) (((x) >> 0x16) & 0x3FF)
#define PAGE_TABLE_INDEX(x) (((x) >> 0xC) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDR(x) (*x & ~0xFFF)
#define PAGE_TABLE_ADDR_SPACE_SIZE 0x400000
#define PAGE_DIRECTORY_TABLE_ADDR_SPACE_SIZE 0x100000000
#define PAGE_SIZE 0x1000

extern page_directory_table *curr_directory;
extern page_directory_table *kernel_directory;
extern uint32_t             curr_pdbr;

uint32_t                alloc_page(pt_entry *entry, uint8_t supervisor,
                                   uint8_t readonly);
void                    free_page(pt_entry *entry);
pt_entry                *ptable_lookup_entry(page_table *ptable, uint32_t addr);
pd_entry                *pdirectory_lookup_entry(page_directory_table *directory, uint32_t addr);
uint32_t                switch_pdirectory(page_directory_table *directory);
page_directory_table    *get_pdirectory();
//page_directory_table    *clone_directory(page_directory_table *src);
void                    map_page(uint32_t *physical_addr, uint32_t *virtual_addr,
                                 uint8_t supervisor, uint8_t readonly);
uint32_t                request_page(uint32_t virtual_addr, uint8_t supervisor,
                                     uint8_t readonly);
pt_entry                *get_page(page_directory_table *directory, uint32_t addr);
void                    vrtmemmngr_init();
void                    flush_tlb_entry(uint32_t addr);

/* kmalloc routines */
uint32_t                kmalloc_int(uint32_t size, uint32_t align, uint32_t *physical_addr);
/* kmalloc_a: returns page-aligned address */
uint32_t                kmalloc_a(uint32_t size);
/* kmalloc_p: sets a physical_addr pointer when allocating new memory */
uint32_t                kmalloc_p(uint32_t size, uint32_t *physical_addr);
/* kmalloc_ap: sets a physical_addr pointer & returns a page-aligned address */
uint32_t                kmalloc_ap(uint32_t size, uint32_t *physical_addr);
uint32_t                kmalloc(uint32_t size);

#endif
