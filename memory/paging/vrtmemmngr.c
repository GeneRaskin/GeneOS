#include "paging.h"
#include "../kheap/kheap.h"
#include "../../tss.h"

// current memory address of the directory table &
// current value of the page directory base register
page_directory_table    *curr_directory = 0;
uint32_t                curr_pdbr = 0;
/* The pointer to the kernel page directory table */
page_directory_table    *kernel_directory = 0;
/* define an area of free memory for kernel heap,
 * starting at 3076 MB virtual. Assume pages are
 * already mapped and allocated! */
uint32_t                placement_address = 0xC0400000;

uint32_t    alloc_page(pt_entry *entry, uint8_t supervisor,
                        uint8_t readonly) {
    uint32_t    *p = allocate_blocks(1);
    if (!p)
        return 0;
    pt_entry_set_frame(entry, (uint32_t) p);
    pt_entry_set_attribute(entry, PTE_PRESENT);
    if (!supervisor)
        pt_entry_set_attribute(entry, PTE_USER);
    if (!readonly)
        pt_entry_set_attribute(entry, PTE_WRITABLE);
    return 1;
}

/* request a new page to be allocated at a specified
 * virtual address. Returns 0 if failed, 1 if
 * succeeded. Alternatively, call alloc_page
 * with the pointer to pt_entry returned from a call
 * to get_page */
uint32_t    request_page(uint32_t virtual_addr, uint8_t supervisor,
                            uint8_t readonly) {
    uint32_t *p = allocate_blocks(1);
    if (!p) // if failed to allocate a block, return immediately
        return 0;
    map_page(p, (uint32_t *) virtual_addr,
             supervisor, readonly); // map memory
    return 1;
}

void    free_page(pt_entry *entry) {
    uint32_t *p = (uint32_t *)pt_entry_frame_addr(*entry);
    if (p)
        free_blocks(p, 1);
    pt_entry_unset_attribute(entry, PTE_PRESENT);
    if (pt_entry_is_writable(*entry))
        pt_entry_unset_attribute(entry, PTE_WRITABLE);
    if (pt_entry_is_user(*entry))
        pt_entry_unset_attribute(entry, PTE_USER);
}

pt_entry    *get_page(page_directory_table *directory, uint32_t addr) {
    pd_entry *curr_pd_entry = pdirectory_lookup_entry(directory, addr);
    return ptable_lookup_entry((page_table *) curr_pd_entry, addr);
}

pt_entry    *ptable_lookup_entry(page_table *ptable, uint32_t addr) {
    if (ptable)
        return &ptable->entries[PAGE_TABLE_INDEX(addr)];
    return 0;
}

pd_entry    *pdirectory_lookup_entry(page_directory_table *directory,
                                        uint32_t addr) {
    if (directory)
        return &directory->entries[PAGE_DIRECTORY_TABLE_INDEX(addr)];
    return 0;
}

uint32_t    switch_pdirectory(page_directory_table *directory) {
    if (!directory)
        return 0;
    curr_directory = directory;
    load_pdbr((uint32_t)curr_directory);
    return 1;
}

page_directory_table    *get_pdirectory() {
    return curr_directory;
}

void    map_page(uint32_t *physical_addr, uint32_t *virtual_addr,
                 uint8_t supervisor, uint8_t readonly) {
    // get the current page directory
    page_directory_table *directory = get_pdirectory();
    // get the page directory entry (a particular page table)
    pd_entry *entry = &directory->entries[
            PAGE_DIRECTORY_TABLE_INDEX((uint32_t) virtual_addr)];
    if (!pd_entry_is_present(*entry)) {
        //if page table is not present have to allocate it
        page_table  *table = (page_table *) allocate_blocks(1);
        if (!table)
            return;
        //clear page table
        ft_memset(table, 0, sizeof(page_table));
        //create a new entry
        pd_entry    *entry = &directory->entries[
                PAGE_DIRECTORY_TABLE_INDEX((uint32_t) virtual_addr)];
        pd_entry_set_attribute(entry, PDE_PRESENT);
        if (!readonly)
            pd_entry_set_attribute(entry, PDE_WRITABLE);
        if (!supervisor)
            pd_entry_set_attribute(entry, PDE_USER);
        pd_entry_set_frame(entry, (uint32_t) table);
    }
    // get table (at this point, the page table is guaranteed to be valid
    // at the given virtual address
    page_table  *table = (page_table *) PAGE_GET_PHYSICAL_ADDR(entry);
    // get page from the table
    pt_entry    *page = &table->entries[PAGE_TABLE_INDEX((uint32_t) virtual_addr)];
    pt_entry_set_frame(page, (uint32_t) physical_addr);
    pt_entry_set_attribute(page, PTE_PRESENT);
    if (!readonly)
        pt_entry_set_attribute(page, PTE_WRITABLE);
    if (!supervisor)
        pt_entry_set_attribute(page, PTE_USER);
}

/* Map kernel heap starting at 3076 MB virtual
 * Returns 0 if failed, 1 if succeeded. */
static uint8_t map_memory_for_kheap() {
    uint32_t i;
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i+=0x1000) {
        if (!request_page(i, 1, 0))
            return 0;
    }
    return 1;
}

