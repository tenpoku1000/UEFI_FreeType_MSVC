#include <stdio_impl.h>

int __toread(FILE *f)
{
	f->mode |= f->mode-1;
	if (f->wpos > f->buf) f->write(f, 0, 0);
	f->wpos = f->wbase = f->wend = 0;
	if (f->flags & (F_EOF|F_NORD)) {
		if (f->flags & F_NORD) f->flags |= F_ERR;
		return EOF;
	}
	f->rpos = f->rend = f->buf;
	return 0;
}

//@@@
//void __stdio_exit_needed(void);
//
//void __toread_needs_stdio_exit()
//{
//	__stdio_exit_needed();
//}
//@@@
