/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_hex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 01:01:00 by                   #+#    #+#             */
/*   Updated: 2021/10/17 02:24:21 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

static void	ft_puthex(t_state state)
{
	if (state.data_type == HEX_LOWERCASE)
		convert_to_base(state.u_arg.uint_arg, "0123456789abcdef");
	else if (state.data_type == HEX_UPPERCASE)
		convert_to_base(state.u_arg.uint_arg, "0123456789ABCDEF");
}

static int	ft_print_hex_left_justified(t_state state)
{
	int		num_printed;
	int		num_zeros;
	int		trailing_spaces;

	num_printed = ft_count_field_width_left_justified(state,
			&num_zeros, &trailing_spaces);
	if (state.alternate_output && state.data_type == HEX_LOWERCASE)
		ft_putstr("0x");
	else if (state.alternate_output && state.data_type == HEX_UPPERCASE)
		ft_putstr("0X");
	while (num_zeros-- > 0)
		ft_putchar('0');
	if (state.u_arg.uint_arg != 0 || state.precision != 0)
		ft_puthex(state);
	while (trailing_spaces-- > 0)
		ft_putchar(' ');
	return (num_printed);
}

static int	ft_print_hex_default(t_state state)
{
	int		num_printed;
	int		leading_spaces;
	int		num_zeros;

	leading_spaces = 0;
	num_printed = ft_count_field_width_default(state,
			&num_zeros, &leading_spaces);
	while (leading_spaces-- > 0)
		ft_putchar(' ');
	if (state.alternate_output && state.data_type == HEX_LOWERCASE)
		ft_putstr("0x");
	else if (state.alternate_output && state.data_type == HEX_UPPERCASE)
		ft_putstr("0X");
	while (num_zeros-- > 0)
		ft_putchar('0');
	if (state.u_arg.uint_arg != 0 || state.precision != 0)
		ft_puthex(state);
	return (num_printed);
}

int	ft_handle_hex(t_state state)
{
	if (state.u_arg.uint_arg == 0 && state.alternate_output)
		state.alternate_output = 0;
	if (state.left_adjust)
		return (ft_print_hex_left_justified(state));
	else
		return (ft_print_hex_default(state));
}
