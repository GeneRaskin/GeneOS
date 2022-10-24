#include "fat32.h"
#include "../../cstd/include/libft.h"
#include "../../vfs/vfs.h"
#include "../../memory/paging/paging.h"
#include "../../memory/kheap/kheap.h"

uint8_t    fat_determine_fs_type(BOOT_SECTOR *bootsec, MOUNT_INFO *mount_info) {
    BIOS_PARAMETER_BLOCK bpb = bootsec->bpb;
    uint16_t root_dir_sec = ((bpb.BPB_RootEntCnt * 32) +
            (bpb.BPB_BytsPerSec - 1)) / bpb.BPB_BytsPerSec;
    uint32_t fat_size, total_sec;
    if (bpb.BPB_FATSz16 != 0)
        fat_size = bpb.BPB_FATSz16;
    else
        fat_size = bootsec->extended_bpb.bpb_next_fat32.BPB_FATSz32;
    if (bpb.BPB_TotSec16 != 0)
        total_sec = bpb.BPB_TotSec16;
    else
        total_sec = bpb.BPB_TotSec32;
    uint32_t data_sec = total_sec - (bpb.BPB_RsvdSecCnt +
            (bpb.BPB_NumFATs * fat_size) + root_dir_sec);
    uint32_t cls_cnt = data_sec / bpb.BPB_SecPerClus;
    if (cls_cnt < 4085)
        mount_info->fs_type = TYPE_FAT12;
    else if (cls_cnt < 65525)
        mount_info->fs_type = TYPE_FAT16;
    else
        mount_info->fs_type = TYPE_FAT32;
    mount_info->total_sec = total_sec;
    mount_info->first_data_sec = bpb.BPB_RsvdSecCnt +
            (bpb.BPB_NumFATs * fat_size) + root_dir_sec;
    mount_info->root_dir_size = root_dir_sec;
    return 1;
}

uint8_t fat_mount_fs(BOOT_SECTOR *bootsec, MOUNT_INFO *mount_info) {
    ft_printf("Mounting FAT filesystem\n");
    BIOS_PARAMETER_BLOCK bpb = bootsec->bpb;
    // BS_jmpBoot needs to have specific instructions
    if (!(bootsec->BS_jmpBoot[0] == 0xEB && bootsec->BS_jmpBoot[2] == 0x90)
        && !(bootsec->BS_jmpBoot[0] == 0xE9)) {
        ft_printf("Bad FileSystem: jmp instruction wrong\n");
        return 0;
    }
    // only 512 bytes per sector are supported
    if (bpb.BPB_BytsPerSec != 512) {
        ft_printf("Bad FileSystem: only 512 bytes per sector supported\n");
        return 0;
    }
    if (!fat_determine_fs_type(bootsec, mount_info)) {
        ft_printf("Bad FileSystem: unsupported FAT type\n");
        return 0;
    }
    mount_info->sec_per_Cls = bpb.BPB_SecPerClus;
    mount_info->reserved_sec = bpb.BPB_RsvdSecCnt;
    return 1;
}

FILE    *fat_directory(const char *dirname, DEVICE *device) {
    FILE            *file = (FILE *)kmalloc(sizeof(file));
    MOUNT_INFO      *mount_info = (MOUNT_INFO *)device->fs->type_specific_info;
    uint8_t         buffer[512];
    DIRECTORY_ENTRY *directory;
    for (uint32_t sector = 0; sector < mount_info->root_dir_size; sector++) {
        /*read in sector*/
        device->read(device, buffer, 1);
        /*get directory info*/
        directory = (DIRECTORY_ENTRY *)buffer;
        /* 16 entries per sector */
        for (uint32_t i = 0; i < 16; ++i) {
            char name[11];
            ft_memcpy(name, directory->DIR_Name, 11);
            if (ft_strncmp(dirname, name, 11) == 0) {
                /* dir is found */
                ft_strlcpy((char *)file->name, dirname, 12);
                file->id = 0;
                file->curr_cls = directory->DIR_FstClusLO;
                file->curr_cls |= (directory->DIR_FstClusHI << 16);
                file->eof = 0;
                file->file_size = directory->DIR_FileSize;
                /* set file type */
                if (directory->DIR_Attr == 0x10)
                    file->flags = DIR_TYPE;
                else
                    file->flags = FILE_TYPE;
                return file;
            }
            /* otherwise go to the next directory */
            directory++;
        }
    }
    /* failed to find a directory */
    file->flags = INVALID_TYPE;
    return file;
}

