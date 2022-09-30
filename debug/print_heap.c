#include "debug.h"
#include "../cstd/include/libft.h"

/*static void print_memory_heap(heap_t *heap, uint32_t num_dwords) {
    ft_printf("HEAP memory:\n");
    uint32_t *i = (uint32_t *)heap->start_addr;
    for (uint32_t iter = 0; iter < num_dwords; iter++) {
        ft_printf("%.8x ", i[iter]);
    }
}*/

void print_heap(heap_t *heap) {
    ft_printf("HEAP:\n");
    char *i = (char *)heap->start_addr;
    while ((uint32_t) i < heap->end_addr && ((header_t *)i)->magic == MAGIC_HEAP_NUM) {
        ft_printf("Block addr: %p ", i);
        ft_printf("Size: %#.8x is_hole: %u\n", ((header_t *)i)->size,
                  ((header_t *)i)->is_hole);
        i += ((header_t *)i)->size;
    }
}

void print_heap_index(heap_t *heap) {
    ft_printf("HEAP index:\n");
    for (uint32_t i = 0; i < heap->index_table.size; ++i) {
        header_t *header = heap->index_table.array[i];
        if (header->magic == MAGIC_HEAP_NUM) {
            ft_printf("Hole addr: %p ", header);
            ft_printf("Size: %#.8x is_hole: %u\n", header->size, header->is_hole);
        }
    }
}
