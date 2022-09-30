#include "page_table_entry.h"

void    pt_entry_set_attribute(pt_entry *entry, uint32_t attribute) {
    *entry |= attribute;
}

void    pt_entry_unset_attribute(pt_entry *entry, uint32_t attribute) {
    *entry &= ~attribute;
}

void    pt_entry_set_frame(pt_entry *entry, uint32_t addr) {
    *entry |= addr & PTE_FRAME;
}

uint32_t    pt_entry_is_present(pt_entry entry) {
    return entry & PTE_PRESENT;
}

uint32_t    pt_entry_is_writable(pt_entry entry) {
    return entry & PTE_WRITABLE;
}

uint32_t    pt_entry_frame_addr(pt_entry entry) {
    return entry & PTE_FRAME;
}

uint32_t    pt_entry_is_user(pt_entry entry) {
    return entry & PTE_USER;
}