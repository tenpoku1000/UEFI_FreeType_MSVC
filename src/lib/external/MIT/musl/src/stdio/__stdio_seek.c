#include "stdio_impl.h"
// @@@
#ifdef _MSC_VER
#include<errno.h>
#include<efi.h>
#include<efilib.h>
#if defined(_DEBUG)
#include "win64env.h"
#endif

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
#if !defined(_DEBUG)
    if ((NULL == f) || (NULL == f->efi_file) ||
        ((SEEK_SET != whence) && (SEEK_CUR != whence) && (SEEK_END != whence))){

        errno = EINVAL;

        return -1;
    }

    EFI_FILE* efi_file = f->efi_file;

    EFI_STATUS status = EFI_SUCCESS;

    switch (whence){
    case SEEK_SET:
        status = efi_file->SetPosition(efi_file, 0);
        break;
    case SEEK_END:
        status = efi_file->SetPosition(efi_file, 0xFFFFFFFFFFFFFFFF);
        break;
    default:
        status = efi_file->SetPosition(efi_file, off);
        break;
    }

    if (EFI_ERROR(status)){

        errno = EBADF;

        return -1;
    }
#else
    if ((NULL == f) || (NULL == f->win64_file) ||
        ((SEEK_SET != whence) && (SEEK_CUR != whence) && (SEEK_END != whence))){

        errno = EINVAL;

        return -1;
    }

    int status = set_file_pointer_ex_seek(f->win64_file, off, whence);

    if (FALSE == status){

        errno = EBADF;

        return -1;
    }
#endif

    return 0;
}

off_t __stdio_tell(FILE* f)
{
#if !defined(_DEBUG)
    if ((NULL == f) || (NULL == f->efi_file)){

        errno = EINVAL;

        return -1;
    }

    EFI_FILE_HANDLE efi_file = f->efi_file;

    UINT64 pos = 0;
    EFI_STATUS status = efi_file->GetPosition(f->efi_file, &pos);

    if (EFI_ERROR(status)){

        errno = EBADF;

        return -1;
    }
#else
    if ((NULL == f) || (NULL == f->win64_file)){

        errno = EINVAL;

        return -1;
    }

    UINT64 pos = { 0 };

    int status = set_file_pointer_ex_tell(f->win64_file, &pos);

    if (FALSE == status){

        errno = EBADF;

        return -1;
    }
#endif

    return pos;
}
#else
// @@@

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
    off_t ret;
#ifdef SYS__llseek
    if (syscall(SYS__llseek, f->fd, off>>32, off, &ret, whence)<0)
        ret = -1;
#else
    ret = syscall(SYS_lseek, f->fd, off, whence);
#endif
    return ret;
}
// @@@
#endif
// @@@
