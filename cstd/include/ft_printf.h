/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 19:20:35 by                   #+#    #+#             */
/*   Updated: 2021/10/17 04:05:29 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"

typedef uint32_t size_t;

typedef struct s_state {
	int	data_type;
	int	precision;
	int	left_adjust;
	int	print_sign;
	int	print_space;
	int	zero_padded;
	int	alternate_output;
	int	width;
	int	has_sign;
	union u_arg
	{
		int				int_arg;
		uint32_t	uint_arg;
		void			*void_ptr_arg;
		char			*char_ptr_arg;
	} u_arg;
}	t_state;

typedef union u_int_
{
	uint32_t	uint_var;
	int				int_var;
}	t_int;

enum {
	UNSIGNED_DECIMAL = 1,
	HEX_UPPERCASE,
	HEX_LOWERCASE,
	SIGNED_DECIMAL,
	CHAR_PTR,
	VOID_PTR,
	SIGNED_CHAR
};

int				ft_printf(const char *format, ...);
void			ft_cast_arg(t_state *state, int arg_type, va_list args);
int				ft_get_conversion(const char **format, t_state *state,
					va_list args);
int				ft_get_flags(const char **format, t_state *state);
int				ft_get_width(const char **format, t_state *state);
int				ft_get_precision(const char **format, t_state *state);
int				ft_print_converted(t_state state);
int				ft_numlen_from_state_decimal(t_state state);
int				ft_numlen_from_state_hex(t_state state);
int				ft_numlen_from_state_ptr(t_state state);
int				ft_handle_int(t_state state);
int				ft_handle_uint(t_state state);
int				ft_handle_hex(t_state state);
int				ft_handle_vptr(t_state state);
int				ft_handle_char(t_state state);
int				ft_handle_str(t_state state);
int				ft_count_field_width_left_justified(t_state state,
					int32_t*num_zeros, int *trailing_spaces);
int				ft_count_field_width_default(t_state state,
					int *num_zeros, int *leading_spaces);
void			convert_to_base(size_t nbr, char *base);

#endif
