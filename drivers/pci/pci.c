#include "pci.h"
#include "../../cstd/include/libft.h"
#include "../../memory/paging/paging.h"

PCI_BUS *pci_root = 0;
static uint8_t bus_num = 0;

CONFIG_ADDRESS_REG create_config_address(uint8_t reg_offset,
                                         uint8_t func_num,
                                         uint8_t dev_num,
                                         uint8_t bus_num) {
    CONFIG_ADDRESS_REG config_address_reg;
    ft_memset(&config_address_reg, 0, sizeof(config_address_reg));
    config_address_reg.reg_offset = reg_offset;
    config_address_reg.func_num = func_num & 0x7;
    config_address_reg.dev_num = dev_num & 0x1F;
    config_address_reg.bus_num = bus_num;
    config_address_reg.enable_bit = 1;
    return config_address_reg;
}

uint32_t config_addr_reg_to_dword(CONFIG_ADDRESS_REG *addr) {
    uint32_t address;
    uint32_t lbus = (uint32_t) addr->bus_num;
    uint32_t ldev = (uint32_t) addr->dev_num;
    uint32_t lfunc = (uint32_t) addr->func_num;
    uint8_t  offset = addr->reg_offset;
    address = (uint32_t) ((lbus << 16) | (ldev << 11) |
            (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    return address;
}

uint32_t pci_config_read_dword(CONFIG_ADDRESS_REG *addr) {
    outbl(PCI_CONFIG_ADDR, config_addr_reg_to_dword(addr));
    /* Retrieve the dword */
    return inbl(PCI_CONFIG_DATA);
}

void     pci_config_write_word(CONFIG_ADDRESS_REG *addr,
                               uint32_t data_dword) {
    outbl(PCI_CONFIG_ADDR, config_addr_reg_to_dword(addr));
    outbl(PCI_CONFIG_DATA, data_dword);
}

void    pci_config_read_all(CONFIG_ADDRESS_REG *addr, PCI_DEVICE *pci_dev) {
    uint32_t    *config_space;
    uint8_t     tmp_offset = addr->reg_offset;
    config_space = (uint32_t *) &(pci_dev->pci_header);
    for (uint32_t i = 0; i < 0x10; ++i) {
        config_space[i] = pci_config_read_dword(addr);
        addr->reg_offset += 0x4;
    }
    addr->reg_offset = tmp_offset;
}

uint16_t    pci_config_read_vendor_id(CONFIG_ADDRESS_REG *addr) {
    uint32_t reg = pci_config_read_dword(addr);
    return (uint16_t)(reg & 0xFFFF);
}

uint8_t     pci_config_get_device_type(CONFIG_ADDRESS_REG *addr) {
    uint8_t tmp_offset = addr->reg_offset;
    addr->reg_offset = 0xC;
    uint32_t info = pci_config_read_dword(addr);
    addr->reg_offset = tmp_offset;
    return (uint8_t)((info >> 16) & 0xFF);
}

PCI_DEVICE  *init_pci_device(CONFIG_ADDRESS_REG *addr, PCI_BUS *parent) {
    PCI_DEVICE *new_device = (PCI_DEVICE *) kmalloc(sizeof(PCI_DEVICE));
    ft_memset(new_device, 0, sizeof(PCI_DEVICE));
    new_device->bus = parent;
    pci_config_read_all(addr, new_device);
    return new_device;
}

PCI_BUS *init_pci_bus(PCI_BUS *parent, uint8_t bus_num) {
    PCI_BUS *new_bus = (PCI_BUS *) kmalloc(sizeof(PCI_BUS));
    ft_memset(new_bus, 0, sizeof(PCI_BUS));
    new_bus->bus_num = bus_num;
    new_bus->parent = parent;
    return new_bus;
}

void    check_device(uint8_t curr_bus_num, uint8_t curr_dev_num,
                     PCI_DEVICE *curr_device, PCI_BUS *parent) {
    PCI_DEVICE  *new_function;
    uint8_t     type_header = 0;
    CONFIG_ADDRESS_REG curr_addr = create_config_address(0, 0,
                                      curr_dev_num, curr_bus_num);
    switch (curr_device->pci_dev_type) {
        case PCI_BRIDGE_DEVICE:
            type_header = curr_device->pci_header.pci_header_bridge.header_type;
            break;
        case PCI_TERMINAL_DEVICE:
            type_header = curr_device->pci_header.pci_header_device.header_type;
    }
    if (type_header & 0x80) { // device is multifunctional
        for (uint32_t function_cnt = 0; function_cnt < 8; ++function_cnt) {
            curr_addr.func_num = function_cnt; // set the current function number
            if (pci_config_read_vendor_id(&curr_addr) == 0xFFFF) // empty function
                continue;
            new_function = init_pci_device(&curr_addr, parent);
            new_function->next = curr_device->next;
            curr_device->sibling = new_function;
            curr_device = new_function;
        }
    }
}

void    check_bus(uint8_t curr_bus_num, PCI_BUS *curr_bus) {
    PCI_DEVICE *last_device = curr_bus->devices;
    PCI_BUS    *last_bus = curr_bus->children;
    CONFIG_ADDRESS_REG curr_addr = create_config_address(
            0, 0, 0, curr_bus_num);
    for (uint32_t device_num = 0; device_num < 32; ++device_num) {
        curr_addr.dev_num = device_num;
        if (pci_config_read_vendor_id(&curr_addr) == 0xFFFF) // empty
            continue;
        PCI_DEVICE *curr_device = init_pci_device(&curr_addr, curr_bus);
        if (last_device) {
            last_device->next = curr_device;
            last_device = last_device->next;
        } else {
            last_device = curr_device;
            curr_bus->devices = last_device;
        }
        check_device(curr_bus_num, device_num, curr_device, curr_bus);
        if (curr_device->pci_dev_type == PCI_BRIDGE_DEVICE) { // if bridge, create a new bus
            bus_num++;
            PCI_BUS *new_bus = init_pci_bus(curr_bus, bus_num);
            if (last_bus) {
                last_bus->next = new_bus;
                last_bus = last_bus->next;
            } else {
                last_bus = new_bus;
                curr_bus->children = new_bus;
            }
            check_bus(bus_num, new_bus);
        }
    }
}

void    pci_enumerate() {
    // init the pci_root of the PCI tree
    pci_root = init_pci_bus(0, 0);
    check_bus(0, pci_root);
    ft_printf("PCI enumeration complete\n");
}
