#include "stdio_impl.h"
// @@@
#ifdef _MSC_VER
#include<errno.h>
#include<efi.h>
#include<efilib.h>
#if defined(_DEBUG)
#include "win64env.h"
#endif

int __stdio_close(FILE* f)
{
#if !defined(_DEBUG)
    if ((NULL == f) || (NULL == f->efi_file)){

        errno = EBADF;

        return EOF;
    }

    EFI_FILE* efi_file = f->efi_file;

    EFI_STATUS status = efi_file->Close(efi_file);

    if (EFI_ERROR(status)){

        errno = EBADF;

        f->efi_file = NULL;

        free(f->file_name);
        f->file_name = NULL;

        (void)f->efi_file_root->Close(f->efi_file_root);
        f->efi_file_root = NULL;
        f->efi_simple_file_system = NULL;

        return EOF;
    }

    f->efi_file = NULL;

    free(f->file_name);
    f->file_name = NULL;

    (void)f->efi_file_root->Close(f->efi_file_root);
    f->efi_file_root = NULL;
    f->efi_simple_file_system = NULL;
#else
    if (NULL == f){

        errno = EBADF;

        return EOF;
    }

    close_handle(f->win64_file);
    f->win64_file = NULL;
#endif

    return 0;
}
#else
// @@@

int __stdio_close(FILE *f)
{
    return syscall(SYS_close, f->fd);
}
// @@@
#endif
// @@@
