/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msilk <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 16:51:04 by msilk             #+#    #+#             */
/*   Updated: 2021/10/17 01:56:02 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

static int	g_converted[50];

static int	ft_radixlen(char *str)
{
	int	len;

	len = 0;
	while (*str++)
		len++;
	return (len);
}

void	convert_to_base(size_t nbr, char *base)
{
	int		baselen;
	int		i;
	char	curr_char;

	i = 0;
	baselen = ft_radixlen(base);
	if (nbr == 0)
	{
		ft_putchar('0');
		return ;
	}
	while (nbr)
	{
		g_converted[i] = nbr % baselen;
		nbr /= baselen;
		i++;
	}
	while (i--)
	{
		curr_char = base[g_converted[i]];
		ft_putchar(curr_char);
	}
}
