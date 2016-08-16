#include <fcntl.h>
#include <string.h>
// @@@
#ifdef _MSC_VER
#include<efi.h>
#include<efilib.h>
#endif
// @@@

int __fmodeflags(const char *mode)
{
// @@@
#ifdef _MSC_VER
    int flags = 0;

    if (strchr(mode, '+')) flags = EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
    else if (*mode == 'r') flags = EFI_FILE_MODE_READ;
    else flags = EFI_FILE_MODE_WRITE;

    if (*mode != 'r') flags |= EFI_FILE_MODE_WRITE;

    return flags;
#else
// @@@
    int flags;
    if (strchr(mode, '+')) flags = O_RDWR;
    else if (*mode == 'r') flags = O_RDONLY;
    else flags = O_WRONLY;
    if (strchr(mode, 'x')) flags |= O_EXCL;
    if (strchr(mode, 'e')) flags |= O_CLOEXEC;
    if (*mode != 'r') flags |= O_CREAT;
    if (*mode == 'w') flags |= O_TRUNC;
    if (*mode == 'a') flags |= O_APPEND;
    return flags;
// @@@
#endif
// @@@
}
