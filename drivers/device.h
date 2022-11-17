#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

struct _FSYS;

#define ATA_DEV_TYPE    0x0
#define PCI_DEV_TYPE    0x1

typedef struct _DEVICE {
    uint32_t        dev_type;
    void            *type_specific_info;
    int32_t         (*read)(struct _DEVICE *, void *, uint32_t);
    int32_t         (*write)(struct _DEVICE *, void *, uint32_t);
    void            (*seek)(struct _DEVICE *, uint32_t);
    struct _FSYS     *fs;
} DEVICE;

#endif
