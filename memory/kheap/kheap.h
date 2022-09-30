#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include "../../ordered_array/ordered_array.h"

#define MAGIC_HEAP_NUM     0xCAFEBABE
#define KHEAP_START        0xC0400000
#define KHEAP_INITIAL_SIZE 0x100000
#define HEAP_MIN_SIZE      0x70000
#define HEAP_INDEX_SIZE    0x20000
#define KHEAP_MAX_ADDR     0xCFFFF000

typedef struct _header_t {
    uint32_t magic;
    uint8_t is_hole;
    uint32_t size;
} header_t;

typedef struct _footer_t {
   uint32_t magic;
   header_t *header;
} footer_t;

typedef struct _heap_t {
    ordered_array_t index_table; // index table of allocated blocks
    uint32_t start_addr; // the start of the heap
    uint32_t end_addr;   // the end of the heap
    uint32_t max_addr;   // the max address the heap can expand to
    uint8_t supervisor;  // requested pages privilege level
    uint8_t readonly;    // requested pages should be read-only
} heap_t;

/* the global variable holding the pointer to the
 * kernel heap */
extern heap_t   *kheap;

/* Create a new heap */
heap_t *create_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr,
                        uint8_t supervisor, uint8_t readonly);
/* Allocate a contiguous chunk of memory of size specified
 * by the first argument. If align is set to 1, creates a
 * new block starting on a page boundary*/
uint32_t    *alloc(uint32_t size, uint8_t page_align, heap_t *heap);
/* Release a block pointed to by p*/
void    free(void *p, heap_t *heap);

#endif
