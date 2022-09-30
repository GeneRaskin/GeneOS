#include "dma.h"

void    dmac_set_addr(uint8_t drq, uint16_t addr_word) {
    if (drq > 7)
        return;
    uint8_t low_addr = addr_word & 0xFF;
    uint8_t high_addr = addr_word >> 8;
    uint16_t port = 0;
    switch (drq) {
        case 0: {port = DMAC_SLAVE_ADDR0_PORT; break;}
        case 1: {port = DMAC_SLAVE_ADDR1_PORT; break;}
        case 2: {port = DMAC_SLAVE_ADDR2_PORT; break;}
        case 3: {port = DMAC_SLAVE_ADDR3_PORT; break;}
        case 4: {port = DMAC_MASTER_ADDR4_PORT; break;}
        case 5: {port = DMAC_MASTER_ADDR5_PORT; break;}
        case 6: {port = DMAC_MASTER_ADDR6_PORT; break;}
        case 7: {port = DMAC_MASTER_ADDR7_PORT; break;}
    }
    outb(port, low_addr);
    outb(port, high_addr);
}

void    dmac_set_count(uint8_t drq, uint16_t count_word) {
    if (drq > 7)
        return;
    uint8_t low_count = count_word & 0xFF;
    uint8_t high_count = count_word >> 8;
    uint16_t port = 0;
    switch (drq) {
        case 0: {port = DMAC_SLAVE_COUNTER0_PORT; break;}
        case 1: {port = DMAC_SLAVE_COUNTER1_PORT; break;}
        case 2: {port = DMAC_SLAVE_COUNTER2_PORT; break;}
        case 3: {port = DMAC_SLAVE_COUNTER3_PORT; break;}
        case 4: {port = DMAC_MASTER_COUNTER4_PORT; break;}
        case 5: {port = DMAC_MASTER_COUNTER5_PORT; break;}
        case 6: {port = DMAC_MASTER_COUNTER6_PORT; break;}
        case 7: {port = DMAC_MASTER_COUNTER7_PORT; break;}
    }
    outb(port, low_count);
    outb(port, high_count);
}

void    dmac_set_external_page_reg(uint8_t reg, uint8_t page_val) {
    if (reg > 7 || reg < 1 || reg == 4)
        return;
    uint8_t port = 0;
    switch (reg) {
        case 1: {port = DMAC_SLAVE_ADDR1_EXTRA_PORT; break;}
        case 2: {port = DMAC_SLAVE_ADDR2_EXTRA_PORT; break;}
        case 3: {port = DMAC_SLAVE_ADDR3_EXTRA_PORT; break;}
        case 5: {port = DMAC_MASTER_ADDR5_EXTRA_PORT; break;}
        case 6: {port = DMAC_MASTER_ADDR6_EXTRA_PORT; break;}
        case 7: {port = DMAC_MASTER_ADDR7_EXTRA_PORT; break;}
    }
    outb(port, page_val);
}

void    dmac_set_mode(uint8_t channel, uint8_t mode) {
    if (channel > 7)
        return;
    int dma = (channel < 4) ? 0 : 1;
    int chan = (dma == 0) ? channel : channel - 4;
    dmac_mask_channel(channel);
    outb((channel < 4) ? (DMAC_SLAVE_MODE_REG) : DMAC_MASTER_MODE_REG,
         chan | (mode));
    dmac_unmask_all();
}

void    dmac_set_read(uint8_t channel) {
    dmac_set_mode(channel, DMAC_AUTO_MASK | DMAC_MODE_READ_TRANSFER |
        DMAC_MODE_TRANSFER_SINGLE);
}

void    dmac_set_write(uint8_t channel) {
    dmac_set_mode(channel, DMAC_AUTO_MASK | DMAC_MODE_WRITE_TRANSFER |
        DMAC_MODE_TRANSFER_SINGLE);
}

void    dmac_mask_channel(uint8_t channel) {
    if (channel > 7)
        return;
    if (channel < 4)
        outb(DMAC_SLAVE_SINGLE_MASK_REG, (1 << channel));
    else
        outb(DMAC_MASTER_SINGLE_MASK_REG, (1 << (channel - 4)));
}

void    dmac_unmask_channel(uint8_t channel) {
    if (channel > 7)
        return;
    if (channel < 4)
        outb(DMAC_SLAVE_SINGLE_MASK_REG, (1 << channel));
}

void    dmac_unmask_all() {
    outb(DMAC_MASTER_CLEAR_MASK_REG, 0xFF);
}
