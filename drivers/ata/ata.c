#include "ata.h"
#include "../device.h"
#include "../../cstd/include/libft.h"
#include "../../memory/paging/paging.h"
#include "../../debug/debug.h"
#include "../../debug/errno.h"
#include "../fat32/fat32.h"
#include "../../interrupts/pit.h"

DEVICE *ata_devices[4];
int32_t n_ata_devices = 0;
uint8_t ide_buf[2048] = {};

void    ide_400ns_delay(uint16_t channel) {
    for (uint32_t i = 0; i < 15; i++)
        inb(channel + ATA_REG_ALTSTATUS);
}

void    ide_select_drive(uint8_t channel, uint8_t port) {
    if (channel == ATA_PRIMARY) {
        if (port == ATA_MASTER)
            outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
        else outb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    } else {
        if (port == ATA_MASTER)
            outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
        else outb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
    }
    ide_400ns_delay(channel);
}

uint8_t ide_poll(uint16_t channel) {
    uint8_t status;
    while (1) {
        status = inb(channel + ATA_REG_STATUS);
        if (status & ATA_SR_DF)
            return 2;
        if (status & ATA_SR_ERR)
            return 1;
        if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
            return 0;
    }
}

void    ide_disable_irq(uint8_t channel) {
    outb(channel + ATA_REG_CONTROL, 0x02);
}

void    ide_enable_irq(uint8_t channel) {
    outb(channel + ATA_REG_CONTROL, 0x0);
}

void    ide_read_buffer(uint8_t channel, uint8_t reg,
                        uint8_t *buffer, uint32_t quads) {
    for (uint32_t i = 0; i < quads; i++) {
        ((uint16_t *)buffer)[i] = inbw(channel + reg);
    }
}

