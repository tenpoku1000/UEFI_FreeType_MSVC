#ifndef _STDIO_IMPL_H
#define _STDIO_IMPL_H

#include <stdio.h>
#include "syscall.h"
#include "libc.h"
// @@@
#ifdef _MSC_VER
#include<efi.h>
#include<efilib.h>
#if defined(_DEBUG)
typedef void* PVOID;
typedef PVOID HANDLE;
#endif
#endif
// @@@

#define UNGET 8

#define FFINALLOCK(f) ((f)->lock>=0 ? __lockfile((f)) : 0)
#define FLOCK(f) int __need_unlock = ((f)->lock>=0 ? __lockfile((f)) : 0)
#define FUNLOCK(f) if (__need_unlock) __unlockfile((f)); else

#define F_PERM 1
#define F_NORD 4
#define F_NOWR 8
#define F_EOF 16
#define F_ERR 32
#define F_SVB 64
#define F_APP 128

struct _IO_FILE {
    unsigned flags;
    unsigned char *rpos, *rend;
    int (*close)(FILE *);
    unsigned char *wend, *wpos;
    unsigned char *mustbezero_1;
    unsigned char *wbase;
    size_t (*read)(FILE *, unsigned char *, size_t);
    size_t (*write)(FILE *, const unsigned char *, size_t);
    off_t (*seek)(FILE *, off_t, int);
    unsigned char *buf;
    size_t buf_size;
    FILE *prev, *next;
// @@@
#ifdef _MSC_VER
#if defined(_DEBUG)
    HANDLE win64_file;
#else
    EFI_FILE_IO_INTERFACE* efi_simple_file_system;
    EFI_FILE* efi_file_root;
    EFI_FILE* efi_file;
#endif
    CHAR16* file_name;
    off_t(*tell)(FILE*);
#endif
// @@@
    int fd;
    int pipe_pid;
    long lockcount;
    short dummy3;
    signed char mode;
    signed char lbf;
    int lock;
    int waiters;
    void *cookie;
    off_t off;
    char *getln_buf;
    void *mustbezero_2;
    unsigned char *shend;
    off_t shlim, shcnt;
    FILE *prev_locked, *next_locked;
};

size_t __stdio_read(FILE *, unsigned char *, size_t);
size_t __stdio_write(FILE *, const unsigned char *, size_t);
size_t __stdout_write(FILE *, const unsigned char *, size_t);
off_t __stdio_seek(FILE *, off_t, int);
// @@@
#ifdef _MSC_VER
off_t __stdio_tell(FILE*);
#endif
// @@@
int __stdio_close(FILE *);

size_t __string_read(FILE *, unsigned char *, size_t);

int __toread(FILE *);
int __towrite(FILE *);

#if defined(__PIC__) && (100*__GNUC__+__GNUC_MINOR__ >= 303)
__attribute__((visibility("protected")))
#endif
int __overflow(FILE *, int), __uflow(FILE *);

int __fseeko(FILE *, off_t, int);
int __fseeko_unlocked(FILE *, off_t, int);
off_t __ftello(FILE *);
off_t __ftello_unlocked(FILE *);
size_t __fwritex(const unsigned char *, size_t, FILE *);
int __putc_unlocked(int, FILE *);

// @@@
//FILE *__fdopen(int, const char *);
#ifdef _MSC_VER
FILE *__fdopen(const char __restrict*, const char __restrict*);
#else
FILE *__fdopen(int, const char *);
#endif
// @@@
int __fmodeflags(const char *);

#define OFLLOCK() LOCK(libc.ofl_lock)
#define OFLUNLOCK() UNLOCK(libc.ofl_lock)

#define feof(f) ((f)->flags & F_EOF)
#define ferror(f) ((f)->flags & F_ERR)

#define getc_unlocked(f) \
    ( ((f)->rpos < (f)->rend) ? *(f)->rpos++ : __uflow((f)) )

#define putc_unlocked(c, f) ( ((c)!=(f)->lbf && (f)->wpos<(f)->wend) \
    ? *(f)->wpos++ = (c) : __overflow((f),(c)) )

/* Caller-allocated FILE * operations */
FILE *__fopen_rb_ca(const char *, FILE *, unsigned char *, size_t);
int __fclose_ca(FILE *);

#endif
