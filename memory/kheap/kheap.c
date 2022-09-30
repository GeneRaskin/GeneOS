#include "kheap.h"
#include "../../debug/debug.h"
#include "../paging/paging.h"

heap_t *kheap = 0;

// Find the smallest hole that is big enough to contain requested block size
int32_t find_smallest_hole(uint32_t size, uint8_t align, heap_t *heap) {
    uint32_t i = 0;
    while (i < heap->index_table.size) {
        header_t *header = (header_t *) lookup_ordered_array(&heap->index_table, i);
        if (align) {
            uint32_t addr = (uint32_t) header;
            int32_t offset = 0;
            if (((addr + sizeof(header_t)) & 0xFFFFF000) != 0)
                offset = 0x1000 - (addr + sizeof(header_t)) % 0x1000;
            int32_t hole_size = (int32_t)header->size - offset;
            if (hole_size >= (int32_t)size)
                break;
        } else if (header->size >= size)
            break;
        i++;
    }
    if (i == heap->index_table.size) {
        return -1; // the hole was not found
    } else
        return i; // return index of the suitable hole in the index table
}

// Expand the heap to the new_size; new_size is in bytes
void expand(uint32_t new_size, heap_t *heap) {
    // check arguments first; if too small, return immediately
    if (new_size <= heap->end_addr - heap->start_addr) {
        return;
    }
    // if the requested size is not page-aligned, fix it
    if ((new_size & 0xFFFFF000) != 0) {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }
    // if the new size is too big, return immediately
    if (heap->start_addr + new_size > heap->max_addr) {
        return;
    }
    uint32_t old_size = heap->end_addr - heap->start_addr;
    uint32_t i = old_size;
    while (i < new_size) {
        // if failed to request a page, return immediately
        if (!request_page(heap->start_addr + i, heap->supervisor,
                            heap->readonly)) {
            return;
        }
        i += 0x1000; // get next frame
    }
    heap->end_addr = heap->start_addr + new_size;
}

// contract the heap to the new_size; new_size is specified in bytes
int32_t contract(uint32_t new_size, heap_t *heap) {
    // if new_size is bigger than old, return immediately
    if (new_size >= heap->end_addr - heap->start_addr)
        return -1;
    // fix page-alignment
    if (new_size & 0x1000) {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }
    // don't contract beyond the minimum size
    if (new_size < HEAP_MIN_SIZE)
        return -1;
    uint32_t old_size = heap->end_addr - heap->start_addr;
    uint32_t i = old_size - 0x1000;
    while (new_size < i) {
        free_page(get_page(curr_directory, heap->start_addr + i));
        i -= 0x1000;
    }
    heap->end_addr = heap->start_addr + new_size;
    return new_size;
}

