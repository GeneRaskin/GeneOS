SRCS = $(wildcard cstd/libft/*.c cstd/printf/*.c *.c drivers/fb_routines/*.c \
	drivers/serial/*.c drivers/fb_write/*.c interrupts/*.c \
	drivers/keyboard/*.c memory/physmemmngr/*.c memory/paging/*.c \
	debug/*.c memory/kheap/*.c ordered_array/*.c vfs/*.c \
	drivers/cpu/*.c drivers/dma/*.c drivers/pci/*.c drivers/ata/*.c \
	drivers/*.c drivers/fat32/*.c kshell/*.c)
ASM_SRCS = $(wildcard drivers/*.s *.s interrupts/*.s drivers/serial/*.s \
	memory/physmemmngr/*.s memory/paging/*.s debug/*.s drivers/cpu/*.s )
OBJS = $(filter %.o,$(patsubst %.c,%.o,$(SRCS)) $(patsubst %.s,%.o,$(ASM_SRCS)))
CC = gcc
CFLAGS = -m32 -nostdlib -fno-builtin -fno-stack-protector \
	 -nostartfiles -nodefaultlibs -c -Wall -Werror -Wextra -g
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf32

all: kernel.elf generate_symbols

generate_symbols: kernel.elf
	objcopy --only-keep-debug kernel.elf kernel.debug
	./generate_symbols.sh kernel.debug
	mv kernel.debug.sym ../bochs
	rm -f kernel.debug

kernel.elf: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o kernel.elf

os.iso: all
	cp kernel.elf ../iso/boot/kernel.elf
	genisoimage -R \
		-b boot/grub/stage2_eltorito \
		-no-emul-boot \
		-boot-load-size 4 \
		-A os \
		-input-charset utf8 \
		-quiet \
		-boot-info-table \
		-o os.iso \
		../iso

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(OBJS) kernel.elf os.iso kernel.debug.sym
