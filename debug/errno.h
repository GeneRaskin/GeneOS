#ifndef ERRNO_H
#define ERRNO_H

#include <stdint.h>
extern uint32_t errno;

#define EIO     5  /* I/O error */
#define ENOMEM  12 /* Out of memory */
#define EINVAL  22 /* Invalid argument */
#define ERANGE  34
#define ENOSYS  35 /* No such system call */

#endif