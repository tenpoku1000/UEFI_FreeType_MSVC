#include <string.h>

char *__stpcpy(char *, const char *);

// @@@
//char *strcpy(char *restrict dest, const char *restrict src)
#ifdef _MSC_VER
char *strcpy(char __restrict* dest, const char __restrict* src)
#else
char *strcpy(char *restrict dest, const char *restrict src)
#endif
// @@@
{
#if 1
    __stpcpy(dest, src);
    return dest;
#else
    const unsigned char *s = src;
    unsigned char *d = dest;
    while ((*d++ = *s++));
    return dest;
#endif
}
