global loader                   ; the entry symbol for ELF

MAGIC_NUMBER      equ 0x1BADB002     ; define the magic number constant
FLAGS             equ 0x0            ; multiboot flags
CHECKSUM          equ -MAGIC_NUMBER  ; calculate the checksum
                                ; (magic number + checksum + flags should equal 0)
KERNEL_STACK_SIZE equ 4096
KERNEL_STACK_OVERFLOW_PROTECTOR equ 4096
INTERRUPT_STACK_SIZE equ 4096
extern kmain, fb_write

section .__mbHeader
	align 4
	dd MAGIC_NUMBER         ; write the magic number to the machine code,
        dd FLAGS                ; the flags,
        dd CHECKSUM             ; and the checksum

section .text
	align 4
loader:                         ; the loader label (defined as entry point in linker script)
	mov esp, kernel_stack + KERNEL_STACK_SIZE + KERNEL_STACK_OVERFLOW_PROTECTOR
	xor ebp, ebp
	push eax
	push ebx
	call kmain
.loop:
	jmp .loop

section .__kernelStack
	align 4
	kernel_stack:
		times KERNEL_STACK_SIZE + KERNEL_STACK_OVERFLOW_PROTECTOR db 0

section .__interruptStack
    align 4
    interrupt_stack:
        times INTERRUPT_STACK_SIZE db 0