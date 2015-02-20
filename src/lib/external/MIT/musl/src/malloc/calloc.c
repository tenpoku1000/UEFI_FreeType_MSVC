#include <stdlib.h>
#include <errno.h>
/* @@@ */
#ifdef _MSC_VER
#include <string.h>
#endif

void *calloc(size_t m, size_t n)
{
	void *p;
	size_t *z;
	if (n && m > (size_t)-1/n) {
		errno = ENOMEM;
		return 0;
	}
	n *= m;
	p = malloc(n);
	if (!p) return 0;
	/* @@@ */
#ifdef _MSC_VER
	memset(p, 0, n);
#else
	/* Only do this for non-mmapped chunks */
	if (((size_t *)p)[-1] & 7) {
		/* Only write words that are not already zero */
		m = (n + sizeof *z - 1)/sizeof *z;
		for (z=p; m; m--, z++) if (*z) *z=0;
	}
#endif
	return p;
}
