global enter_usermode

section .text
enter_usermode:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax