/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/17 04:16:05 by                   #+#    #+#             */
/*   Updated: 2021/10/17 04:56:03 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

static int	ft_count_size_and_spaces(t_state state, int *size,
								int *num_spaces)
{
	*num_spaces = 0;
	*size = 0;
	if (state.u_arg.char_ptr_arg != NULL)
		*size = ft_strlen(state.u_arg.char_ptr_arg);
	else
		*size = 6;
	if (*size > state.precision && state.precision != -1)
		*size = state.precision;
	if (*size < state.width)
		*num_spaces = state.width - *size;
	return (*num_spaces + *size);
}

int	ft_handle_str(t_state state)
{
	int	size;
	int	num_spaces;
	int	num_printed;

	num_printed = ft_count_size_and_spaces(state, &size, &num_spaces);
	if (state.u_arg.char_ptr_arg == NULL)
		state.u_arg.char_ptr_arg = "(null)";
	if (state.left_adjust)
	{
		while (*state.u_arg.char_ptr_arg && size-- > 0)
			ft_putchar(*state.u_arg.char_ptr_arg++);
		while (num_spaces-- > 0)
			ft_putchar(' ');
	}
	else
	{
		while (num_spaces-- > 0)
			ft_putchar(' ');
		while (size-- > 0)
			ft_putchar(*state.u_arg.char_ptr_arg++);
	}
	return (num_printed);
}
