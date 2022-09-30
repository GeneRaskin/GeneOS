/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_int.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:56:41 by                   #+#    #+#             */
/*   Updated: 2021/10/17 03:02:47 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

int	ft_count_field_width_left_justified(t_state state,
												int *num_zeros,
												int *trailing_spaces)
{
	int	numlen;

	numlen = 0;
	if (state.data_type == SIGNED_DECIMAL
		|| state.data_type == UNSIGNED_DECIMAL)
		numlen = ft_numlen_from_state_decimal(state);
	else if (state.data_type == HEX_UPPERCASE
		|| state.data_type == HEX_LOWERCASE)
		numlen = ft_numlen_from_state_hex(state);
	else if (state.data_type == VOID_PTR)
		numlen = ft_numlen_from_state_ptr(state);
	*num_zeros = 0;
	*trailing_spaces = 0;
	if (state.precision != -1 && state.precision > numlen)
		*num_zeros = state.precision - numlen;
	numlen += state.has_sign + 2 * state.alternate_output;
	if (numlen + *num_zeros < state.width)
		*trailing_spaces = state.width - numlen - *num_zeros;
	return (numlen + *num_zeros + *trailing_spaces);
}

static int	ft_print_int_left_justified(t_state state)
{
	int		num_printed;
	int		num_zeros;
	int		trailing_spaces;
	char	*number;

	num_printed = ft_count_field_width_left_justified(state,
			&num_zeros, &trailing_spaces);
	if (state.print_space && state.u_arg.int_arg >= 0)
		ft_putchar(' ');
	else if (state.print_sign && state.u_arg.int_arg >= 0)
		ft_putchar('+');
	else if (state.u_arg.int_arg < 0)
		ft_putchar('-');
	while (num_zeros-- > 0)
		ft_putchar('0');
	number = ft_itoa(state.u_arg.int_arg);
	if (state.u_arg.int_arg != 0 || state.precision != 0)
	{
		if (state.u_arg.int_arg < 0)
			ft_putstr(number + 1);
		else
			ft_putstr(number);
	}
	while (trailing_spaces-- > 0)
		ft_putchar(' ');
	return (num_printed);
}

int	ft_count_field_width_default(t_state state,
											 int *num_zeros,
											 int *leading_spaces)
{
	int	numlen;

	numlen = 0;
	if (state.data_type == SIGNED_DECIMAL
		|| state.data_type == UNSIGNED_DECIMAL)
		numlen = ft_numlen_from_state_decimal(state);
	else if (state.data_type == HEX_UPPERCASE
		|| state.data_type == HEX_LOWERCASE)
		numlen = ft_numlen_from_state_hex(state);
	else if (state.data_type == VOID_PTR)
		numlen = ft_numlen_from_state_ptr(state);
	*num_zeros = 0;
	if (state.precision != -1)
	{
		if (state.precision > numlen)
			*num_zeros = state.precision - numlen;
		if (state.width > numlen + state.has_sign + *num_zeros
			+ 2 * state.alternate_output)
			*leading_spaces = state.width - numlen
				- *num_zeros - state.has_sign - 2 * state.alternate_output;
	}
	else
	{
		if (state.width > numlen + state.has_sign + 2 * state.alternate_output)
		{
			if (state.zero_padded)
				*num_zeros = state.width - numlen - state.has_sign
					- 2 * state.alternate_output;
			else
				*leading_spaces = state.width - numlen - state.has_sign
					- 2 * state.alternate_output;
		}
	}
	return (numlen + *leading_spaces + *num_zeros + state.has_sign
		+ 2 * state.alternate_output);
}

static int	ft_print_int_default(t_state state)
{
	int		num_printed;
	int		leading_spaces;
	int		num_zeros;
	char	*number;

	leading_spaces = 0;
	num_printed = ft_count_field_width_default(state,
			&num_zeros, &leading_spaces);
	while (leading_spaces-- > 0)
		ft_putchar(' ');
	if (state.print_space && state.u_arg.int_arg >= 0)
		ft_putchar(' ');
	else if (state.print_sign && state.u_arg.int_arg >= 0)
		ft_putchar('+');
	else if (state.u_arg.int_arg < 0)
		ft_putchar('-');
	while (num_zeros-- > 0)
		ft_putchar('0');
	number = ft_itoa(state.u_arg.int_arg);
	if (state.u_arg.int_arg != 0 || state.precision != 0)
	{
		if (state.u_arg.int_arg < 0)
			ft_putstr(number + 1);
		else
			ft_putstr(number);
	}
	return (num_printed);
}

int	ft_handle_int(t_state state)
{
	if ((state.u_arg.int_arg >= 0 && (state.print_sign || state.print_space))
		|| state.u_arg.int_arg < 0)
		state.has_sign = 1;
	if (state.left_adjust)
		return (ft_print_int_left_justified(state));
	else
		return (ft_print_int_default(state));
}
