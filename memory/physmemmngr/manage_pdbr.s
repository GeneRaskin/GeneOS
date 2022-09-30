global load_pdbr, get_pdbr, enable_paging
global is_paging_enabled

section .text
load_pdbr:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]
    mov cr3, eax
    mov esp, ebp
    pop ebp
    ret
get_pdbr:
    mov eax, cr3
    ret
enable_paging:
    mov eax, cr0
    cmp dword [esp + 4], 1
    je .enable
    jmp .disable
.enable:
    or eax, 0x80000000 ; set bit 31
    mov cr0, eax
    jmp .end
.disable:
    and eax, 0x7FFFFFFF ; clear the 31st bit
    mov cr0, eax
.end:
    ret
is_paging_enabled:
    mov eax, cr0
    and eax, 0x80000000
    test eax, eax
    jz .end
    .then_statement:
    rol eax, 1
    jmp .end
    .end:
    ret