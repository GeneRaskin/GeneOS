global get_regs
global printf_reg, printf_regs_values
global get_ebp
extern ft_printf, reg_names

section .bss
reg_vals:
    resd 14

section .data
ZERO_ADDR_FMT:
    db "%s: 0x00000000", 0
NONZERO_ADDR_FMT:
    db "%s: %#.8x", 0
TAB_FMT:
    db "     ", 0
NEWLINE:
    db 0ah, 0

section .text
printf_reg:
    mov eax, dword [esp + 8] ; eax = reg_value
    mov ecx, dword [esp + 4] ; ecx = reg_name
    test eax, eax ; test if eax == 0
    jnz .else
    .then_block:
        push ecx ; push reg_name
        push dword ZERO_ADDR_FMT ; push format string
        call ft_printf
        add esp, 8 ; restore stack pointer and deallocate args
        jmp .endif
    .else:
        push eax ; push reg_value
        push ecx ; push reg_name
        push dword NONZERO_ADDR_FMT
        call ft_printf
        add esp, 12 ; restore stack pointer
    .endif:
    ret

printf_regs_values:
    xor eax, eax ; eax = 0
    .loop:
        push eax ; save eax before calling printf_reg
        .get_reg_value: ; get appropriate reg_value
            mov ecx, [reg_vals + 4 * eax] ; ecx = reg_values[i]
        .get_reg_name: ; get appropriate reg_name
            mov edx, [reg_names + 4 * eax] ; edx = reg_names[i]
        push ecx
        push edx
        call printf_reg
        add esp, 8 ; restore stack
        pop eax ; restore eax
        test al, 1 ; test if the lowest bit is set
        push eax ; save eax
        jnz .not_even
        .even:
            push dword TAB_FMT
            call ft_printf
            add esp, 4
            jmp .endif
        .not_even:
            push dword NEWLINE
            call ft_printf
            add esp, 4
        .endif:
        pop eax ; restore eax counter
        inc eax
        cmp eax, 14
        jne .loop
        jmp .end
    .end:
    ret

get_regs:
    push eax
    and eax, 0x0000FFFF
    mov ax, gs
    mov dword [reg_vals], eax
    mov ax, fs
    mov dword [reg_vals + 4], eax
    mov ax, es
    mov dword [reg_vals + 8], eax
    mov ax, ds
    mov dword [reg_vals + 12], eax
    pop eax
    mov dword [reg_vals + 16], edi
    mov dword [reg_vals + 20], esi
    mov dword [reg_vals + 24], ebp
    mov dword [reg_vals + 28], esp
    mov dword [reg_vals + 32], ebx
    mov dword [reg_vals + 36], edx
    mov dword [reg_vals + 40], ecx
    mov dword [reg_vals + 44], eax
    push eax
    and eax, 0x0000FFFF
    mov ax, cs
    mov dword [reg_vals + 48], eax
    pop eax
    pushfd
    pop eax
    mov dword [reg_vals + 52], eax
    ret

get_ebp:
    mov eax, ebp
    ret
