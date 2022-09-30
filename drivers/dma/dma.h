#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include "../io.h"

/* DMAC port 0 (slave) ports */
#define DMAC_SLAVE_STATUS_REG         0x08
#define DMAC_SLAVE_COMMAND_REG        0x08
#define DMAC_SLAVE_REQUEST_REG        0x09
#define DMAC_SLAVE_SINGLE_MASK_REG    0x0A
#define DMAC_SLAVE_MODE_REG           0x0B
#define DMAC_SLAVE_CLR_BPTR_FLFP      0x0C
#define DMAC_SLAVE_INTERMEDIATE_REG   0x0D
#define DMAC_SLAVE_MASTER_CLEAR       0x0D
#define DMAC_SLAVE_CLEAR_MASK_REG     0x0E
#define DMAC_SLAVE_WRT_MASK_REG       0x0F

/* DMAC port 0 (slave) channel ports (DRQ0-DRQ3) */
#define DMAC_SLAVE_ADDR0_PORT         0x00
#define DMAC_SLAVE_COUNTER0_PORT      0x01
#define DMAC_SLAVE_ADDR1_PORT         0x02
#define DMAC_SLAVE_COUNTER1_PORT      0x03
#define DMAC_SLAVE_ADDR2_PORT         0x04
#define DMAC_SLAVE_COUNTER2_PORT      0x05
#define DMAC_SLAVE_ADDR3_PORT         0x06
#define DMAC_SLAVE_COUNTER3_PORT      0x07

/* DMAC port 1 (master) ports */
#define DMAC_MASTER_STATUS_REG        0xD0
#define DMAC_MASTER_COMMAND_REG       0xD0
#define DMAC_MASTER_REQUEST_REG       0xD2
#define DMAC_MASTER_SINGLE_MASK_REG   0xD4
#define DMAC_MASTER_MODE_REG          0xD6
#define DMAC_MASTER_CLR_BPTR_FLFP     0xD8
#define DMAC_MASTER_INTERMEDIATE_REG  0xDA
#define DMAC_MASTER_MASTER_CLEAR      0xDA
#define DMAC_MASTER_CLEAR_MASK_REG    0xDC
#define DMAC_MASTER_WRT_MASK_REG      0xDE

/* DMAC port 1 (master) channel ports (DRQ4-DRQ7) */
#define DMAC_MASTER_ADDR4_PORT        0xC0
#define DMAC_MASTER_COUNTER4_PORT     0xC2
#define DMAC_MASTER_ADDR5_PORT        0xC4
#define DMAC_MASTER_COUNTER5_PORT     0xC6
#define DMAC_MASTER_ADDR6_PORT        0xC8
#define DMAC_MASTER_COUNTER6_PORT     0xCA
#define DMAC_MASTER_ADDR7_PORT        0xCC
#define DMAC_MASTER_COUNTER7_PORT     0xCE

/* ISA DMAC extended page address registers */
#define DMAC_DIAG_PORT                0x80
#define DMAC_SLAVE_ADDR2_EXTRA_PORT   0x81
#define DMAC_SLAVE_ADDR3_EXTRA_PORT   0x82
#define DMAC_SLAVE_ADDR1_EXTRA_PORT   0x83
#define DMAC_MASTER_ADDR6_EXTRA_PORT  0x87
#define DMAC_MASTER_ADDR7_EXTRA_PORT  0x88
#define DMAC_MASTER_ADDR5_EXTRA_PORT  0x89

/* DMAC command register flags */
#define DMAC_CMD_REG_ENABLE_FLAG      0x04

/* DMAC mode register flags and bitmasks*/
#define DMAC_SEL_MASK                 0x03
#define DMAC_TRA_MASK                 0x0C
#define DMAC_AUTO_MASK                0x10
#define DMAC_IDEC_MASK                0x20
#define DMAC_MOD_MASK                 0xC0
#define DMAC_MODE_SELF_TEST           0x00
#define DMAC_MODE_READ_TRANSFER       0x04
#define DMAC_MODE_WRITE_TRANSFER      0x08
#define DMAC_MODE_TRANSFER_ON_DEMAND  0x00
#define DMAC_MODE_TRANSFER_SINGLE     0x40
#define DMAC_MODE_TRANSFER_BLOCK      0x80
#define DMAC_MODE_TRANSFER_CASCADE    0xC0

void    dmac_set_addr(uint8_t drq, uint16_t addr_word);
void    dmac_set_count(uint8_t drq, uint16_t count_word);
void    dmac_set_external_page_reg(uint8_t reg, uint8_t page_val);
void    dmac_set_mode(uint8_t channel, uint8_t mode);
void    dmac_set_read(uint8_t channel);
void    dmac_set_write(uint8_t channel);
void    dmac_mask_channel(uint8_t channel);
void    dmac_unmask_channel(uint8_t channel);
void    dmac_unmask_all();

#endif