uint8_t headers_comparator(void *a, void *b) {
    return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

heap_t *create_heap(uint32_t start_addr, uint32_t end_addr, uint32_t max_addr,
                    uint8_t supervisor, uint8_t readonly) {
    /* If either start_addr or end_addr are not page-aligned, return
       immediately */
    if (start_addr % 0x1000 != 0 || end_addr % 0x1000 != 0) {
        return 0;
    }
    heap_t  *heap = (heap_t *) kmalloc(sizeof(heap_t));
    start_addr += sizeof(heap_t);
    // initialize the index table
    heap->index_table = place_ordered_array((void *)start_addr, HEAP_INDEX_SIZE,
                                            headers_comparator);
    start_addr += sizeof(void *) * HEAP_INDEX_SIZE;
    // Ensure that the start address is still page-aligned
    if ((start_addr & 0xFFFFF000) != 0) {
        start_addr &= 0xFFFFF000;
        start_addr += 0x1000;
    }
    // Set the remaining field in the heap struct
    heap->start_addr = start_addr;
    heap->end_addr = end_addr;
    heap->max_addr = max_addr;
    heap->supervisor = supervisor;
    heap->readonly = readonly;
    // Initialize one large hole in the index table
    header_t *hole = (header_t *) start_addr;
    hole->size = end_addr - start_addr;
    hole->magic = MAGIC_HEAP_NUM;
    hole->is_hole = 1;
    footer_t *hole_footer = (footer_t *)(heap->end_addr - sizeof(footer_t));
    hole_footer->magic = MAGIC_HEAP_NUM;
    hole_footer->header = hole;
    insert_ordered_array(&heap->index_table, (void *) hole);

    return heap;
}

uint32_t    *alloc(uint32_t size, uint8_t page_align, heap_t *heap) {
    // add sizes of header and footer to the requested size
    uint32_t new_size = size + sizeof(header_t) + sizeof(footer_t);
    // find the smallest hole that is large enough
    int32_t i = find_smallest_hole(new_size, page_align, heap);
    if (i == -1) { // if hole was not found
        // save the current heap state
        uint32_t old_len = heap->end_addr - heap->start_addr;
        uint32_t old_end_addr = heap->end_addr;
        // allocate more space
        expand(old_len + new_size, heap);
        uint32_t new_len = heap->end_addr - heap->start_addr;
        // find the endmost header by location in the heap
        i = 0;
        int32_t idx = -1;
        uint32_t value = 0x0;
        while (i < (int32_t)heap->index_table.size) {
            uint32_t tmp = (uint32_t) lookup_ordered_array(&heap->index_table,i);
            if (tmp > value) {
                value = tmp;
                idx = i;
            }
            i++;
        }
        if (idx == -1) { // if no headers found, add one
            header_t *header = (header_t *)old_end_addr;
            header->magic = MAGIC_HEAP_NUM;
            header->size = new_len - old_len;
            header->is_hole = 1;
            footer_t *footer = (footer_t *)(old_end_addr + header->size - sizeof(footer_t));
            footer->magic = MAGIC_HEAP_NUM;
            footer->header = header;
            insert_ordered_array(&heap->index_table, (void *)header);
        } else { // adjust the rightmost header
            header_t *header = lookup_ordered_array(&heap->index_table, idx);
            header->size += new_len - old_len;
            footer_t *footer = (footer_t *) ((uint32_t)header + header->size - sizeof(footer_t));
            footer->header = header;
            footer->magic = MAGIC_HEAP_NUM;
            update_item_position(&heap->index_table, idx);
        }
        return alloc(size, page_align, heap); // try to allocate a block once more
    }
    // get the header of the suitable hole
    header_t *suitable_hole_header = (header_t *) lookup_ordered_array(&heap->index_table, i);
    uint32_t suitable_hole_pos = (uint32_t) suitable_hole_header;
    uint32_t suitable_hole_size = suitable_hole_header->size;
    // if you can't split the suitable hole into two smaller ones...
    if (suitable_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
        // just increase the requested size
        size += suitable_hole_size - new_size;
        new_size = suitable_hole_size;
    }
    // if page align was requested, make a new hole to the left of the block
    if (page_align && suitable_hole_pos & 0xFFFFF000) {
        // calculate the new address of the occupied hole
        uint32_t new_addr = (suitable_hole_pos & 0xFFFFF000) + 0x1000 - sizeof(header_t);
        header_t *hole_header = (header_t *)suitable_hole_pos;
        hole_header->size = 0x1000 - (suitable_hole_pos & 0xFFF) - sizeof(header_t);
        hole_header->magic = MAGIC_HEAP_NUM;
        hole_header->is_hole = 1;
        footer_t *hole_footer = (footer_t *) ((uint32_t)new_addr - sizeof(footer_t));
        hole_footer->magic = MAGIC_HEAP_NUM;
        hole_footer->header = hole_header;
        suitable_hole_pos = new_addr;
        suitable_hole_size -= hole_header->size;
        remove_ordered_array(&heap->index_table, i);
        insert_ordered_array(&heap->index_table, hole_header);
    } else {
        // otherwise just delete this hole from the index table
        remove_ordered_array(&heap->index_table, i);
    }
    // update the header and footer of the suitable hole
    header_t *suitable_block_header = (header_t *)suitable_hole_pos;
    suitable_block_header->magic = MAGIC_HEAP_NUM;
    suitable_block_header->is_hole = 0;
    suitable_block_header->size = new_size;
    footer_t *suitable_block_footer = (footer_t *) (suitable_hole_pos + sizeof(header_t) + size);
    suitable_block_footer->magic = MAGIC_HEAP_NUM;
    suitable_block_footer->header = suitable_block_header;
    /* if there is space left to the right of the requested block,
     * allocate a new hole */
    if ((int32_t)(suitable_hole_size - new_size) > 0) {
        header_t *hole_header = (header_t *)(suitable_hole_pos + sizeof(header_t) + sizeof(footer_t)
                + size);
        hole_header->magic = MAGIC_HEAP_NUM;
        hole_header->is_hole = 1;
        hole_header->size = suitable_hole_size - new_size;
        footer_t *hole_footer = (footer_t *) ((uint32_t)hole_header + suitable_hole_size - new_size -
                sizeof(footer_t));
        if ((uint32_t)hole_footer < heap->end_addr) {
            hole_footer->magic = MAGIC_HEAP_NUM;
            hole_footer->header = hole_header;
        }
        insert_ordered_array(&heap->index_table, (void *)hole_header);
    }
    return (uint32_t *)((uint32_t)suitable_block_header + sizeof(header_t));
}

int32_t locate_header_in_index_table(heap_t *heap, void *header) {
    uint32_t i = 0;
    // locate this header and update its position
    while (i < heap->index_table.size &&
           lookup_ordered_array(&heap->index_table, i) !=
           (void *)header)
        i++;
    // if the header hasn't been found, return an error code to a caller
    if (i == heap->index_table.size)
        return -1;
    return i;
}

void    free(void *p, heap_t *heap) {
    if (p == 0) // if null pointer, return immediately
        return;
    // get the header and footer
    header_t *header = (header_t *)((uint32_t)p - sizeof(header_t));
    footer_t *footer = (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));
    // check magic numbers; if pointer is invalid, return immediately
    if (header->magic != MAGIC_HEAP_NUM || footer->magic != MAGIC_HEAP_NUM)
        return;
    header->is_hole = 1;
    uint8_t do_add = 1; // do we need to add this header into the index table?
    int32_t inserted_pos = 0; // where does the new entry go?
    /* unify left logic
     * check whether there is a footer immediately to the left*/
    footer_t *test_footer = (footer_t *) ((uint32_t) header - sizeof(footer_t));
    if (test_footer->magic == MAGIC_HEAP_NUM && test_footer->header->is_hole) {
        uint32_t old_size = header->size; // save the current size
        header = test_footer->header; // rewrite the header
        footer->header = header; // rewrite the footer
        header->size += old_size;
        do_add = 0; // since this header is already in the index, don't add it
        int32_t i = locate_header_in_index_table(heap, header);
        if (i == -1) // return if an error has occurred
            return;
        update_item_position(&heap->index_table, i);
        inserted_pos = i;
    }
    /* unify right logic
     * check whether there is a header to the right of the
     * block*/
    header_t *test_header = (header_t *) ((uint32_t)footer + sizeof(footer_t));
    if (test_header->magic == MAGIC_HEAP_NUM && test_header->is_hole) {
        header->size += test_header->size;
        test_footer = (footer_t *)((uint32_t)test_header + test_header->size -
                sizeof(footer_t));
        footer = test_footer;
        footer->header = header;
        int32_t i = locate_header_in_index_table(heap, test_header);
        if (i == -1) // return if an error has occurred
            return;
        remove_ordered_array(&heap->index_table, i);
    }
    if (do_add == 1)
        inserted_pos = insert_ordered_array(&heap->index_table, (void *)header);
    if (inserted_pos == -1) // if an error has occurred, quit
        return;
    // if the footer is the end addr, it is possible to contract the heap
    if ((uint32_t)footer + sizeof(footer_t) == heap->end_addr) {
        uint32_t old_len = heap->end_addr - heap->start_addr;
        int32_t new_len = contract((uint32_t)header
                - heap->start_addr, heap);
        if (new_len != -1) { // if didn't fail to contract
            // if we still fit, resize
            if ((int32_t)(header->size - (old_len - new_len)) > 0) {
                header->size -= old_len - new_len;
                footer = (footer_t *) ((uint32_t) header + header->size
                                       - sizeof(footer_t));
                footer->magic = MAGIC_HEAP_NUM;
                footer->header = header;
                update_item_position(&heap->index_table, inserted_pos);
            } else // otherwise remove the block from index table
                remove_ordered_array(&heap->index_table, inserted_pos);
        }
    }
}
