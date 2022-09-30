global flush_tss

section .text
flush_tss:
    cli
    mov ax, [esp + 4]
    ltr ax
    sti
    ret