#include "fat32.h"
#include "../../cstd/include/libft.h"
#include "../../vfs/vfs.h"
#include "../../memory/paging/paging.h"
#include "../../memory/kheap/kheap.h"

void    fat_print_dir_entry(DIRECTORY_ENTRY *directory);

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

uint32_t calc_start_sec(DEVICE *device) {
    MOUNT_INFO  *mount_info = (MOUNT_INFO *)device->fs->type_specific_info;
    uint32_t    curr_cls = device->fs->curr_cls;
    uint32_t    first_data_sec = mount_info->first_data_sec - mount_info->root_dir_size;
    uint32_t    sector_offset = (curr_cls - FAT_FIRST_DATA_CLS) * mount_info->sec_per_Cls;
    return first_data_sec + sector_offset;
}

uint32_t    get_cls_value(DEVICE *dev, uint32_t cls_number) {
    MOUNT_INFO *mount_info = (MOUNT_INFO *)dev->fs->type_specific_info;
    uint32_t num_bytes_into_table = cls_number * 2;
    uint32_t sec_to_read = num_bytes_into_table / 512;
    uint8_t fat_buffer[512];
    dev->seek(dev, mount_info->first_fat_sec + sec_to_read);
    dev->read(dev, fat_buffer, 1);
    return (((uint16_t *)fat_buffer)[(num_bytes_into_table % 512) / 2]);
}

FILE    *fat_parse_directory(const char *dirname, DEVICE *device) {
    FILE            *file = (FILE *)kmalloc(sizeof(file));
    MOUNT_INFO      *mount_info = (MOUNT_INFO *)device->fs->type_specific_info;
    uint8_t         buffer[512];
    uint32_t        curr_cls = device->fs->curr_cls;
    uint32_t        old_cls = curr_cls;
    uint32_t        dirname_len = ft_strlen(dirname);
    DIRECTORY_ENTRY *directory;

    ft_printf("Parsing cluster: %x\n", device->fs->curr_cls);
    do {
        device->fs->curr_cls = curr_cls;
        for (uint32_t sector = 0; sector < mount_info->sec_per_Cls; sector++) {
            /*read in sector*/
            device->seek(device, calc_start_sec(device) + sector);
            device->read(device, buffer, 1);
            /*get directory info*/
            directory = (DIRECTORY_ENTRY *) buffer;
            /* 16 entries per sector */
            for (uint32_t i = 0; i < 16; ++i) {
                char name[12];
                ft_strlcpy(name, (char *)directory->DIR_Name, 12);
                if (ft_strncmp(dirname, name, dirname_len) == 0
                    && (!name[dirname_len + 1] || name[dirname_len + 1] == ' ')) {
                    /* dir is found */
                    ft_strlcpy((char *) file->name, dirname, 12);
                    file->id = 0;
                    file->curr_cls = directory->DIR_FstClusLO;
                    file->curr_cls |= (directory->DIR_FstClusHI << 16);
                    file->curr_cls += FAT_FIRST_DATA_CLS - 1;
                    //ft_printf("%d\n", file->curr_cls);
                    file->eof = 0;
                    file->file_size = directory->DIR_FileSize;
                    /* set file type */
                    if (directory->DIR_Attr == 0x10)
                        file->flags = DIR_TYPE;
                    else
                        file->flags = FILE_TYPE;
                    device->fs->curr_cls = old_cls;
                    return file;
                }
                /* otherwise go to the next directory */
                //fat_print_dir_entry(directory);
                directory++;
            }
        }
        if (curr_cls < 0xFFF7 && curr_cls > 0x0001)
            curr_cls = get_cls_value(device, curr_cls);
        ft_printf("Parsing cluster: %x\n", curr_cls);
    } while (curr_cls < 0xFFF7 && curr_cls > 0x0001);
    /* failed to find a directory */
    file->flags = INVALID_TYPE;
    device->fs->curr_cls = old_cls;
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
    ft_printf("curr cls: %d start sector: %d\n", device->fs->curr_cls,
              calc_start_sec(device));
    device->seek(device, calc_start_sec(device));
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
            file->curr_cls += FAT_FIRST_DATA_CLS - 1;
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

uint8_t fat_set_dir(DEVICE *device, const char *filename, uint8_t absolute) {
    //MOUNT_INFO *mount_info = (MOUNT_INFO *)device->fs->type_specific_info;
    FILE *curr_dir_entry;
    uint32_t old_cls = device->fs->curr_cls;
    if (absolute)
        device->fs->curr_cls = FAT_FIRST_DATA_CLS;
    int num_dirs;
    char **dir_names = ft_split(filename, '\\', &num_dirs);
    if (!num_dirs)
        return (0);
    for (uint8_t i = 0; i < num_dirs;) {
        curr_dir_entry = fat_parse_directory(dir_names[i], device);
        if (curr_dir_entry->flags == DIR_TYPE) {
            ft_printf("found dir %s at cluster %d\n",
                      dir_names[i], curr_dir_entry->curr_cls);
            device->fs->curr_cls = curr_dir_entry->curr_cls;
            i++;
            free(curr_dir_entry, kheap);
        } else {
            free(curr_dir_entry, kheap);
            device->fs->curr_cls = old_cls;
            return (0);
        }
    }
    return (1);
}

/*
FILE    *fat_open_file(DEVICE *device, const char *filename, uint8_t absolute) {
    MOUNT_INFO *mount_info = (MOUNT_INFO *)device->fs->type_specific_info;
    uint32_t old_cls = device->fs->curr_cls;
    if (absolute)
        device->fs->curr_cls = mount_info->first_data_sec - mount_info->root_dir_size;
}*/