uint32_t ata_ide_access(DEVICE *dev, void *buffer, uint8_t sec_count,
                        uint8_t direction) {
    uint8_t lba_mode, dma = 0;
    uint8_t lba_io[6], head, sect;
    ATA_DEVICE_INFO *dev_info = (ATA_DEVICE_INFO *) dev->type_specific_info;
    int32_t channel = dev_info->channel == ATA_PRIMARY ? ATA_PRIMARY_IO
                                                       : ATA_SECONDARY_IO;
    uint32_t words_per_sec = 256, lba = dev_info->pos;
    uint8_t slavebit = dev_info->drive, cmd, err;
    uint16_t cyl;
    if (dev_info->pos >= 0x10000000) { // must support LBA in this case
        // LBA48
        lba_mode = 2;
        lba_io[0] = (lba & 0x000000FF);
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = 0;
    } else if (dev_info->capabilities & 0x200) { // device supports LBA
        // LBA28
        lba_mode = 1;
        lba_io[0] = (lba & 0x000000FF);
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba & 0xF000000) >> 24;
    } else {
        // CHS
        lba_mode = 0;
        sect = (lba % 63) + 1;
        cyl = (lba + 1  - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba + 1  - sect) % (16 * 63) / (63);
    }
    while (inb(channel + ATA_REG_STATUS) & ATA_SR_BSY);
    if (lba_mode == 0)
        outb(channel + ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
    else
        outb(channel + ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);
    //ide_400ns_delay(channel);
    if (lba_mode == 2) {
        outb(channel + ATA_REG_SECCOUNT1, 0);
        outb(channel + ATA_REG_LBA3, lba_io[3]);
        outb(channel + ATA_REG_LBA4, lba_io[4]);
        outb(channel + ATA_REG_LBA5, lba_io[5]);
    }
    outb(channel + ATA_REG_SECCOUNT0, sec_count);
    outb(channel + ATA_REG_LBA0, lba_io[0]);
    outb(channel + ATA_REG_LBA1, lba_io[1]);
    outb(channel + ATA_REG_LBA2, lba_io[2]);
    if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
    if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;
    outb(channel + ATA_REG_COMMAND, cmd);

    if (direction == 0) {  // PIO read
        for (uint8_t i = 0; i < sec_count; ++i) {
            if ((err = ide_poll(channel)))
                return err;
            for (uint32_t i = 0; i < words_per_sec; i++) {
                ((uint16_t *)buffer)[i] = inbw(channel + ATA_REG_DATA);
            }
            buffer += words_per_sec * 2;
            dev_info->pos++;
        }
    } else { // PIO write
        for (uint8_t i = 0; i < sec_count; ++i) {
            if ((err = ide_poll(channel)))
                return err;
            outsb(channel, buffer, words_per_sec);
            buffer += words_per_sec;
            dev_info->pos++;
        }
        outb(channel + ATA_REG_COMMAND, (char []) {
            ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH,
            ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
        if ((err = ide_poll(channel)))
            return err;
    }
    return 0;
}

void    ata_seek(DEVICE *dev, uint32_t lba) {
    ATA_DEVICE_INFO *dev_info = (ATA_DEVICE_INFO *) dev->type_specific_info;
    dev_info->pos = lba;
}

int32_t ata_read_pio(DEVICE *dev, void *buffer, uint32_t sec_count) {
    return ata_ide_access(dev, buffer, sec_count, 0);
}

int32_t ata_write_pio(DEVICE *dev, void *buffer, uint32_t sec_count) {
    return ata_ide_access(dev, buffer, sec_count, 1);
}

void    ide_identify() {
    uint8_t type;
    for (uint32_t i = 0; i < 2; i++) {
        uint16_t io = (i == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
        for (uint32_t j = 0; j < 2; j++) {
            type = IDE_ATA;
            ide_select_drive(i, j);
            ide_disable_irq(io);
            outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            if (inb(io + ATA_REG_STATUS) == 0)
                continue;
            uint8_t status = ide_poll(io);
            if (status == 1) {
                uint8_t cl = inb(io + ATA_REG_LBA1);
                uint8_t ch = inb(io + ATA_REG_LBA2);
                if (cl == 0x14 && ch == 0xEB) {
                    type = IDE_ATAPI;
                } else if (cl == 0x69 && ch == 0x96) {
                    type = IDE_ATAPI;
                } else {
                    continue;
                }
                sys_sleep(1);
                outb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                sys_sleep(1);
            } else if (status == 2)
                continue;
            ide_read_buffer(io, ATA_REG_DATA, ide_buf, 256);
            ide_poll(io);
            DEVICE *new_ata = (DEVICE *) kmalloc(sizeof(DEVICE));
            ft_memset(new_ata, 0, sizeof(DEVICE));
            ATA_DEVICE_INFO *dev_info = (ATA_DEVICE_INFO *) kmalloc(sizeof(ATA_DEVICE_INFO));
            new_ata->type_specific_info = dev_info;
            dev_info->channel = i;
            dev_info->drive = j;
            dev_info->pos = 0;
            dev_info->command_sets = *((uint32_t *)(ide_buf + ATA_IDENT_COMMANDSETS));
            dev_info->capabilities = *((uint16_t *)(ide_buf + ATA_IDENT_CAPABILITIES));
            if (type == IDE_ATAPI)
                dev_info->name = "atapi";
            else
                dev_info->name = "ata";
            new_ata->read = ata_read_pio;
            new_ata->write = ata_write_pio;
            new_ata->seek = ata_seek;
            ata_devices[n_ata_devices++] = new_ata;
        }
    }
}

void    ls_ata() {
    for (int32_t i = 0; i < n_ata_devices; ++i) {
        ATA_DEVICE_INFO *dev_info = ata_devices[i]->type_specific_info;
        char *bus = dev_info->channel == ATA_PRIMARY ?
                "PRIMARY" : "SECONDARY";
        char *drive = dev_info->drive == ATA_MASTER ?
                "MASTER" : "SLAVE";
        ft_printf("ATA DEVICE type: %s, bus: %s, drive: %s\n",
                  dev_info->name, bus, drive);
    }
}

void    ata_init() {
    ide_identify();
    /* mount fat partitions */
    for (int32_t i = 0; i < n_ata_devices; ++i) {
        parse_mbr(ata_devices[i]);
    }
}
