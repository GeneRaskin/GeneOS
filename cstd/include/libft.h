/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:  <>                                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/04 15:18:46 by                   #+#    #+#             */
/*   Updated: 2021/10/15 15:59:35 by                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

#define NULL (void *)0
#include <limits.h>
#include "stdarg.h"
#include "../../drivers/fb_write/fb_write.h"
#include "ft_printf.h"

extern char	itoa_buffer[12];

int			ft_isalpha(int c);
int			ft_isdigit(int c);
int			ft_isalnum(int c);
int			ft_isascii(int c);
int			ft_isprint(int c);
uint32_t		ft_strlen(const char *s);
void			*ft_memset(void *b, int c, uint32_t len);
void			ft_bzero(void *s, uint32_t n);
void			*ft_memcpy(void *dst, const void *src, uint32_t n);
void			*ft_memmove(void *dst, const void *src, uint32_t len);
uint32_t			ft_strlcpy(char *dst, const char *src, uint32_t dstsize);
uint32_t			ft_strlcat(char *dst, const char *src, uint32_t dstsize);
int				ft_toupper(int c);
int				ft_tolower(int c);
char			*ft_strchr(const char *s, int c);
char			*ft_strrchr(const char *s, int c);
char			*ft_strnstr(const char *haystack, const char *needle,
					uint32_t len);
int				ft_strncmp(const char *s1, const char *s2, uint32_t n);
void			*ft_memchr(const void *s, int c, uint32_t n);
int				ft_memcmp(const void *s1, const void *s2, uint32_t n);
int				ft_atoi(const char *str);
void			ft_putchar(char c);
void			ft_putstr(char *s);
void			ft_putendl(char *s);
void			ft_putnbr(int n);
char			*ft_itoa(int n);
char		    **ft_split(char const *s, char c, int *argc);
float           ft_strtof(const char *nptr, char **endptr);
long            ft_strtol(const char *nptr, char **endptr, int base);

#endif
