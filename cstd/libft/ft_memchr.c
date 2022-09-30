/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/06 16:03:38 by                   #+#    #+#             */
/*   Updated: 2021/10/06 16:12:09 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

void	*ft_memchr(const void *s, int32_t c, uint32_t n)
{
	const uint8_t	*str;

	str = (const uint8_t *)s;
	while (n--)
	{
		if (*str == (uint8_t)c)
			return ((void *)str);
		str++;
	}
	return (NULL);
}