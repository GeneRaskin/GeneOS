/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_uint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/16 02:11:02 by                   #+#    #+#             */
/*   Updated: 2021/10/17 02:50:34 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

static void	ft_putuint(uint32_t n)
{
	if (n < 10)
		ft_putchar(n + '0');
	else
	{
		ft_putnbr(n / 10);
		ft_putchar(n % 10 + '0');
	}
}

static int	ft_print_uint_left_justified(t_state state)
{
	int		num_printed;
	int		num_zeros;
	int		trailing_spaces;

	num_printed = ft_count_field_width_left_justified(state,
			&num_zeros, &trailing_spaces);
	while (num_zeros-- > 0)
		ft_putchar('0');
	if (state.u_arg.uint_arg != 0 || state.precision != 0)
		ft_putuint(state.u_arg.uint_arg);
	while (trailing_spaces-- > 0)
		ft_putchar(' ');
	return (num_printed);
}

static int	ft_print_uint_default(t_state state)
{
	int		num_printed;
	int		leading_spaces;
	int		num_zeros;

	leading_spaces = 0;
	num_printed = ft_count_field_width_default(state,
			&num_zeros, &leading_spaces);
	while (leading_spaces-- > 0)
		ft_putchar(' ');
	while (num_zeros-- > 0)
		ft_putchar('0');
	if (state.u_arg.uint_arg != 0 || state.precision != 0)
		ft_putuint(state.u_arg.uint_arg);
	return (num_printed);
}

int	ft_handle_uint(t_state state)
{
	if (state.left_adjust)
		return (ft_print_uint_left_justified(state));
	else
		return (ft_print_uint_default(state));
}
