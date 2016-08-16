#include <string.h>

// @@@
//char *strcat(char *restrict dest, const char *restrict src)
#ifdef _MSC_VER
char *strcat(char __restrict* dest, const char __restrict* src)
#else
char *strcat(char *restrict dest, const char *restrict src)
#endif
// @@@
{
    strcpy(dest + strlen(dest), src);
    return dest;
}
