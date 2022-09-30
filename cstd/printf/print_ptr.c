/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 03:32:10 by                   #+#    #+#             */
/*   Updated: 2021/10/17 03:44:31 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

static void	ft_putptr(t_state state)
{
	size_t	ptr_addr;

	ptr_addr = (intptr_t) state.u_arg.void_ptr_arg;
	convert_to_base(ptr_addr, "0123456789abcdef");
}

static int	ft_print_ptr_left_justified(t_state state)
{
	int		num_printed;
	int		num_zeros;
	int		trailing_spaces;

	num_printed = ft_count_field_width_left_justified(state,
			&num_zeros, &trailing_spaces);
	ft_putstr("0x");
	while (num_zeros-- > 0)
		ft_putchar('0');
	ft_putptr(state);
	while (trailing_spaces-- > 0)
		ft_putchar(' ');
	return (num_printed);
}

static int	ft_print_ptr_default(t_state state)
{
	int		num_printed;
	int		leading_spaces;
	int		num_zeros;

	leading_spaces = 0;
	num_printed = ft_count_field_width_default(state,
			&num_zeros, &leading_spaces);
	while (leading_spaces-- > 0)
		ft_putchar(' ');
	ft_putstr("0x");
	while (num_zeros-- > 0)
		ft_putchar('0');
	ft_putptr(state);
	return (num_printed);
}

int	ft_handle_vptr(t_state state)
{
	state.alternate_output = 1;
	if (state.left_adjust)
		return (ft_print_ptr_left_justified(state));
	else
		return (ft_print_ptr_default(state));
}
