#include "pci.h"
#include "../../cstd/include/libft.h"

static uint32_t    check_device(PCI_DEVICE *curr_device, uint32_t class_code,
                                uint32_t subclass) {
    PCI_HEADER_DEVICE curr_device_header = curr_device->pci_header.pci_header_device;

    if (curr_device_header.subclass == subclass &&
        curr_device_header.class_code == class_code)
        return 1;
    return 0;
}

PCI_DEVICE   *search_device(PCI_DEVICE *curr_device, uint32_t class_code,
                              uint32_t subclass) {
    if (check_device(curr_device, class_code, subclass))
        return curr_device;
    PCI_DEVICE  *last_sibling = curr_device->sibling;
    while (last_sibling) {
        if (check_device(last_sibling, class_code, subclass))
            return last_sibling;
        last_sibling = last_sibling->sibling;
    }
    return 0;
}

PCI_DEVICE  *search_bus(PCI_BUS *curr_bus, uint32_t class_code, uint32_t subclass) {
    PCI_DEVICE *curr_device = curr_bus->devices;
    PCI_DEVICE *found_device;
    PCI_BUS    *last_child_bus = curr_bus->children;
    while (curr_device != 0) {
        found_device = search_device(curr_device, class_code, subclass);
        if (found_device)
            return found_device;
        if (curr_device->pci_dev_type == PCI_BRIDGE_DEVICE) {
            found_device = search_bus(last_child_bus, class_code, subclass);
            if (found_device)
                return found_device;
            last_child_bus = last_child_bus->next;
        }
        curr_device = curr_device->next;
    }
    return 0;
}

PCI_DEVICE  *locate_device(uint32_t class_code, uint32_t subclass) {
    return search_bus(pci_root, class_code, subclass);
}