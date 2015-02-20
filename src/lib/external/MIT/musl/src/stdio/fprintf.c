#include <stdio.h>
#include <stdarg.h>

/* @@@ */
#ifdef _MSC_VER
int fprintf(FILE __restrict* f, const char __restrict* fmt, ...)
#else
int fprintf(FILE *restrict f, const char *restrict fmt, ...)
#endif
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vfprintf(f, fmt, ap);
	va_end(ap);
	return ret;
}
