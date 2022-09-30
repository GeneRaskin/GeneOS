/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 18:07:58 by                   #+#    #+#             */
/*   Updated: 2021/10/15 15:59:35 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

uint32_t	ft_strlcpy(char *dst, const char *src, uint32_t dstsize)
{
	char		*d;
	const char	*s;
	uint32_t	n;

	d = dst;
	s = src;
	n = dstsize;
	if (n != 0)
	{
		while (--n > 0)
		{
			*d++ = *src++;
			if (*(src - 1) == '\0')
				break ;
		}
	}
	if (n == 0)
	{
		if (dstsize != 0)
			*d = '\0';
		while (*src++)
			;
	}
	return (src - s - 1);
}
