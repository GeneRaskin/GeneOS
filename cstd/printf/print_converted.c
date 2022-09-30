/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_converted.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 00:32:51 by                   #+#    #+#             */
/*   Updated: 2021/10/17 04:04:12 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

int	ft_print_converted(t_state state)
{
	int	num_printed;

	num_printed = 0;
	if (state.data_type == UNSIGNED_DECIMAL)
		num_printed = ft_handle_uint(state);
	else if (state.data_type == SIGNED_DECIMAL)
		num_printed = ft_handle_int(state);
	else if (state.data_type == HEX_LOWERCASE
		|| state.data_type == HEX_UPPERCASE)
		num_printed = ft_handle_hex(state);
	else if (state.data_type == CHAR_PTR)
		num_printed = ft_handle_str(state);
	else if (state.data_type == VOID_PTR)
		num_printed = ft_handle_vptr(state);
	else if (state.data_type == SIGNED_CHAR)
		num_printed = ft_handle_char(state);
	return (num_printed);
}
