#include <string.h>

char *__stpncpy(char *, const char *, size_t);

/* @@@ */
#ifdef _MSC_VER
char *strncpy(char __restrict* d, const char __restrict* s, size_t n)
#else
char *strncpy(char *restrict d, const char *restrict s, size_t n)
#endif
{
	__stpncpy(d, s, n);
	return d;
}
