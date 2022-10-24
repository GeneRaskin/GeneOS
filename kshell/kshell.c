#include "kshell.h"
#include "../cstd/include/libft.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/ata/ata.h"
#include "../memory/paging/paging.h"
#include "../debug/debug.h"
#include "../drivers/fat32/fat32.h"
#include "../vfs/vfs.h"
#include "argparse.h"

const char *tokens[] = {
        "read_ata",
        "ls_ata",
        "print_kheap",
        "mount_info",
        "fat_ls_root",
        "cd",
        "pwd"
        0
};

char *curr_path[100] = {0};
const char *usages[] = {
        "basic [[--] args]",
        NULL
};

void    print_mount_info() {
    for (int32_t i = 'A'; i < last_dev; ++i) {
        ft_printf("File System id: %c ", i);
        print_fat_info(file_systems[i - 'A']->type_specific_info);
    }
}

/* This command takes 2 args in the format
 * drive_num pos */
void    read_ata(int argc, const char **argv) {
    int32_t drive;
    int32_t pos;
    /*for (int32_t i = 0; i < argc; ++i)
        ft_printf("%s\n", argv[i]);*/
    struct argparse_option options[] = {
            OPT_HELP(),
            OPT_GROUP("Basic options"),
            OPT_INTEGER('d', "drive", &drive, "drive select",
                        NULL, 0, 0),
            OPT_INTEGER('p', "lba pos", &pos, "lba select",
                        NULL, 0, 0),
            OPT_END()
    };
    struct argparse *argparse = (struct argparse *)
            kmalloc(sizeof(struct argparse));
    argparse_init(argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);
    argparse_describe(argparse, "Select an lba address and read one sector",
                      "Use ls_ata command to see available drives to read from");
    int32_t status;
    if ((status = argparse_parse(argparse, argc, argv)) == 1) {
        free(argparse, kheap);
        return;
    }
    if (argc != 5) {
        argparse_usage(argparse);
        free(argparse, kheap);
        return;
    }
    if (drive < (int32_t)n_ata_devices &&
        drive >= 0) {
        uint8_t *buf = (uint8_t *)kmalloc(512);
        ft_memset(buf, 0, 512);
        (ata_devices[drive])->seek(ata_devices[drive], pos);
        (ata_devices[drive])->read(ata_devices[drive], buf, 1);
        print_mem(buf, 512);
        free(buf, kheap);
        ft_printf("sector number %d read, from drive %d\n", pos, drive);
    } else {
        ft_printf("Wrong device number\n");
    }
    free(argparse, kheap);
}

void    free_argv(int argc, char **argv) {
    for (int32_t i = 0; i < argc; i++)
        free(argv[i], kheap);
    free(argv, kheap);
}

void    execute_command() {
    int argc;
    char **argv;
    uint32_t curr_token_len;

    argv = ft_split(line_buffer, ' ', &argc);
    if (line_length == 0 || !argc) {
        if (argv)
            free(argv, kheap);
        return;
    }
    for (uint32_t i = 0; tokens[i]; ++i) {
        curr_token_len = ft_strlen(tokens[i]);
        if (line_length < curr_token_len)
            continue;
        if (!ft_strncmp(tokens[i], line_buffer, curr_token_len)
        && ft_strlen(argv[0]) == curr_token_len) {
            switch (i) {
                case 0:
                    read_ata(argc, (const char **) argv);
                    free_argv(argc, argv);
                    return;
                case 1:
                    ls_ata();
                    return;
                case 2:
                    print_heap(kheap);
                    return;
                case 3:
                    print_mount_info();
                    return;
                case 4:
                    fat_ls_root_dir(ata_devices[0]);
                    return;
            }
        }
    }
    free_argv(argc, argv);
    ft_printf("Unknown command\n");
}

void    kshell() {
    while (1) {
        ft_printf("kernel>");
        while (!enter_pressed);
        if (enter_pressed) {
            asm("cli");
            execute_command();
            line_length = 0;
            enter_pressed = 0;
            asm("sti");
        }
    }
}