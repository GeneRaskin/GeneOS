/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 15:48:16 by                   #+#    #+#             */
/*   Updated: 2021/10/06 16:02:32 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

int	ft_memcmp(const void *s1, const void *s2, uint32_t n)
{
	const uint8_t	*str1;
	const uint8_t	*str2;

	if (n == 0)
		return (0);
	str1 = (const uint8_t *)s1;
	str2 = (const uint8_t *)s2;
	while (*str1 == *str2 && --n)
	{
		str1++;
		str2++;
	}
	return (*str1 - *str2);
}
