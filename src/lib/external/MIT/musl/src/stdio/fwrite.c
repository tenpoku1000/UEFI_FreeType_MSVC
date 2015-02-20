#include "stdio_impl.h"
#include <string.h>

/* @@@ */
#ifdef _MSC_VER
size_t __fwritex(const unsigned char __restrict* s, size_t l, FILE __restrict* f)
#else
size_t __fwritex(const unsigned char *restrict s, size_t l, FILE *restrict f)
#endif
{
	size_t i=0;

	if (!f->wend && __towrite(f)) return 0;

	if (l > f->wend - f->wpos) return f->write(f, s, l);

	if (f->lbf >= 0) {
		/* Match /^(.*\n|)/ */
		for (i=l; i && s[i-1] != '\n'; i--);
		if (i) {
			if (f->write(f, s, i) < i)
				return i;
			s += i;
			l -= i;
		}
	}

	memcpy(f->wpos, s, l);
	f->wpos += l;
	return l+i;
}

/* @@@ */
#ifdef _MSC_VER
size_t fwrite(const void __restrict* src, size_t size, size_t nmemb, FILE __restrict* f)
#else
size_t fwrite(const void *restrict src, size_t size, size_t nmemb, FILE *restrict f)
#endif
{
	size_t k, l = size*nmemb;
	FLOCK(f);
	k = __fwritex(src, l, f);
	FUNLOCK(f);
	return k==l ? nmemb : k/size;
}

/* @@@ */
#ifndef _MSC_VER
weak_alias(fwrite, fwrite_unlocked);
#endif
