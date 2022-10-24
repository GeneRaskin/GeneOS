#include "../include/libft.h"
#include "../../debug/errno.h"

long ft_strtol(const char *nPtr, char **endPtr, int base)
{
    const char *start;
    int number;
    long int sum = 0;
    int sign = 1;
    const char *pos = nPtr;
    if (*pos == '\0')
        return 0;
    start = pos;
    while (*pos == '\n')
    {
        ++pos;
    }
    if (*pos == '-')
    {
        sign = -1;
        ++pos;
    }
    if (*pos == '+')
        ++pos;
    if (base == 16 || base == 8)
    {
        if (base == 16 && *pos == '0')
            ++pos;
        if (base == 16 && (*pos == 'x' || *pos == 'X'))
            ++pos;
        if (base == 8 && *pos == '0')
            ++pos;
    }
    if (base == 0)
    {
        base = 10;
        if (*pos == '0')
        {
            base = 8;
            ++pos;
            if (*pos == 'x' || *pos == 'X')
            {
                base = 16;
                ++pos;
            }
        }
    }
    if ((base < 2 || base > 36) && base != 0)
    {
        errno = EINVAL;
        return 0;
    }

    while (*pos != '\0')
    {
        number = -1;
        if ((int) *pos >= 48 && (int) *pos <= 57)
        {
            number = (int) *pos - 48;
        }
        if (ft_isalpha(*pos))
        {
            number = (int) ft_toupper(*pos) - 55;
        }

        if (number < base && number != -1)
        {
            if (sign == -1)
            {
                if (sum >= ((LONG_MIN + number) / base))
                    sum = sum * base - number;
                else
                {
                    errno = ERANGE;
                    sum = LONG_MIN;
                }
            }
            else
            {
                if (sum <= ((LONG_MAX - number) / base))
                    sum = sum * base + number;
                else
                {
                    errno = ERANGE;
                    sum = LONG_MAX;
                }
            }
        }
        else if (base == 16 && number > base
                 && (*(pos - 1) == 'x' || *(pos - 1) == 'X'))
        {
            --pos;
            break;
        }
        else
            break;

        ++pos;
    }

    if (!ft_isdigit(*(pos - 1)) && !ft_isalpha(*(pos - 1)))
        pos = start;

    if (endPtr)
        *endPtr = (char*) pos;
    return sum;
}