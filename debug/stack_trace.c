#include "debug.h"
#include "../cstd/include/libft.h"

const char *reg_names[14] = {"gs    ", "fs    ", "es    ",
                             "ds    ", "edi   ", "esi   ",
                             "ebp   ", "esp   ", "ebx   ",
                             "edx   ", "ecx   ", "eax   ",
                             "cs    ", "eflags"};

struct stackframe {
    struct stackframe *ebp;
    uint32_t eip;
};

void get_stack_trace(uint32_t max_frames) {
    struct stackframe *stack;
    stack = (struct stackframe *) get_ebp();
    ft_printf("\nStack trace:\n");
    for (uint32_t curr_frame = 0; stack && curr_frame < max_frames;
            ++curr_frame) {
        ft_printf("%#.8x\n", stack->eip);
        stack = stack->ebp;
    }
}