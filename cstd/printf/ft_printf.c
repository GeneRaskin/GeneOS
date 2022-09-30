/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 19:22:35 by                   #+#    #+#             */
/*   Updated: 2021/10/15 19:49:20 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_printf.h"

static t_state	ft_init_state(void)
{
	t_state	state;

	state.alternate_output = 0;
	state.data_type = 0;
	state.left_adjust = 0;
	state.print_sign = 0;
	state.zero_padded = 0;
	state.width = 0;
	state.precision = -1;
	state.print_space = 0;
	state.has_sign = 0;
	return (state);
}

static int	ft_convert(const char **format, va_list args)
{
	int		num_printed;
	t_state	state;

	num_printed = 0;
	if (!**format)
		return (num_printed);
	state = ft_init_state();
	if (**format == '%')
	{
		ft_putchar(**format);
		(*format)++;
		return (1);
	}
	if (!ft_get_flags(format, &state))
		return (0);
	if (!ft_get_width(format, &state))
		return (0);
	if (!ft_get_precision(format, &state))
		return (0);
	if (!ft_get_conversion(format, &state, args))
		return (0);
	num_printed += ft_print_converted(state);
	return (num_printed);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		num_printed;

	num_printed = 0;
	if (format == NULL)
		return (0);
	va_start(args, format);
	while (*format)
	{
		if (*format != '%')
		{
			ft_putchar(*format);
			num_printed++;
			format++;
		}
		else
		{
			format++;
			num_printed += ft_convert(&format, args);
			args++;
		}
	}
	va_end(args);
	return (num_printed);
}
