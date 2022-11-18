SRCS = $(wildcard cstd/libft/*.c cstd/printf/*.c *.c drivers/fb_routines/*.c \
	drivers/serial/*.c drivers/fb_write/*.c interrupts/*.c \
	drivers/keyboard/*.c memory/physmemmngr/*.c memory/paging/*.c \
	debug/*.c memory/kheap/*.c ordered_array/*.c vfs/*.c \
	drivers/cpu/*.c drivers/dma/*.c drivers/pci/*.c drivers/ata/*.c \
	drivers/*.c drivers/fat32/*.c kshell/*.c)
ASM_SRCS = $(wildcard drivers/*.s *.s interrupts/*.s drivers/serial/*.s \
	memory/physmemmngr/*.s memory/paging/*.s debug/*.s drivers/cpu/*.s )
OBJS = $(filter %.o,$(patsubst %.c,%.o,$(SRCS)) $(patsubst %.s,%.o,$(ASM_SRCS)))
CC = i686-elf-gcc
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy
CFLAGS = -m32 -nostdlib -fno-builtin -fno-stack-protector \
	 -nostartfiles -nodefaultlibs -c -Wall -Werror -Wextra -g \
	 -I./cstd/include -Wno-infinite-recursion
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf32

all: kernel.elf

kernel.elf: $(OBJS)
	mkdir -p build/iso/boot
	mkdir -p build/iso/boot/grub
	$(LD) $(LDFLAGS) $(OBJS) -o kernel.elf
	mv kernel.elf build
	$(OBJCOPY) --only-keep-debug build/kernel.elf kernel
	./generate_symbols.sh kernel
	mv kernel.sym build
	rm -f kernel

os.iso:
	cp build/kernel.elf build/iso/boot/
	cp resources/stage2_eltorito build/iso/boot/grub/
	cp resources/menu.lst build/iso/boot/grub/
	genisoimage -R \
		-b boot/grub/stage2_eltorito \
		-no-emul-boot \
		-boot-load-size 4 \
		-A os \
		-input-charset utf8 \
		-quiet \
		-boot-info-table \
		-o build/os.iso \
		build/iso

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(OBJS) build

.PHONY: all clean os.iso
