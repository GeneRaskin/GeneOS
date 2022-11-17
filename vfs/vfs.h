#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define MAX_DEVICES  26
#define FILE_TYPE    0
#define DIR_TYPE     1
#define INVALID_TYPE 2

typedef struct _FILE {
    uint8_t name[32];
    uint32_t flags;
    uint32_t file_size;
    uint32_t id;
    uint32_t eof;
    uint32_t pos;
    uint32_t curr_cls;
    uint32_t dev_id;
} FILE;

extern char last_dev;

typedef void        (*mount_type)(void);
typedef FILE        *(*open_type)(const char *filename);
typedef int32_t     (*read_type)(FILE *file, uint8_t *buffer,
        uint32_t len);
typedef int32_t     (*close_type)(FILE *file);

typedef struct _FSYS {
    uint8_t name[8];
    mount_type mount;
    read_type read;
    close_type close;
    open_type open;
    uint8_t type;
    DEVICE  *dev_handle;
    void    *type_specific_info;
    uint32_t curr_cls; // curr directory file location
} FSYS;

extern FSYS *file_systems[MAX_DEVICES];

FILE    *vfs_open_file(const char *filename);
int32_t vfs_read_file(FILE *file, uint8_t *buffer, uint32_t len);
int32_t vfs_close_file(FILE *file);
int32_t vfs_unregister_fsys(uint32_t device_id);
int32_t vfs_register_fsys(FSYS *fsys, uint32_t device_id);
void    init_mount_list();

#endif
