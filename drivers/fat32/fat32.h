#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#define TYPE_FAT12 0
#define TYPE_FAT16 1
#define TYPE_FAT32 2

#define BAD_CLUSTER_MARK32 0x0FFFFFF7
#define BAD_CLUSTER_MARK16 0xFFF7
#define EOF_MARK32         0x0FFFFFF8
#define EOF_MARK16         0xFFF8

#define ATTR_DIRECTORY     0x10
#define FLAG_DIRTY         0x01
#define FLAG_OPEN          0x02
#define FLAG_SIZE_CHANGED  0x04
#define FLAG_ROOT          0x08

#define MAX_PATH           256

#define FILE_ATTR_READONLY 0x1
#define FILE_ATTR_HIDDEN   0x2
#define FILE_ATTR_SYSTEM   0x4
#define FILE_ATTR_VOLLABEL 0x8
#define FILE_ATTR_SUBDIR   0x10
#define FILE_ATTR_ARCHIVE  0x20
#define FILE_ATTR_DEVICE   0x60

#include "../device.h"

typedef struct _DIRECTORY_ENTRY {
    uint8_t     DIR_Name[11];
    uint8_t     DIR_Attr;
    uint8_t     DIR_NTRes;
    uint8_t     DIR_CrtTimeTenth;
    uint16_t    DIR_CrtTime;
    uint16_t    DIR_CrtDate;
    uint16_t    DIR_LstAccDate;
    uint16_t    DIR_FstClusHI;
    uint16_t    DIR_WrtTime;
    uint16_t    DIR_WrtDate;
    uint16_t    DIR_FstClusLO;
    uint32_t    DIR_FileSize;
}__attribute__((packed)) DIRECTORY_ENTRY;

typedef struct _FSINFO {
    uint32_t FSI_LeadSig;
    uint8_t FSI_Reserved1[480];
    uint32_t FSI_StrucSig;
    uint32_t FSI_Free_Count;
    uint32_t FSI_Nxt_Free;
    uint8_t FSI_Reserved2[12];
    uint32_t FSI_TrailSig;
}__attribute__((packed)) FSINFO;

typedef struct _BIOS_PARAMETER_BLOCK {
    uint8_t     BS_OEMName[8];
    uint16_t    BPB_BytsPerSec; // count of bytes per sector
    uint8_t     BPB_SecPerClus; // number of sectors per allocation unit
    uint16_t    BPB_RsvdSecCnt; // number of reserved sectors
    uint8_t     BPB_NumFATs;    // set to 2
    uint16_t    BPB_RootEntCnt; // set to 0
    uint16_t    BPB_TotSec16;   // set to 0
    uint8_t     BPB_Media;
    uint16_t    BPB_FATSz16;    // set to 0
    uint16_t    BPB_SecPerTrk;  // sectors per track
    uint16_t    BPB_NumHeads;   // number of heads
    uint32_t    BPB_HiddSec;    // count of hidden sectors preceding the FAT volume
    uint32_t    BPB_TotSec32;   // total count of sectors on the volume
}__attribute__((packed)) BIOS_PARAMETER_BLOCK;

typedef struct _BIOS_PARAMETER_BLOCK_NEXT_FAT32 {
    uint32_t    BPB_FATSz32;        // 32-bit count of sectors occupied by ONE FAT
    uint16_t    BPB_ExtFlags;
    uint16_t    BPB_FSVer;
    uint32_t    BPB_RootClus;       // cluster number of the first cluster of the root dir
    uint16_t    BPB_FSInfo;         // sector number of FSINFO data structure
    uint16_t    BPB_BkBootSec;      // set to 6
    uint8_t     BPB_Reserved[12];   // reserved
    uint8_t     BS_DrvNum;          // Int 0x13 drive number
    uint8_t     BS_Reserved1;       // reserved (used by Windows NT)
    uint8_t     BS_BootSig;         // Extended boot sig (0x29)
    uint32_t    BS_VolID;           // Volume serial number
    uint8_t     BS_VolLab[11];      // Volume label
    uint64_t    BS_FilSysType;      // always set to "FAT32   "
    uint8_t     filler[416];        // struct should be 512 bytes long
}__attribute__((packed)) BIOS_PARAMETER_BLOCK_NEXT_FAT32;

typedef struct _BIOS_PARAMETER_BLOCK_NEXT_FAT1612 {
    uint8_t     BS_DrvNum;
    uint8_t     BS_Reserved1;
    uint8_t     BS_BootSig;
    uint32_t    BS_VolID;
    uint8_t     BS_VolLab[11];
    uint64_t    BS_FilSysType;
    uint8_t     filler[448];
}__attribute__((packed)) BIOS_PARAMETER_BLOCK_NEXT_FAT1612;

typedef struct _BOOT_SECTOR {
    uint8_t                     BS_jmpBoot[3];  // ignore the first 3 bytes
    BIOS_PARAMETER_BLOCK        bpb;            // the BPB struct
    union {                                     // extended BPB info
        BIOS_PARAMETER_BLOCK_NEXT_FAT32     bpb_next_fat32;
        BIOS_PARAMETER_BLOCK_NEXT_FAT1612   bpb_next_fat1612;
    } extended_bpb;
}__attribute__((packed)) BOOT_SECTOR;

typedef struct _MOUNT_INFO {
    uint8_t     fs_type;
    uint8_t     sec_per_Cls;    // sectors per cluster
    uint32_t    first_data_sec; // first data sector
    uint32_t    total_sec;      // total sectors count
    uint16_t    reserved_sec;   // reserved sectors count
    uint32_t    curr_sec;       // current sector
    uint32_t    first_fat_sec;  // FAT sector
    uint8_t     sec_flags;
    uint32_t    root_dir_size;
    uint8_t     buffer[512];
}__attribute__((packed)) MOUNT_INFO;

typedef struct _FAT_LONG_FILENAME {
    uint8_t     LDIR_Ord;
    uint8_t     LDIR_Name1[10];
    uint8_t     LDIR_Attr;
    uint8_t     LDIR_Type;
    uint8_t     LDIR_Chksum;
    uint8_t     LDIR_Name2[12];
    uint16_t    LDIR_FstClusLO;
    uint32_t    LDIR_Name3;
}__attribute__((packed)) FAT_LONG_FILENAME;

typedef union _FAT_FILE_ENTRY {
    DIRECTORY_ENTRY     msdos;
    FAT_LONG_FILENAME   long_filename;
}__attribute__((packed)) FAT_FILE_ENTRY;

/* Mount the FAT filesystem
 * Returns 1 on success, 0 otherwise */
uint8_t fat_mount_fs(BOOT_SECTOR *bootsec, MOUNT_INFO *mount_info);
FILE    *fat_directory(const char *dirname, DEVICE *device);
void    fat_read(FILE *file, uint8_t *buf, uint32_t len,
                 DEVICE *device);
uint8_t parse_mbr(DEVICE *dev);
void    print_fat_info(MOUNT_INFO *mount_info);
void    fat_ls_root_dir(DEVICE *device);

#endif
