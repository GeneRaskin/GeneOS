#include "../drivers/fb_routines/fb_routines.h"
#include "../drivers/fb_routines/colors.h"
#include "../cstd/include/libft.h"
#include "debug.h"
/*
void printf_reg(char *reg_name, uint32_t reg_value) {
    if (reg_value == 0) {
        ft_printf("%s: 0x00000000", reg_name);
    } else {
        ft_printf("%s: %#.8x", reg_name, reg_value);
    }
}*/

/*void printf_regs_values(struct cpu_state cpu, uint16_t cs, uint32_t eflags) {
    char *reg_names[14] = {"eax   ", "ecx   ", "edx   ",
                           "ebx   ", "esp   ", "ebp   ",
                           "esi   ", "edi   ", "ds    ",
                           "es    ", "fs    ", "gs    ",
                           "cs    ", "eflags"};
    uint32_t reg_values[14] = {cpu.eax, cpu.ecx, cpu.edx,
                               cpu.ebx, cpu.esp, cpu.ebp,
                               cpu.esi, cpu.edi, cpu.ds,
                               cpu.es, cpu.fs, cpu.gs,
                               cs, eflags};
    for (uint32_t i = 0; i < 14; ++i) {
        printf_reg(reg_names[i], reg_values[i]);
        if (i % 2 == 0)
            ft_printf("    ");
        else
            ft_printf("\n");
    }
}*/

void kpanic(char *error_msg) {
    asm("cli");
    fb_set_bg_color(FB_WHITE);
    fb_set_fg_color(FB_BLUE);
    clear_fb();
    ft_printf("Kernel panic occurred. System halted.\n");
    ft_printf("Technical information:\n\n");
    printf_regs_values();
    ft_printf("\n*** STOP: %s\n", error_msg);
    get_stack_trace(10);
    asm("hlt");
    while (1) {

    }
}