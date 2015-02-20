#include "stdio_impl.h"
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
/* @@@ */
#ifdef _MSC_VER
#include <errno.h>
#include <efi.h>
#include <efilib.h>
#if defined(_DEBUG)
#include "win64env.h"
#endif

static CHAR16* to_char16(const unsigned char* filename)
{
	size_t len = strlen((const char*)filename);

	if (0 == len) return NULL;

	CHAR16* p = (CHAR16*)calloc(len + 1, sizeof(CHAR16));

	if (NULL == p) return NULL;

	for (size_t i = 0; len > i; ++i) p[i] = filename[i];

	return p;
}

FILE *__fdopen(const char __restrict* filename, const char __restrict* mode)
{
	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)){

		errno = EINVAL;

		return 0;
	}

	/* Compute the flags */
	int flags = __fmodeflags(mode);

	FILE* f = NULL;

	/* Allocate FILE+buffer or fail */
	if (!(f = malloc(sizeof *f + UNGET + BUFSIZ))) return 0;

	/* Zero-fill only the struct, not the buffer */
	memset(f, 0, sizeof *f);

	/* Initialize op ptrs. No problem if some are unneeded. */
	f->read = __stdio_read;
	f->write = __stdio_write;
	f->seek = __stdio_seek;
	f->tell = __stdio_tell;
	f->close = __stdio_close;

#if !defined(_DEBUG)
	EFI_STATUS status = BS->LocateProtocol(
		&FileSystemProtocol,
		NULL,
		&(f->efi_simple_file_system)
	);

	if (EFI_ERROR(status)){

		errno = EACCES;

		free(f);

		return NULL;
	}

	status = f->efi_simple_file_system->OpenVolume(
		f->efi_simple_file_system, &(f->efi_file_root)
	);

	if (EFI_ERROR(status)){

		errno = EACCES;

		free(f);

		return NULL;
	}

	UINT64 efi_mode = 0;

	if (EFI_FILE_MODE_READ & flags) efi_mode |= EFI_FILE_MODE_READ;
	if (EFI_FILE_MODE_WRITE & flags) efi_mode |= EFI_FILE_MODE_WRITE;
	if (EFI_FILE_MODE_CREATE & flags) efi_mode |= EFI_FILE_MODE_CREATE;

	f->file_name = to_char16((const unsigned char*)filename);

	if (NULL == f->file_name){

		free(f);

		return NULL;
	}

	status = f->efi_file_root->Open(
		f->efi_file_root, &(f->efi_file), f->file_name,
		efi_mode, EFI_FILE_ARCHIVE
	);

	if (EFI_ERROR(status)){

		errno = EACCES;

		free(f->file_name);
		free(f);

		return NULL;
	}
#else

	f->file_name = to_char16((const unsigned char*)filename);

	if (NULL == f->file_name){

		free(f);

		return NULL;
	}

	f->win64_file = create_file(
		f->file_name, flags,
		EFI_FILE_MODE_READ, EFI_FILE_MODE_WRITE, EFI_FILE_MODE_CREATE
	);

	if (NULL == f->win64_file){

		errno = EACCES;

		free(f->file_name);
		free(f);

		return NULL;
	}
#endif

	return f;
}
#else
FILE *__fdopen(int fd, const char *mode)
{
	FILE *f;
	struct termios tio;

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)) {
		errno = EINVAL;
		return 0;
	}

	/* Allocate FILE+buffer or fail */
	if (!(f=malloc(sizeof *f + UNGET + BUFSIZ))) return 0;

	/* Zero-fill only the struct, not the buffer */
	memset(f, 0, sizeof *f);

	/* Impose mode restrictions */
	if (!strchr(mode, '+')) f->flags = (*mode == 'r') ? F_NOWR : F_NORD;

	/* Apply close-on-exec flag */
	if (strchr(mode, 'e')) __syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC);

	/* Set append mode on fd if opened for append */
	if (*mode == 'a') {
		int flags = __syscall(SYS_fcntl, fd, F_GETFL);
		if (!(flags & O_APPEND))
			__syscall(SYS_fcntl, fd, F_SETFL, flags | O_APPEND);
		f->flags |= F_APP;
	}

	f->fd = fd;
	f->buf = (unsigned char *)f + sizeof *f + UNGET;
	f->buf_size = BUFSIZ;

	/* Activate line buffered mode for terminals */
	f->lbf = EOF;
	if (!(f->flags & F_NOWR) && !__syscall(SYS_ioctl, fd, TCGETS, &tio))
		f->lbf = '\n';

	/* Initialize op ptrs. No problem if some are unneeded. */
	f->read = __stdio_read;
	f->write = __stdio_write;
	f->seek = __stdio_seek;
	f->close = __stdio_close;

	if (!libc.threaded) f->lock = -1;

	/* Add new FILE to open file list */
	OFLLOCK();
	f->next = libc.ofl_head;
	if (libc.ofl_head) libc.ofl_head->prev = f;
	libc.ofl_head = f;
	OFLUNLOCK();

	return f;
}

weak_alias(__fdopen, fdopen);
#endif
