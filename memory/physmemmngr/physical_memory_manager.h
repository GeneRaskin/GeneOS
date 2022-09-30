#ifndef PHYSICAL_MEMORY_MANAGER_H
#define PHYSICAL_MEMORY_MANAGER_H

#define BLOCK_SIZE      4096    //Size of a single block of memory
#define BLOCKS_PER_BYTE 8       //Each byte of a bitmap will hold 8 blocks
#define MEMORY_MAP_ADDR 0x200000 //Define the address of the memory map
#define MAX_BITMAP_SIZE_IN_BYTES 0x20000 //Max length of a bitmap
#define RESERVED_FOR_KERNEL_MAX_ADDR  0x400000 // Initial memory region for kernel
#include <stdint.h>
#include "../../cstd/include/libft.h"
#include "../../multiboot.h"

// Pointer to the memory map
extern uint32_t *memory_map;
extern uint32_t max_blocks;
extern uint32_t used_blocks;

void set_block(uint32_t bit);
void unset_block(uint32_t bit);
uint32_t test_block(uint32_t bit);
int32_t find_first_free_blocks(uint32_t num_blocks);
void    init_memory_manager(uint32_t addr, uint32_t size);
void    init_memory_region(uint32_t addr, uint32_t size);
void    deinit_memory_region(uint32_t addr, uint32_t size);
uint32_t *allocate_blocks(uint32_t num_blocks);
void free_blocks(uint32_t *addr, uint32_t num_blocks);
void load_physmmrmngr(multiboot_info_t *mbd);
void print_bitmap_region(uint32_t offset, uint32_t num_blocks);
void print_memory_stats();
void real_bitmap_stats();

void load_pdbr(uint32_t addr);
uint32_t get_pdbr();
void enable_paging(uint32_t b);
uint32_t is_paging_enabled();

#endif
