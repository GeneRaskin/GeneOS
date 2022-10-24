#include "debug.h"
#include "../cstd/include/libft.h"

void    print_mem(uint8_t *buf, uint32_t num_bytes) {
    for (uint32_t offset = 0; offset < num_bytes; offset += 0x1A) {
        ft_printf("%08x ", offset);
        for (uint32_t curr_byte = offset; curr_byte < offset + 0x1A
            && curr_byte < num_bytes; curr_byte += 2) {
            ft_printf("%02x%02x ", buf[curr_byte], buf[curr_byte + 1]);
        }
        ft_printf("\n");
    }
}