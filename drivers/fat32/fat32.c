#include "fat32.h"
#include "../../cstd/include/libft.h"
#include "../../vfs/vfs.h"
#include "../../memory/paging/paging.h"

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
    if (!(bootsec->BS_JumpBoot[0] == 0xEB && bootsec->BS_JumpBoot[2] == 0x90)
        && !(bootsec->BS_JumpBoot[0] == 0xE9)) {
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

FILE    *fat_directory(const char *dirname) {
    FILE            *file = (FILE *)kmalloc(sizeof(file));
    uint8_t         *buffer;
    DIRECTORY_ENTRY directory;
}

FILE    *fat_open_file(const char *filename) {
    FILE curr_dir;
    char *p = 0;
    // look for '\' in the path provided
    p = ft_strchr(path, '\\');
    if (!p) {

    }
    p++; // increment the pointer
}
