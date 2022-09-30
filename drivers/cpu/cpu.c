#include "cpu.h"
#include "../../interrupts/idt.h"
#include "../../gdt.h"

void    get_vendor(char *vendor);
char    vendor[32] = {0};

int32_t i86_cpu_init() {
    init_idt();
    init_gdt();
    return 0;
}

void i86_cpu_shutdown() {

}

void    i86_cpu_get_vendor() {
    get_vendor(vendor);
}
