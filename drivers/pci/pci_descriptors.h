#ifndef PCI_DESCRIPTORS_H
#define PCI_DESCRIPTORS_H

#include <stdint.h>

extern const char *DeviceClasses[20];

const char *GetVendorName(uint16_t vendorID);
const char *GetDeviceName(uint16_t vendorID, uint16_t deviceID);
const char *MassStorageControllerSubclassName(uint8_t subclassCode);
const char *SerialBusControllerSubclassName(uint8_t subclassCode);
const char *BridgeDeviceSubclassName(uint8_t subclassCode);
const char *GetSubclassName(uint8_t classCode, uint8_t subclassCode);
const char *GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF);

#endif
