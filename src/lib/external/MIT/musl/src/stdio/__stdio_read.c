#include "stdio_impl.h"
#include <sys/uio.h>
#include <pthread.h>
// @@@
#ifdef _MSC_VER
#include<errno.h>
#include<efi.h>
#include<efilib.h>
#if defined(_DEBUG)
#include "win64env.h"
#endif

size_t __stdio_read(FILE *f, unsigned char *buf, size_t len)
{
#if !defined(_DEBUG)
    if ((NULL == f) || (NULL == f->efi_file)){

        errno = EINVAL;

        return -1;
    }

    if ((0 == len) || (NULL == buf)){

        return 0;
    }

    EFI_FILE* efi_file = f->efi_file;

    size_t length = len;

    EFI_STATUS status = efi_file->Read(efi_file, &len, buf);

    if (EFI_ERROR(status)){

        if (EFI_BUFFER_TOO_SMALL == status){

            len = length;
        }

        f->flags |= F_ERR;

        return len;
    }
#else
    if ((NULL == f) || (NULL == f->win64_file)){

        errno = EINVAL;

        return -1;
    }

    if ((0 == len) || (NULL == buf)){

        return 0;
    }

    size_t length = len;

    int status = read_file(f->win64_file, buf, &len);

    if (FALSE == status){

        f->flags |= F_ERR;

        return len;
    }
#endif

    if (length != len){

        f->flags |= F_EOF;
    }

    return len;
}
#else
// @@@
static void cleanup(void *p)
{
    FILE *f = p;
    if (!f->lockcount) __unlockfile(f);
}

size_t __stdio_read(FILE *f, unsigned char *buf, size_t len)
{
    struct iovec iov[2] = {
        { .iov_base = buf, .iov_len = len - !!f->buf_size },
        { .iov_base = f->buf, .iov_len = f->buf_size }
    };
    ssize_t cnt;

    pthread_cleanup_push(cleanup, f);
    cnt = syscall_cp(SYS_readv, f->fd, iov, 2);
    pthread_cleanup_pop(0);
    if (cnt <= 0) {
        f->flags |= F_EOF ^ ((F_ERR^F_EOF) & cnt);
        f->rpos = f->rend = 0;
        return cnt;
    }
    if (cnt <= iov[0].iov_len) return cnt;
    cnt -= iov[0].iov_len;
    f->rpos = f->buf;
    f->rend = f->buf + cnt;
    if (f->buf_size) buf[len-1] = *f->rpos++;
    return len;
}
// @@@
#endif
// @@@
