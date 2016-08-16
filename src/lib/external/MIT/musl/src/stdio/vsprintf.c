#include <stdio.h>
#include <limits.h>

// @@@
//int vsprintf(char *restrict s, const char *restrict fmt, va_list ap)
#ifdef _MSC_VER
int vsprintf(char __restrict* s, const char __restrict* fmt, __isoc_va_list ap)
#else
int vsprintf(char *restrict s, const char *restrict fmt, va_list ap)
#endif
// @@@
{
    return vsnprintf(s, INT_MAX, fmt, ap);
}
