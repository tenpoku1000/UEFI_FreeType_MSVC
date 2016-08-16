#include "pthread_impl.h"

int *__errno_location(void)
{
    static int e;
// @@@
//    if (libc.has_thread_pointer) return &__pthread_self()->errno_val;
#ifndef _MSC_VER
    if (libc.has_thread_pointer) return &__pthread_self()->errno_val;
#endif
// @@@
    return &e;
}
