global flush_tlb_entry

section .text
flush_tlb_entry:
    cli
    invlpg [esp + 4]
    sti
    ret