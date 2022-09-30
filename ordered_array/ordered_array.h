#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <stdint.h>

/* A comparator should return negative value if the first arg
 * is less than the second. Otherwise, it should return
 * zero. */
typedef uint8_t (*comp)(void *, void *);

typedef struct _ordered_array_t {
    void    **array;
    uint32_t size;
    uint32_t max_size;
    comp comparator;

} ordered_array_t;

/* Standard comparator, finds whether left is less than right */
uint8_t standard_comparator(void *a, void *b);

ordered_array_t create_ordered_array(uint32_t, comp comparator);
ordered_array_t place_ordered_array(void *addr, uint32_t max_size, comp comparator);
//void            destroy_ordered_array(ordered_array_t *arr);
int32_t         insert_ordered_array(ordered_array_t *arr, void *item);
/* Lookup an item at index specified by the idx arg */
void            *lookup_ordered_array(ordered_array_t *arr, uint32_t idx);
/* Destroy an item at index specified by the idx arg */
void            remove_ordered_array(ordered_array_t *arr, uint32_t idx);
/* Update an item's position in case its fields were changed */
void            update_item_position(ordered_array_t *arr, uint32_t idx);

#endif
