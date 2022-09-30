/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_numlen_int.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 17:33:42 by                   #+#    #+#             */
/*   Updated: 2021/10/17 03:29:09 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

int	ft_numlen_from_state_ptr(t_state state)
{
	int		len;
	size_t	ptr_addr;

	len = 0;
	ptr_addr = (intptr_t) state.u_arg.void_ptr_arg;
	if (ptr_addr == 0 && state.precision != 0)
		return (1);
	while (ptr_addr > 0)
	{
		ptr_addr /= 0x10;
		len++;
	}
	return (len);
}

int	ft_numlen_from_state_hex(t_state state)
{
	int	len;

	len = 0;
	if (state.u_arg.uint_arg == 0 && state.precision != 0)
		return (1);
	while (state.u_arg.uint_arg > 0)
	{
		state.u_arg.uint_arg /= 0x10;
		len++;
	}
	return (len);
}

int	ft_numlen_from_state_decimal(t_state state)
{
	int				len;
	t_int			n;

	len = 0;
	if (state.data_type == SIGNED_DECIMAL)
	{
		n.int_var = state.u_arg.int_arg;
		if (n.int_var == INT_MIN)
			return (10);
		if (n.int_var == 0 && state.precision != 0)
			return (1);
		if (n.int_var < 0)
			n.int_var *= (-1);
		while (n.int_var > 0 && ++len)
			n.int_var /= 10;
	}
	else if (state.data_type == UNSIGNED_DECIMAL)
	{
		n.uint_var = state.u_arg.uint_arg;
		if (n.uint_var == 0 && state.precision != 0)
			return (1);
		while (n.uint_var > 0 && ++len)
			n.uint_var /= 10;
	}
	return (len);
}