void    vrtmemmngr_init() {
    ft_printf("Virtual memory manager initialization...\n");
    // allocate 3GB page table
    page_table *table = (page_table *) allocate_blocks(1);
    if (!table)
        return;
    // allocate default page table
    page_table *table2 = (page_table *) allocate_blocks(1);
    if (!table2)
        return;
    ft_memset(table, 0, sizeof(page_table));
    // first 4mb are identity mapped
    for (uint32_t i = 0, frame = 0x0, virt = 0x0; i < 0x400;
        i++, frame += 0x1000, virt += 0x1000) {
        // create a new page
        pt_entry page = 0;
        if (frame != 0x101000) // check if not kernel stack protector
            pt_entry_set_attribute(&page, PTE_PRESENT);
        pt_entry_set_frame(&page, frame);
        // add this page to the page table
        table2->entries[PAGE_TABLE_INDEX(virt)] = page;
    }
    // map kernel to 3gb
    for (uint32_t i = 0, frame = 0x0, virt = KERNEL_VIRTUAL_ADDR;
        i < 0x400; i++, frame += 0x1000, virt += 0x1000) {
        // create a new page
        pt_entry page = 0;
        if (frame != 0x101000) // check if not kernel stack protector
            pt_entry_set_attribute(&page, PTE_PRESENT);
        pt_entry_set_frame(&page, frame);
        // add the newly created page to the corresponding page table
        table->entries[PAGE_TABLE_INDEX(virt)] = page;
    }
    // create default page directory table and clear it; set to current
    page_directory_table *directory = (page_directory_table *) allocate_blocks(1);
    if (!directory)
        return;
    ft_memset(directory, 0, sizeof(page_directory_table));
    pd_entry *entry = &directory->entries[PAGE_DIRECTORY_TABLE_INDEX(KERNEL_VIRTUAL_ADDR)];
    pd_entry_set_attribute(entry, PDE_PRESENT);
    pd_entry_set_attribute(entry, PDE_WRITABLE);
    pd_entry_set_frame(entry, (uint32_t) table);

    pd_entry *entry2 = &directory->entries[PAGE_DIRECTORY_TABLE_INDEX(0x0)];
    pd_entry_set_attribute(entry2, PDE_PRESENT);
    pd_entry_set_attribute(entry2, PDE_WRITABLE);
    pd_entry_set_frame(entry2, (uint32_t) table2);

    // switch to the created page directory
    curr_pdbr = (uint32_t) &directory->entries;
    switch_pdirectory(directory);
    // set the global kernel page directory pointer to the newly
    // created one
    kernel_directory = directory;
    // set tss cr3 field
    set_page_directory_tss(&INTERRUPT_TSS, (uint32_t) curr_directory);
    set_page_directory_tss(&GARBAGE_TSS, (uint32_t) curr_directory);
    set_page_directory_tss(&USERMODE_TSS, (uint32_t) curr_directory);
    // map pages for the kernel heap
    map_memory_for_kheap();
    // enable paging
    enable_paging(1);
}

/* An auxiliary function to clone page tables */
/*page_table *clone_table(page_table *src, uint32_t physical_addr) {
    page_table *new_table = (page_table *) kmalloc_ap(
            sizeof(page_table), physical_addr);
    ft_memset(new_table, 0, sizeof(page_table));
    for (uint32_t i = 0; i < 0x400; ++i) {
        if (!src->entries[i])
            continue;

    }
}*/

/* Function to clone an existing page directory
 * Used for multitasking management */
/*page_directory_table *clone_directory(page_directory_table *src) {
    uint32_t physical_addr;
    page_directory_table *new_directory = (page_directory_table *) kmalloc_ap(
            sizeof(page_directory_table),&physical_addr);
    // ensure that the newly allocated chunk is clean
    ft_memset(new_directory, 0, sizeof(page_directory_table));
    for (uint32_t i = 0; i < 0x400; ++i) {
        if (!src->entries[i]) // if empty pointer, continue
            continue;
        // if a page table is in the kernel space, make a link
        if (kernel_directory->entries[i] == src->entries[i]) {
            new_directory->entries[i] = src->entries[i];
        } else { // copy the page table
            uint32_t physical_addr;

        }
    }
    return new_directory;
}*/

uint32_t kmalloc_int(uint32_t size, uint32_t align, uint32_t *physical_addr) {
    // if the kheap is allocated, call kernel heap allocation routine
    if (kheap) {
        return ((uint32_t) alloc(size, align, kheap));
    }
    // if the addr is not page-aligned, have to align it
    if (align == 1 && (placement_address & 0xFFFFF000)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (physical_addr) {
        *physical_addr = placement_address;
    }
    uint32_t tmp = placement_address;
    // if not enough free space available
    if (placement_address + size > KHEAP_MAX_ADDR) {
        return 0;
    }
    placement_address += size;
    return tmp;
}

uint32_t kmalloc_a(uint32_t size) {
    return kmalloc_int(size, 1, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *physical_addr) {
    return kmalloc_int(size, 0, physical_addr);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *physical_addr) {
    return kmalloc_int(size, 1, physical_addr);
}

uint32_t kmalloc(uint32_t size) {
    return kmalloc_int(size, 0, 0);
}

uint32_t kv2p(uint32_t k_virt_addr) {
    return (k_virt_addr - KERNEL_VIRTUAL_ADDR);
}
