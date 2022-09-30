/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 16:44:09 by                   #+#    #+#             */
/*   Updated: 2021/10/06 16:32:57 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/libft.h"

void	*ft_memmove(void *dst, const void *src, uint32_t len)
{
	char	*s;
	char	*d;
	char	*lastd;
	char	*lasts;

	if (dst == NULL && src == NULL)
		return (NULL);
	s = (char *)src;
	d = dst;
	if (d < s)
		while (len--)
			*d++ = *s++;
	else
	{
		lastd = d + (len - 1);
		lasts = s + (len - 1);
		while (len--)
			*lastd-- = *lasts--;
	}
	return (dst);
}