void    fat_print_dir_entry(DIRECTORY_ENTRY *directory) {
    uint32_t flags;
    uint8_t  name[12];
    ft_strlcpy((char *)name, (char *)directory->DIR_Name, 12);
    if (directory->DIR_Attr == 0x10)
        flags = DIR_TYPE;
    else
        flags = FILE_TYPE;
    if (flags == FILE_TYPE)
        ft_printf("%-11s %-6d BYTES %.02u-%.02u-%u ", name, directory->DIR_FileSize,
                  directory->DIR_WrtDate & 0x1F, ((directory->DIR_WrtDate & 0x1E0) >> 5),
                  1980 + ((directory->DIR_WrtDate & 0xFE00) >> 9));
    else
        ft_printf("%-11s %-6s       %.02u-%.02u-%u ", name, "DIR",
                  directory->DIR_WrtDate & 0x1F, ((directory->DIR_WrtDate & 0x1E0) >> 5),
                  1980 + ((directory->DIR_WrtDate & 0xFE00) >> 9));
    ft_printf("%.02u:%.02u\n", (directory->DIR_WrtTime & 0xF800) >> 11,
              (directory->DIR_WrtTime & 0x7E0) >> 5);
}

void    fat_ls_root_dir(DEVICE *device) {
    MOUNT_INFO      *mount_info = (MOUNT_INFO *)device->fs->type_specific_info;
    uint8_t         buffer[512];
    DIRECTORY_ENTRY *directory;
    uint8_t         vol_label[12];
    /* read fat table */
    ft_printf("start sector: %d\n", mount_info->first_data_sec - mount_info->root_dir_size);
    device->seek(device, mount_info->first_data_sec - mount_info->root_dir_size);
    for (uint32_t sector = 0; sector < mount_info->root_dir_size; sector++) {
        /*read in sector*/
        device->read(device, buffer, 1);
        /*get directory info*/
        directory = (DIRECTORY_ENTRY *)buffer;
        /* 16 entries per sector */
        for (uint32_t i = 0; i < 16; ++i) {
            if (directory->DIR_Name[0] == 0x0) {
                ft_printf("Volume label is: %s\n", vol_label);
                return;
            }
            if (directory->DIR_Name[0] == 0xE5 ||
            directory->DIR_Attr & FILE_ATTR_VOLLABEL) {
                ft_strlcpy((char *)vol_label, (char *)directory->DIR_Name, 12);
                directory++;
                continue;
            }
            FILE *file = (FILE *) kmalloc(sizeof(FILE));
            ft_strlcpy((char *)file->name, (char *)directory->DIR_Name, 12);
            file->id = 0;
            file->curr_cls = directory->DIR_FstClusLO;
            file->curr_cls |= (directory->DIR_FstClusHI << 16);
            file->eof = 0;
            file->file_size = directory->DIR_FileSize;
            /* set file type */
            //ft_printf("%d\n", directory->DIR_CrtDate);
            fat_print_dir_entry(directory);
            free(file, kheap);
            directory++;
        }
    }
}

void    fat_read(FILE *file, uint8_t *buf, uint32_t len, DEVICE *device) {
    (void)len;
    if (file) {
        /* get starting physical sector */
        uint32_t phys_sec = 32 + (file->curr_cls - 1);
        device->seek(device, phys_sec);
        device->read(device, buf, 1);
        uint32_t fat_offset = file->curr_cls + (file->curr_cls / 2);
        uint32_t fat_sector = 1 + (fat_offset / 512);
        //uint32_t entry_offset = fat_offset % 512;
        device->seek(device, fat_sector);
        device->read(device, buf, 1);
        device->read(device, buf, 1);
    }
}

/*FILE    *fat_open_file(const char *filename) {
    FILE curr_dir;
    char *p = 0;
    // look for '\' in the path provided
    p = ft_strchr(path, '\\');
    if (!p) {

    }
    p++; // increment the pointer
}*/
