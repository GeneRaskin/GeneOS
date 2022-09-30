#include "page_directory_entry.h"

void    pd_entry_set_attribute(pd_entry *entry, uint32_t attribute) {
    *entry |= attribute;
}

void    pd_entry_unset_attribute(pd_entry *entry, uint32_t attribute) {
    *entry &= ~attribute;
}

void    pd_entry_set_frame(pd_entry *entry, uint32_t addr) {
    *entry |= addr & PDE_FRAME;
}

uint32_t    pd_entry_is_present(pd_entry entry) {
    return entry & PDE_PRESENT;
}

uint32_t    pd_entry_is_user(pd_entry entry) {
    return entry & PDE_USER;
}

uint32_t    pd_entry_is_4mb(pd_entry entry) {
    return entry & PDE_4MB;
}

uint32_t    pd_entry_is_writable(pd_entry entry) {
    return entry & PDE_WRITABLE;
}

uint32_t    pd_entry_frame_addr(pd_entry entry) {
    return entry & PDE_FRAME;
}
