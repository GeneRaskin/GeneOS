#ifndef MBR_H
#define MBR_H

#define PART_ONE_OFFSET   0x01BE
#define PART_TWO_OFFSET   0x01CE
#define PART_THREE_OFFSET 0x01DE
#define PART_FOUR_OFFSET  0x01EE

#include <stdint.h>

typedef struct _MBR_ENTRY {
    uint8_t     boot_flag;
    uint8_t     start_head;
    uint16_t    start_sec_cyl;
    uint8_t     system_id;
    uint8_t     end_head;
    uint16_t    end_sec_cyl;
    uint32_t    rel_sec;
    uint32_t    total_sec;
}__attribute__((packed)) MBR_ENTRY;

#endif
