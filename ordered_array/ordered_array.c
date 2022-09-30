#include "ordered_array.h"
#include "../memory/paging/paging.h"

/* Swap pointers of an index table */
static void swap(void **a, void **b) {
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

uint8_t standard_comparator(void *a, void *b) {
    return (a < b) ? 1 : 0;
}

ordered_array_t create_ordered_array(uint32_t max_size, comp comparator) {
    ordered_array_t ordered_arr;
    ordered_arr.array = (void *) kmalloc(max_size * sizeof(void *));
    ft_memset(ordered_arr.array, 0, max_size * sizeof(void *));
    ordered_arr.size = 0;
    ordered_arr.max_size = max_size;
    ordered_arr.comparator = comparator;
    return ordered_arr;
}

ordered_array_t place_ordered_array(void *addr, uint32_t max_size, comp comparator) {
    ordered_array_t ordered_arr;
    ordered_arr.array = addr;
    ft_memset(ordered_arr.array, 0, max_size * sizeof(void *));
    ordered_arr.size = 0;
    ordered_arr.max_size = max_size;
    ordered_arr.comparator = comparator;
    return ordered_arr;
}

int32_t   insert_ordered_array(ordered_array_t *arr, void *item) {
    // if array is already full return immediately
    if (arr->size == arr->max_size) {
        return -1;
    }
    uint32_t i = 0, pos;
    while (i < arr->size && arr->comparator(arr->array[i], item))
        i++;
    pos = i;
    // if reached end of array just append an item
    if (i == arr->size) {
        arr->array[arr->size++] = item;
    } else {
        void *tmp = arr->array[i];
        arr->array[i] = item;
        while (i < arr->size) {
            i++;
            void *tmp2 = arr->array[i];
            arr->array[i] = tmp;
            tmp = tmp2;
        }
        arr->size++;
    }
    return pos;
}

void    *lookup_ordered_array(ordered_array_t *arr, uint32_t idx) {
    // if idx out of bounds, return NULL
    if (idx >= arr->size) {
        return 0;
    }
    return arr->array[idx];
}

void    remove_ordered_array(ordered_array_t *arr, uint32_t idx) {
    // if idx out of bounds, return immediately
    if (idx >= arr->size) {
        return;
    }
    while (idx < arr->size) {
        arr->array[idx] = arr->array[idx + 1];
        idx++;
    }
    arr->size--;
}

static void move_up(ordered_array_t *arr, uint32_t idx) {
    uint32_t iter = 1;
    comp cmp = arr->comparator;
    void *item = lookup_ordered_array(arr, idx);
    void *next_item = lookup_ordered_array(arr, idx + iter);
    while (!cmp(item, next_item)) {
        swap(&next_item, &item);
        item = lookup_ordered_array(arr, idx + iter);
        iter++;
        if (idx + iter == arr->size)
            return;
        next_item = lookup_ordered_array(arr, idx + iter);
    }
}

static void move_down(ordered_array_t *arr, uint32_t idx) {
    uint32_t iter = 1;
    comp cmp = arr->comparator;
    void *item = lookup_ordered_array(arr, idx);
    void *prev_item = lookup_ordered_array(arr, idx - iter);
    while (cmp(item, prev_item)) {
        swap(&prev_item, &item);
        item = lookup_ordered_array(arr, idx - iter);
        iter++;
        if (idx + 1 - iter == 0)
            return;
        prev_item = lookup_ordered_array(arr, idx - iter);
    }
}

void    update_item_position(ordered_array_t *arr, uint32_t idx) {
    if (arr->size < 2) // return if array is empty or contains one item
        return;
    comp cmp = arr->comparator;
    void *item = lookup_ordered_array(arr, idx);
    if (idx == 0)
        move_up(arr, idx);
    else if (idx == arr->size - 1)
        move_down(arr, idx);
    else if (idx >= arr->size) {
        return;
    } else {
        void *next_item = lookup_ordered_array(arr, idx + 1);
        void *prev_item = lookup_ordered_array(arr, idx - 1);
        if (!cmp(item, next_item))
            move_up(arr, idx);
        else if (cmp(item, prev_item))
            move_down(arr, idx);
    }
}
