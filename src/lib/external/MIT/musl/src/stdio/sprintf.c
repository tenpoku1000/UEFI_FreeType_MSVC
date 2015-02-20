#include <stdio.h>
#include <stdarg.h>

/* @@@ */
#ifdef _MSC_VER
int sprintf(char __restrict* s, const char __restrict* fmt, ...)
#else
int sprintf(char *restrict s, const char *restrict fmt, ...)
#endif
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vsprintf(s, fmt, ap);
	va_end(ap);
	return ret;
}
