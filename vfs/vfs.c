#include "vfs.h"
#include "../memory/paging/paging.h"

// list of mounted file systems
FSYS *file_systems[MAX_DEVICES]; // assume all pointers are initially NULL

FILE *vfs_open_file(const char *pathname) {
    if (pathname) {
        uint8_t device = 'A'; // first drive letter
        char *path = (char *) pathname;
        if (pathname[1] == ':') {
            device = path[0];
            path += 2; // move past the drive letter and colon
        }
        if (device < 'A' || device > 'Z') {
            FILE *file = (FILE *) kmalloc(sizeof(file));
            file->flags = INVALID_TYPE;
            return file; // first character invalid
        }
        if (file_systems[device - 'A']) {
            FILE *file = file_systems[device - 'A']->open(path);
            file->dev_id = device;
            return file;
        }
    }
    FILE *file = (FILE *) kmalloc(sizeof(file));
    file->flags = INVALID_TYPE;
    return file; // pathname is NULL or filesystem not mounted
}

int32_t vfs_read_file(FILE *file, uint8_t *buffer, uint32_t len) {
    if (file) {
        if (file_systems[file->dev_id - 'A'])
            return file_systems[file->dev_id - 'A']->read(file, buffer, len);
    }
    return -1;
}

int32_t vfs_close_file(FILE *file) {
    if (file) {
        if (file_systems[file->dev_id - 'A'])
            return file_systems[file->dev_id - 'A']->close(file);
    }
    return -1;
}

int32_t   vfs_register_fsys(FSYS *fsys, uint32_t device_id) {
    if (device_id > 'Z' || device_id < 'A')
        return -1; // if device id is out of bounds
    if (file_systems[device_id - 'A']) {
        if (fsys) {
            file_systems[device_id - 'A'] = fsys;
        }
    }
    return 1;
}

int32_t vfs_unregister_fsys(uint32_t device_id) {
    if (device_id > 'Z' || device_id < 'A')
        return -1; // if device id is out of bounds
    if (file_systems[device_id - 'A']) {
        file_systems[device_id - 'A'] = 0;
    }
    return 1;
}

void    init_mount_list() {
    ft_memset(file_systems, 0, MAX_DEVICES * sizeof(FSYS *));
}
