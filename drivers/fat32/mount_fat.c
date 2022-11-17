#include "fat32.h"
#include "../../memory/paging/paging.h"
#include "../../memory/kheap/kheap.h"
#include "mbr.h"

void    print_fat_info(MOUNT_INFO *mount_info) {
    ft_printf("Mounted FAT partition\n");
    ft_printf("    sec per cls: %d\n", mount_info->sec_per_Cls);
    ft_printf("    first data sec: %d\n", mount_info->first_data_sec);
    ft_printf("    total sec count: %d\n", mount_info->total_sec);
    ft_printf("    reserved sec count: %d\n", mount_info->reserved_sec);
    ft_printf("    root dir size: %d\n", mount_info->root_dir_size);
}

/* for now, mount first partition only */
uint8_t parse_mbr(DEVICE *dev) {
    uint8_t *buf = (uint8_t *) kmalloc(512);
    MBR_ENTRY *mbr_entry;
    BOOT_SECTOR *boot_sector;
    MOUNT_INFO *mount_info;
    uint8_t rel_sec, tot_sec;
    dev->seek(dev, 0);
    dev->read(dev, buf, 1);
    mbr_entry = (MBR_ENTRY *)&buf[PART_ONE_OFFSET];
    if (mbr_entry->boot_flag != 0x80)
        return 0;
    rel_sec = mbr_entry->rel_sec;
    tot_sec = mbr_entry->total_sec;
    ft_printf("Detected bootable partition at sector %d, size %d sectors\n",
              rel_sec, tot_sec);
    dev->seek(dev, rel_sec);
    dev->read(dev, buf, 1); // read first fat partitions
    boot_sector = (BOOT_SECTOR *)buf;
    mount_info = (MOUNT_INFO *) kmalloc(sizeof(MOUNT_INFO));
    if (!fat_mount_fs(boot_sector, mount_info)) {
        return 0;
    }
    mount_info->first_data_sec += rel_sec;
    mount_info->first_fat_sec = rel_sec + 1;
    print_fat_info(mount_info);
    FSYS *new_fs = (FSYS *) kmalloc(sizeof(FSYS));
    dev->fs = new_fs;
    ft_memset(new_fs, 0, sizeof(FSYS));
    new_fs->type_specific_info = mount_info;
    new_fs->curr_cls = FAT_FIRST_DATA_CLS;
    new_fs->dev_handle = dev;
    vfs_register_fsys(new_fs, last_dev);
    last_dev++;
    free(buf, kheap);
    return 1;
}
