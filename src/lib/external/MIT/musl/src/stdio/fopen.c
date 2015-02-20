#include "stdio_impl.h"
#include <fcntl.h>
#include <string.h>
#include <errno.h>
/* @@@ */
#ifdef _MSC_VER

FILE* fopen(const char __restrict* filename, const char __restrict* mode)
{
	return __fdopen(filename, mode);
}
#else

FILE *fopen(const char *restrict filename, const char *restrict mode)
{
	FILE *f;
	int fd;
	int flags;

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)) {
		errno = EINVAL;
		return 0;
	}

	/* Compute the flags to pass to open() */
	flags = __fmodeflags(mode);

	fd = sys_open_cp(filename, flags, 0666);
	if (fd < 0) return 0;
	if (flags & O_CLOEXEC)
		__syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);

	f = __fdopen(fd, mode);
	if (f) return f;

	__syscall(SYS_close, fd);
	return 0;
}

LFS64(fopen);
#endif
