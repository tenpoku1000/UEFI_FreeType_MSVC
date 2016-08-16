#include "libc.h"

// @@@
#ifdef _MSC_VER

static EFI_HANDLE IH = NULL;
static EFI_LOADED_IMAGE* LI = NULL;

void set_uefi_handle_if(EFI_HANDLE* image_handle, EFI_LOADED_IMAGE* loaded_image)
{
    IH = image_handle;
    LI = loaded_image;
}

EFI_HANDLE get_uefi_image_handle(void)
{
    return IH;
}

EFI_LOADED_IMAGE* get_uefi_loaded_image(void)
{
    return LI;
}

#endif
// @@@

#ifdef USE_LIBC_ACCESSOR
struct __libc *__libc_loc()
{
    static struct __libc __libc;
    return &__libc;
}
#else
struct __libc __libc;
#endif

#ifdef BROKEN_VISIBILITY
__asm__(".hidden __libc");
#endif

size_t __hwcap;
size_t __sysinfo;
char *__progname=0, *__progname_full=0;

// @@@
//weak_alias(__progname, program_invocation_short_name);
//weak_alias(__progname_full, program_invocation_name);
#ifdef _MSC_VER
int _fltused = 1;
#else
weak_alias(__progname, program_invocation_short_name);
weak_alias(__progname_full, program_invocation_name);
#endif
// @@@
