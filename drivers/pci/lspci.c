#include "pci.h"
#include "pci_descriptors.h"
#include "../../cstd/include/libft.h"

void    print_device(PCI_DEVICE *pci_device) {
    const char *vendor_name = 0;
    const char *device_name = 0;
    const char *subclass_name = 0;
    const char *progIFName = 0;
    uint16_t vendor_id = 0, device_id = 0;
    uint8_t subclass_code = 0, class_code = 0, progIF = 0;

    if (pci_device->pci_dev_type == PCI_BRIDGE_DEVICE) {
        vendor_id = pci_device->pci_header.pci_header_bridge.vendor_id;
        device_id = pci_device->pci_header.pci_header_bridge.device_id;
        subclass_code = pci_device->pci_header.pci_header_bridge.subclass;
        class_code = pci_device->pci_header.pci_header_bridge.class_code;
        progIF = pci_device->pci_header.pci_header_bridge.prog_if;
    } else if (pci_device->pci_dev_type == PCI_TERMINAL_DEVICE) {
        vendor_id = pci_device->pci_header.pci_header_device.vendor_id;
        device_id = pci_device->pci_header.pci_header_device.dev_id;
        subclass_code = pci_device->pci_header.pci_header_device.subclass;
        class_code = pci_device->pci_header.pci_header_device.class_code;
        progIF = pci_device->pci_header.pci_header_device.prog_if;
    }
    vendor_name = GetVendorName(vendor_id);
    device_name = GetDeviceName(vendor_id, device_id);
    subclass_name = GetSubclassName(class_code, subclass_code);
    progIFName = GetProgIFName(class_code, subclass_code, progIF);
    ft_printf("%s / %s / %s / %s / %s\n", vendor_name,
              device_name, DeviceClasses[class_code], subclass_name, progIFName);
}

void    parse_device(PCI_DEVICE *curr_device) {
    PCI_DEVICE  *last_sibling = curr_device->sibling;
    print_device(curr_device);
    while (last_sibling) {
        print_device(last_sibling);
        last_sibling = last_sibling->sibling;
    }
}

void    parse_bus(PCI_BUS *curr_bus) {
    PCI_DEVICE *curr_device = curr_bus->devices;
    PCI_BUS    *last_child_bus = curr_bus->children;
    while (curr_device != 0) {
        parse_device(curr_device);
        if (curr_device->pci_dev_type == PCI_BRIDGE_DEVICE) {
            parse_bus(last_child_bus);
            last_child_bus = last_child_bus->next;
        }
        curr_device = curr_device->next;
    }
}

void    k_lspci() {
    parse_bus(pci_root);
}