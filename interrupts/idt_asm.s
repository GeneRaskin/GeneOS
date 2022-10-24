global load_idt, ignore_int_no_error, ignore_int_error
global keyboard_interrupt_handler, div_by_zero_handler
global double_fault_handler, general_protection_fault_handler
global page_fault_handler, timer_interrupt_handler
global ata_primary_interrupt_handler, ata_secondary_interrupt_handler
extern ft_printf, keyboard_interrupt, divide_by_zero_exception
extern general_protection_fault_exception, double_fault_exception
extern page_fault_exception, timer_interrupt
extern ata_primary_interrupt, ata_secondary_interrupt

%macro SAVE_REGS 0
	pushad
	push ds
	push es
	push fs
	push gs
%endmacro

%macro RESTORE_REGS 0
	pop gs
	pop fs
	pop es
	pop ds
	popad
%endmacro

section .data
	UNKNOWN_INT_MSG db "KERNEL: Unknown interrupt", 0ah, 0

section .text

timer_interrupt_handler:
    cli
    push dword 32
    SAVE_REGS
    call timer_interrupt
    RESTORE_REGS
    add esp, 4
    sti
    iretd

keyboard_interrupt_handler:
    cli
	push dword 33
	SAVE_REGS
	call keyboard_interrupt
	RESTORE_REGS
	add esp, 4
	sti
	iretd

ata_primary_interrupt_handler:
    cli
    push dword 46
    SAVE_REGS
    call ata_primary_interrupt
    RESTORE_REGS
    add esp, 4
    sti
    iretd

ata_secondary_interrupt_handler:
    cli
    push dword 47
    SAVE_REGS
    call ata_secondary_interrupt
    RESTORE_REGS
    add esp, 4
    sti
    iretd

load_idt:
	mov eax, [esp + 4]
	lidt [eax]
	ret

ignore_int_error:
    cli
	SAVE_REGS
	push dword UNKNOWN_INT_MSG
	call ft_printf
	add esp, 8
	RESTORE_REGS
	add esp, 4
	sti
	iretd

ignore_int_no_error:
    cli
	SAVE_REGS
	push dword UNKNOWN_INT_MSG
	call ft_printf
	add esp, 4
	RESTORE_REGS
	sti
	iretd

div_by_zero_handler:
    SAVE_REGS
    call divide_by_zero_exception
    RESTORE_REGS
    iretd

double_fault_handler:
    SAVE_REGS
    call double_fault_exception
    RESTORE_REGS
    add esp, 4
    iretd

general_protection_fault_handler:
    SAVE_REGS
    call general_protection_fault_exception
    RESTORE_REGS
    add esp, 4
    iretd

page_fault_handler:
    SAVE_REGS
    call page_fault_exception
    RESTORE_REGS
    add esp, 4
    iretd