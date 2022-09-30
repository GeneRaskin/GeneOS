#ifndef _STDC_STDARG_H_
#define _STDC_STDARG_H_

typedef unsigned int		size_t;

typedef size_t*			va_list;
#define va_start(l,p) 		((l) = ((size_t*)(&(p))) + 1)
#define va_arg(l,t)		(*((t*)(l++)))
#define va_end(l)		((void)0)
#define va_copy(d,s)		((d) = (s))

#endif
