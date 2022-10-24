#ifndef PCI_H
#define PCI_H

#include "../io.h"
#include "../../vfs/vfs.h"

/* PCI I/O ports */
#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

/* CONFIG_ADDRESS REGISTER */
typedef struct _CONFIG_ADDRESS_REG {
    uint8_t reg_offset;
    uint8_t func_num:3;
    uint8_t dev_num:5;
    uint8_t bus_num;
    uint8_t reserved:7;
    uint8_t enable_bit:1;
}__attribute__((packed)) CONFIG_ADDRESS_REG;

/* PCI HEADER TYPE 0x0 (device) */
typedef struct _PCI_HEADER_DEVICE {
    uint16_t vendor_id;
    uint16_t dev_id;
    uint16_t command;
    uint16_t status;
    uint8_t rev_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_addr;
    uint8_t capabilities_ptr;
    uint8_t reserved[7];
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
}__attribute__((packed)) PCI_HEADER_DEVICE;

/* PCI HEADER TYPE 0x1 (PCI-to-PCI bridge) */
typedef struct _PCI_HEADER_BRIDGE {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t rev_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint8_t primary_bus_num;
    uint8_t secondary_bus_num;
    uint8_t subordinate_bus_num;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetch_memory_base;
    uint16_t prefetch_memory_limit;
    uint32_t prefetch_base_upper32;
    uint32_t prefetch_limit_upper32;
    uint16_t io_base_upper16;
    uint16_t io_limit_upper16;
    uint8_t capability_ptr;
    uint8_t reserved[3];
    uint32_t expansion_rom_base_addr;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
}__attribute__((packed)) PCI_HEADER_BRIDGE;

typedef enum _PCI_DEV_TYPE {
    PCI_TERMINAL_DEVICE = 0x0,
    PCI_BRIDGE_DEVICE = 0x1
} PCI_DEV_TYPE;

struct _PCI_BUS;

/* PCI device linked list STRUCT */
typedef struct _PCI_DEVICE {
    PCI_DEV_TYPE pci_dev_type;
    union {
        PCI_HEADER_DEVICE pci_header_device;
        PCI_HEADER_BRIDGE pci_header_bridge;
    } pci_header;
    struct _PCI_DEVICE *next;
    struct _PCI_BUS *bus;
    struct _PCI_DEVICE *sibling;
} PCI_DEVICE;

/* PCI bus STRUCT */
typedef struct _PCI_BUS {
    uint8_t bus_num;
    struct _PCI_BUS *parent;
    struct _PCI_BUS *children;
    struct _PCI_BUS *next;
    PCI_DEVICE *devices;
} PCI_BUS;

extern PCI_BUS *pci_root;

CONFIG_ADDRESS_REG create_config_address(uint8_t reg_offset,
                                         uint8_t func_num,
                                         uint8_t dev_num,
                                         uint8_t bus_num);
uint32_t    pci_config_read_dword(CONFIG_ADDRESS_REG *addr);
void        pci_config_write_word(CONFIG_ADDRESS_REG *addr,
                               uint32_t data_dword);
uint16_t    pci_config_read_vendor_id(CONFIG_ADDRESS_REG *addr);
uint8_t     pci_config_get_device_type(CONFIG_ADDRESS_REG *addr);
PCI_DEVICE  *init_pci_device(CONFIG_ADDRESS_REG *addr,
                             PCI_BUS *parent);
PCI_BUS     *init_pci_bus(PCI_BUS *parent, uint8_t bus_num);
void        pci_enumerate();
PCI_DEVICE  *locate_device(uint32_t class_code, uint32_t subclass);
void        k_lspci();

#endif
