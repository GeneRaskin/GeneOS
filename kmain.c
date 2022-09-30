#include "drivers/fb_write/fb_write.h"
#include "drivers/io.h"
#include "drivers/fb_routines/fb_routines.h"
#include "drivers/fb_routines/colors.h"
#include "drivers/serial/serial.h"
#include "gdt.h"
#include "interrupts/idt.h"
#include "multiboot.h"
#include "cstd/include/libft.h"
#include "memory/physmemmngr/physical_memory_manager.h"
#include "memory/paging/paging.h"
#include "debug/debug.h"
#include "memory/kheap/kheap.h"
#include "drivers/pci/pci.h"

uint16_t	fb_pos = 0;
char		*fb = (char *) FB_ADDRESS;

void	display_memory_info(multiboot_info_t *mbd)
{
	uint32_t i, counter = 0;
    ft_printf("Physical Memory Map:\n");
	for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t), ++counter) {
		multiboot_memory_map_t *mmmt = (multiboot_memory_map_t *) (mbd->mmap_addr + i);
        char *type_string;
        switch (mmmt->type) {
            case 1:
                type_string = "Available";
                break;
            case 2:
                type_string = "Reserved";
                break;
            case 3:
                type_string = "ACPI Reclaimable";
                break;
            case 4:
                type_string = "NVS";
                break;
            case 5:
                type_string = "Bad RAM";
        }
        uint64_t addr = (uint64_t)mmmt->addr_low + ((uint64_t)mmmt->addr_high << 0x20);
        uint64_t len = (uint64_t)mmmt->len_low + ((uint64_t)mmmt->len_high << 0x20);
        uint32_t short_addr = (uint32_t) addr;
        uint32_t short_len = (uint32_t) len;
        if (addr + len > UINT32_MAX) {
            if (addr < UINT32_MAX)
                short_len = UINT32_MAX - short_addr;
            else {
                break;
            }
        }
        ft_printf("region: %d start: %#x length (bytes): %#x type: %d (%s)\n", counter,
            short_addr, short_len, mmmt->type, type_string);
    }
    print_memory_stats();
}

void check_kernel_stack_protector() {
    check_kernel_stack_protector();
}

void	kmain(multiboot_info_t *mbd, uint32_t magic)
{
    fb_set_bg_color(FB_WHITE);
    fb_set_fg_color(FB_BLACK);
	clear_fb();
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        get_regs();
        kpanic("multiboot error");
    }
    if (!(mbd->flags >> 6 & 0x1)) {
        get_regs();
        kpanic("multiboot error");
    }
    init_idt();
    init_gdt();
    load_physmmrmngr(mbd);
	display_memory_info(mbd);
    real_bitmap_stats();
    vrtmemmngr_init();
    kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE,
                        KHEAP_MAX_ADDR, 1, 0);
    pci_enumerate();
    /*uint32_t a = kmalloc(8);
    uint32_t b = kmalloc(8);
    uint32_t c = kmalloc(8);
    ft_printf("a: %p ", a);
    ft_printf("b: %p\n", b);
    ft_printf("c: %p ", c);
    uint32_t d = kmalloc(12);
    ft_printf("d: %p\n", d);
    free((void *)c, kheap);
    free((void *)b, kheap);
    c = kmalloc_a(10);
    ft_printf("c: %p\n", c);
    print_heap(kheap);
    print_heap_index(kheap);*/
    k_lspci();
    //check_kernel_stack_protector();
    /*uint32_t *p = (uint32_t *)0x10000000;
    ft_printf("%d\n", *p);*/
	/*serial_configure_baud_rate(SERIAL_COM1_BASE, 1);
	serial_configure_line(SERIAL_COM1_BASE);
	serial_configure_buffer(SERIAL_COM1_BASE);
	serial_configure_modem(SERIAL_COM1_BASE);*/
	while (1) {
	}
}
