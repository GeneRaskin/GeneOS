#include "physical_memory_manager.h"

uint32_t *memory_map;
uint32_t max_blocks;
uint32_t used_blocks;

void set_block(uint32_t block) {
    /*
     * Divide bit by 32 to get a chunk of memory containing
     * the exact bit to be set. Shift 1 to the left by remainder
     * of bit divided by 32. OR to set that bit*/
    memory_map[block / 0x20] |= (1 << (block % 0x20));
}

void unset_block(uint32_t block) {
    /*
     * Divide bit by 32 to get a chunk of memory containing
     * the exact bit to be unset. Shift 1 to the left by remainder
     * of bit divided by 32. AND with the inverse of bit to
     * clear it*/
    memory_map[block / 0x20] &= ~(1 << (block % 0x20));
}

uint32_t test_block(uint32_t block) {
    return memory_map[block / 0x20] & (1 << (block % 0x20));
}

int32_t find_first_free_blocks(uint32_t num_blocks) {
    if (num_blocks == 0)
        return -1; //can't allocate 0 blocks apparently
    for (uint32_t curr_block = 0; curr_block <= max_blocks - num_blocks;) {
        uint32_t free_blocks = 0;
        while (curr_block + free_blocks < max_blocks &&
                !test_block(curr_block + free_blocks)) {
            free_blocks++;
            if (free_blocks == num_blocks) {
                return curr_block;
            }
        }
        if (free_blocks != 0)
            curr_block += free_blocks + 1;
        else
            curr_block++;
    }
    return -1; // couldn't allocate contiguous num_blocks
}

void init_memory_manager(uint32_t addr, uint32_t size) {
    memory_map = (uint32_t *) addr;
    max_blocks = size / BLOCK_SIZE;
    used_blocks = 0;
    deinit_memory_region(0x0, size);
}

// Initialize a memory region (set blocks as free)
void init_memory_region(uint32_t addr, uint32_t size) {
    int32_t align = addr / BLOCK_SIZE;
    int32_t num_blocks = size / BLOCK_SIZE;

    for (; num_blocks > 0; num_blocks--) {
        unset_block(align++);
        used_blocks--;
    }
}

// De-initialize a memory region, and set blocks
// as used or reserved
void deinit_memory_region(uint32_t addr, uint32_t size) {
    int32_t align = addr / BLOCK_SIZE;
    int32_t num_blocks = size / BLOCK_SIZE;

    for (; num_blocks > 0; num_blocks--) {
        set_block(align++);
        used_blocks++;
    }
}

// Alloc blocks of physical memory
uint32_t *allocate_blocks(uint32_t num_blocks) {
    if ((max_blocks - used_blocks) < num_blocks)
        return 0; // can't allocate more than available
    int32_t starting_block = find_first_free_blocks(num_blocks);
    if (starting_block == -1)
        return 0;
    for (uint32_t i = 0; i < num_blocks; ++i) {
        set_block(starting_block + i);
    }
    used_blocks += num_blocks;
    uint32_t addr = starting_block * BLOCK_SIZE;
    return (uint32_t *)addr;
}

// Free blocks of physical memory
void free_blocks(uint32_t *addr, uint32_t num_blocks) {
    int32_t starting_block = (uint32_t) addr / BLOCK_SIZE;

    for (uint32_t i = 0; i < num_blocks; i++) {
        unset_block(starting_block + i);
    }
    used_blocks -= num_blocks;
}

void real_bitmap_stats() {
    uint32_t real_used_blocks = 0;
    for (uint32_t i = 0; i < max_blocks; ++i) {
        if (test_block(i))
            real_used_blocks++;
    }
    ft_printf("Real total blocks used or reserved: %d ", real_used_blocks);
    ft_printf("Real free blocks: %d\n", max_blocks - real_used_blocks);
}

void print_memory_stats() {
    ft_printf("Total blocks initialized: %d; used or reserved blocks: %d\n", max_blocks,
              used_blocks);
    ft_printf("free blocks: %d\n", max_blocks - used_blocks);
}

void load_physmmrmngr(multiboot_info_t *mbd) {
    ft_printf("Physical memory manager initialization...\n");
    uint64_t total_ram_size = 0; // total RAM size in bytes

    // Calculate the total size of all available RAM
    for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t *mmmt = (multiboot_memory_map_t *) (mbd->mmap_addr + i);
        uint64_t curr_region_size = ((uint64_t)mmmt->len_low + ((uint64_t)mmmt->len_high << 0x20));
        total_ram_size += curr_region_size;
        if (total_ram_size > UINT32_MAX) {
            total_ram_size = UINT32_MAX;
            break;
        }
    }
    init_memory_manager(MEMORY_MAP_ADDR, (uint32_t) total_ram_size);

    // Free blocks which are available
    for (uint32_t i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t *mmmt = (multiboot_memory_map_t *) (mbd->mmap_addr + i);
        uint64_t curr_region_size = ((uint64_t)mmmt->len_low + ((uint64_t)mmmt->len_high << 0x20));
        uint64_t addr = (uint64_t)mmmt->addr_low + ((uint64_t)mmmt->addr_high << 0x20);
        uint32_t short_addr = (uint32_t) addr;
        uint32_t short_region_size = (uint32_t) curr_region_size;
        if (addr + curr_region_size > UINT32_MAX) {
            if (addr < UINT32_MAX)
                short_region_size = UINT32_MAX - short_addr;
            else
                break;
        }
        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            init_memory_region(short_addr, short_region_size);
    }

    // Always set the first N MB of memory as occupied
    // to protect important data such as I/O, bitmaps, kernel etc.
    uint32_t protected_blocks = (RESERVED_FOR_KERNEL_MAX_ADDR) / BLOCK_SIZE;
    for (uint32_t i = 0; i < protected_blocks; ++i) {
        if (!test_block(i)) {
            set_block(i);
            used_blocks++;
        }
    }
    ft_printf("Total RAM %d KB\n", (uint32_t)total_ram_size / 0x400);
}

// print a specific bitmap region
void print_bitmap_region(uint32_t offset, uint32_t num_blocks) {
    for (uint32_t i = 0; i < num_blocks && i + offset * 0x20 < max_blocks; ++i) {
        ft_printf("%d", (memory_map[offset + i / 0x20] & (1 << (i % 0x20))) >> i % 0x20);
    }
    ft_printf("\n");
}
