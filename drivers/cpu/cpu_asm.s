global get_vendor, i86_cpu_flush_caches_write
global i86_cpu_flush_caches, i86_cpu_flush_caches_write
global i86_cpu_flush_tlb_entry

section .text
get_vendor:
    xor eax, eax
    cpuid
    mov eax, dword [esp + 4]
    mov dword [eax], ebx
    mov dword [eax + 4], edx
    mov dword [eax + 8], ecx
    ret

i86_cpu_flush_caches:
    cli
    invd
    sti
    ret

i86_cpu_flush_caches_write:
    cli
    wbinvd
    sti
    ret

i86_cpu_flush_tlb_entry:
    cli
    invlpg [esp + 4]
    sti
    ret
