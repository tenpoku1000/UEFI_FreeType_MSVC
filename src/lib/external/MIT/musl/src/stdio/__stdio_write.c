#include "stdio_impl.h"
#include <sys/uio.h>
#include <pthread.h>
/* @@@ */
#ifdef _MSC_VER
#include<errno.h>
#include<efi.h>
#include<efilib.h>
#if defined(_DEBUG)
#include "win64env.h"
#endif

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
#if !defined(_DEBUG)
	if ((NULL == f) || (NULL == f->efi_file) || (NULL == buf) || (0 == len)){

		errno = EINVAL;

		return -1;
	}

	EFI_FILE* efi_file = f->efi_file;

	EFI_STATUS status = efi_file->Write(efi_file, &len, (void*)&buf);

	if (EFI_ERROR(status)){

		f->flags |= F_ERR;

		errno = EACCES;

		return len;
	}
#else
	if ((NULL == f) || (NULL == f->win64_file) || (NULL == buf) || (0 == len)){

		errno = EINVAL;

		return -1;
	}

	int status = write_file(f->win64_file, buf, &len);

	if (FALSE == status){

		f->flags |= F_ERR;

		errno = EACCES;

		return len;
	}
#endif

	return len;
}
#else

static void cleanup(void *p)
{
	FILE *f = p;
	if (!f->lockcount) __unlockfile(f);
}

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
	struct iovec iovs[2] = {
		{ .iov_base = f->wbase, .iov_len = f->wpos-f->wbase },
		{ .iov_base = (void *)buf, .iov_len = len }
	};
	struct iovec *iov = iovs;
	size_t rem = iov[0].iov_len + iov[1].iov_len;
	int iovcnt = 2;
	ssize_t cnt;
	for (;;) {
		pthread_cleanup_push(cleanup, f);
		cnt = syscall_cp(SYS_writev, f->fd, iov, iovcnt);
		pthread_cleanup_pop(0);
		if (cnt == rem) {
			f->wend = f->buf + f->buf_size;
			f->wpos = f->wbase = f->buf;
			return len;
		}
		if (cnt < 0) {
			f->wpos = f->wbase = f->wend = 0;
			f->flags |= F_ERR;
			return iovcnt == 2 ? 0 : len-iov[0].iov_len;
		}
		rem -= cnt;
		if (cnt > iov[0].iov_len) {
			f->wpos = f->wbase = f->buf;
			cnt -= iov[0].iov_len;
			iov++; iovcnt--;
		} else if (iovcnt == 2) {
			f->wbase += cnt;
		}
		iov[0].iov_base = (char *)iov[0].iov_base + cnt;
		iov[0].iov_len -= cnt;
	}
}
#endif
