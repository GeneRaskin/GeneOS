/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interp_specs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 21:56:43 by                   #+#    #+#             */
/*   Updated: 2021/10/15 19:51:02 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

void	ft_cast_arg(t_state *state, int arg_type, va_list args)
{
	if (arg_type == SIGNED_DECIMAL || arg_type == SIGNED_CHAR)
		state->u_arg.int_arg = va_arg(args, int);
	else if (arg_type == CHAR_PTR)
		state->u_arg.char_ptr_arg = va_arg(args, char *);
	else if (arg_type == VOID_PTR)
		state->u_arg.void_ptr_arg = va_arg(args, void *);
	else if (arg_type == UNSIGNED_DECIMAL || arg_type == HEX_UPPERCASE
		|| arg_type == HEX_LOWERCASE)
		state->u_arg.uint_arg = va_arg(args, uint32_t);
}

int	ft_get_conversion(const char **format, t_state *state, va_list args)
{
	if (!**format)
		return (0);
	if (**format == 'c')
		state->data_type = SIGNED_CHAR;
	else if (**format == 's')
		state->data_type = CHAR_PTR;
	else if (**format == 'd' || **format == 'i')
		state->data_type = SIGNED_DECIMAL;
	else if (**format == 'p')
		state->data_type = VOID_PTR;
	else if (**format == 'u')
		state->data_type = UNSIGNED_DECIMAL;
	else if (**format == 'x')
		state->data_type = HEX_LOWERCASE;
	else if (**format == 'X')
		state->data_type = HEX_UPPERCASE;
	ft_cast_arg(state, state->data_type, args);
	(*format)++;
	return (1);
}

int	ft_get_flags(const char **format, t_state *state)
{
	while (**format && ft_strchr(" #+-0", **format))
	{
		if (**format == ' ' && !state->print_sign)
			state->print_space = 1;
		else if (**format == '+')
		{
			state->print_space = 0;
			state->print_sign = 1;
		}
		else if (**format == '0' && !state->left_adjust)
			state->zero_padded = 1;
		else if (**format == '-')
		{
			state->zero_padded = 0;
			state->left_adjust = 1;
		}
		else if (**format == '#')
			state->alternate_output = 1;
		(*format)++;
	}
	if (!**format)
		return (0);
	return (1);
}

int	ft_get_width(const char **format, t_state *state)
{
	int	res;

	res = 0;
	while (**format && ft_isdigit(**format))
	{
		res = res * 10 + (**format - '0');
		(*format)++;
	}
	state->width = res;
	if (!**format)
		return (0);
	return (1);
}

int	ft_get_precision(const char **format, t_state *state)
{
	int	precision;

	precision = 0;
	if (**format != '.')
		return (1);
	(*format)++;
	while (**format && ft_isdigit(**format))
	{
		precision = precision * 10 + (**format - '0');
		(*format)++;
	}
	if (!**format)
		return (0);
	state->precision = precision;
	return (1);
}
