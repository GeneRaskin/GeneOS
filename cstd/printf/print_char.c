/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_char.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 04:04:38 by                   #+#    #+#             */
/*   Updated: 2021/10/17 04:09:48 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

int	ft_handle_char(t_state state)
{
	int	num_printed;

	num_printed = 1;
	if (state.left_adjust)
	{
		ft_putchar(state.u_arg.int_arg);
		while (--state.width > 0 && ++num_printed)
			ft_putchar(' ');
	}
	else
	{
		while (--state.width > 0 && ++num_printed)
			ft_putchar(' ');
		ft_putchar(state.u_arg.int_arg);
	}
	return (num_printed);
}
