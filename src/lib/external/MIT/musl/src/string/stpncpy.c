#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "libc.h"

#define ALIGN (sizeof(size_t)-1)
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

/* @@@ */
#ifdef _MSC_VER
char *__stpncpy(char __restrict* d, const char __restrict* s, size_t n)
#else
char *__stpncpy(char *restrict d, const char *restrict s, size_t n)
#endif
{
	size_t *wd;
	const size_t *ws;

	if (((uintptr_t)s & ALIGN) == ((uintptr_t)d & ALIGN)) {
		for (; ((uintptr_t)s & ALIGN) && n && (*d=*s); n--, s++, d++);
		if (!n || !*s) goto tail;
		wd=(void *)d; ws=(const void *)s;
		for (; n>=sizeof(size_t) && !HASZERO(*ws);
		       n-=sizeof(size_t), ws++, wd++) *wd = *ws;
		d=(void *)wd; s=(const void *)ws;
	}
	for (; n && (*d=*s); n--, s++, d++);
tail:
	memset(d, 0, n);
	return d;
}

/* @@@ */
#ifndef _MSC_VER
weak_alias(__stpncpy, stpncpy);
#endif
