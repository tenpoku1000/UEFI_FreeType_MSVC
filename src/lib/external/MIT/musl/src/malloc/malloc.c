#define _GNU_SOURCE
/* @@@ */
#ifndef _MSC_VER
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include "libc.h"
#include "atomic.h"
#include "pthread_impl.h"
#else
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <efi.h>
#include <efilib.h>
#if defined(_DEBUG)
#include "win64env.h"
#endif
#endif

#if defined(__GNUC__) && defined(__PIC__)
#define inline inline __attribute__((always_inline))
#endif

/* @@@ */
#ifdef _MSC_VER
/**

Copyright (c) 2006 - 2014, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/**

UDK2014.SP1.Complete.MyWorkSpace\UDK2014.SP1.MyWorkSpace\MyWorkSpace\MdePkg\Include\Base.h

**/

/**

  Macro that returns a pointer to the data structure that contains a specified field of
  that data structure.  This is a lightweight method to hide information by placing a
  public data structure inside a larger private data structure and using a pointer to
  the public data structure to retrieve a pointer to the private data structure.

  This function computes the offset, in bytes, of field specified by Field from the beginning
  of the  data structure specified by TYPE.  This offset is subtracted from Record, and is
  used to return a pointer to a data structure of the type specified by TYPE. If the data type
  specified by TYPE does not contain the field specified by Field, then the module will not compile.

  @param   Record   Pointer to the field specified by Field within a data structure of type TYPE.
  @param   TYPE     The name of the data structure type to return.  This data structure must
                    contain the field specified by Field.
  @param   Field    The name of the field in the data structure specified by TYPE to which Record points.

  @return  A pointer to the structure from one of it's elements.

**/
#define BASE_CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))

/**
Returns a 16-bit signature built from 2 ASCII characters.

This macro returns a 16-bit value built from the two ASCII characters specified
by A and B.

@param  A    The first ASCII character.
@param  B    The second ASCII character.

@return A 16-bit value built from the two ASCII characters specified by A and B.

**/
#define SIGNATURE_16(A, B)        ((A) | (B << 8))

/**
Returns a 32-bit signature built from 4 ASCII characters.

This macro returns a 32-bit value built from the four ASCII characters specified
by A, B, C, and D.

@param  A    The first ASCII character.
@param  B    The second ASCII character.
@param  C    The third ASCII character.
@param  D    The fourth ASCII character.

@return A 32-bit value built from the two ASCII characters specified by A, B,
C and D.

**/
#define SIGNATURE_32(A, B, C, D)  (SIGNATURE_16 (A, B) | (SIGNATURE_16 (C, D) << 16))

/**

UDK2014.SP1.Complete.MyWorkSpace\UDK2014.SP1.MyWorkSpace\MyWorkSpace\MdeModulePkg\Core\Dxe\Mem

**/

#define POOL_HEAD_SIGNATURE   SIGNATURE_32('p','h','d','0')
typedef struct {
  UINT32          Signature;
  UINT32          Reserved;
  EFI_MEMORY_TYPE Type;
  UINTN           Size;
  CHAR8           Data[1];
} POOL_HEAD;

void* malloc(size_t buffer_size)
{
	if (0 == buffer_size){

		errno = EINVAL;

		return NULL;
	}

	void* buffer = NULL;

#if !defined(_DEBUG)
	EFI_STATUS status = BS->AllocatePool(EfiLoaderData, (UINTN)buffer_size, &buffer);

	if (EFI_ERROR(status)){

		errno = ENOMEM;

		return NULL;
	}
#else
	buffer = heap_alloc(buffer_size);

	if (NULL == buffer){

		errno = ENOMEM;
	}
#endif

	return buffer;
}

void* realloc(void* buffer, size_t new_size)
{
	void* new_buffer = NULL;
	size_t old_size = 0;
	size_t copy_size = 0;

	if (buffer){

		if (0 == new_size){

			free(buffer);

			return NULL;
		}

#if !defined(_DEBUG)
		POOL_HEAD* head = BASE_CR(buffer, POOL_HEAD, Data);

		if (head){

			if (head->Signature != POOL_HEAD_SIGNATURE) {

				errno = ENOMEM;

				return NULL;
			}

			old_size = head->Size;

		}else{

			errno = ENOMEM;

			return NULL;
		}

		new_buffer = malloc(new_size);

		if (new_buffer){

			copy_size = old_size;

			if (old_size > new_size){

				copy_size = new_size;
			}

			memcpy(new_buffer, buffer, copy_size);

			free(buffer);
		}
#else
		new_buffer = heap_realloc(buffer, new_size);

		if (NULL == new_buffer){

			errno = ENOMEM;
		}
#endif

	}else{

		return malloc(new_size);
	}

	return new_buffer;
}

void free(void* buffer)
{
	if (NULL == buffer){

		return;
	}

#if !defined(_DEBUG)
	(void)BS->FreePool(buffer);
#else
	void heap_free(buffer);
#endif
}

#else

uintptr_t __brk(uintptr_t);
void *__mmap(void *, size_t, int, int, int, off_t);
int __munmap(void *, size_t);
void *__mremap(void *, size_t, size_t, int, ...);
int __madvise(void *, size_t, int);

struct chunk {
	size_t psize, csize;
	struct chunk *next, *prev;
};

struct bin {
	int lock[2];
	struct chunk *head;
	struct chunk *tail;
};

static struct {
	uintptr_t brk;
	size_t *heap;
	uint64_t binmap;
	struct bin bins[64];
	int brk_lock[2];
	int free_lock[2];
	unsigned mmap_step;
} mal;


#define SIZE_ALIGN (4*sizeof(size_t))
#define SIZE_MASK (-SIZE_ALIGN)
#define OVERHEAD (2*sizeof(size_t))
#define MMAP_THRESHOLD (0x1c00*SIZE_ALIGN)
#define DONTCARE 16
#define RECLAIM 163840

#define CHUNK_SIZE(c) ((c)->csize & -2)
#define CHUNK_PSIZE(c) ((c)->psize & -2)
#define PREV_CHUNK(c) ((struct chunk *)((char *)(c) - CHUNK_PSIZE(c)))
#define NEXT_CHUNK(c) ((struct chunk *)((char *)(c) + CHUNK_SIZE(c)))
#define MEM_TO_CHUNK(p) (struct chunk *)((char *)(p) - OVERHEAD)
#define CHUNK_TO_MEM(c) (void *)((char *)(c) + OVERHEAD)
#define BIN_TO_CHUNK(i) (MEM_TO_CHUNK(&mal.bins[i].head))

#define C_INUSE  ((size_t)1)

#define IS_MMAPPED(c) !((c)->csize & (C_INUSE))


/* Synchronization tools */

static inline void lock(volatile int *lk)
{
	if (libc.threads_minus_1)
		while(a_swap(lk, 1)) __wait(lk, lk+1, 1, 1);
}

static inline void unlock(volatile int *lk)
{
	if (lk[0]) {
		a_store(lk, 0);
		if (lk[1]) __wake(lk, 1, 1);
	}
}

static inline void lock_bin(int i)
{
	lock(mal.bins[i].lock);
	if (!mal.bins[i].head)
		mal.bins[i].head = mal.bins[i].tail = BIN_TO_CHUNK(i);
}

static inline void unlock_bin(int i)
{
	unlock(mal.bins[i].lock);
}

static int first_set(uint64_t x)
{
#if 1
	return a_ctz_64(x);
#else
	static const char debruijn64[64] = {
		0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28,
		62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11,
		63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
		51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
	};
	static const char debruijn32[32] = {
		0, 1, 23, 2, 29, 24, 19, 3, 30, 27, 25, 11, 20, 8, 4, 13,
		31, 22, 28, 18, 26, 10, 7, 12, 21, 17, 9, 6, 16, 5, 15, 14
	};
	if (sizeof(long) < 8) {
		uint32_t y = x;
		if (!y) {
			y = x>>32;
			return 32 + debruijn32[(y&-y)*0x076be629 >> 27];
		}
		return debruijn32[(y&-y)*0x076be629 >> 27];
	}
	return debruijn64[(x&-x)*0x022fdd63cc95386dull >> 58];
#endif
}

static int bin_index(size_t x)
{
	x = x / SIZE_ALIGN - 1;
	if (x <= 32) return x;
	if (x > 0x1c00) return 63;
	return ((union { float v; uint32_t r; }){(int)x}.r>>21) - 496;
}

static int bin_index_up(size_t x)
{
	x = x / SIZE_ALIGN - 1;
	if (x <= 32) return x;
	return ((union { float v; uint32_t r; }){(int)x}.r+0x1fffff>>21) - 496;
}

#if 0
void __dump_heap(int x)
{
	struct chunk *c;
	int i;
	for (c = (void *)mal.heap; CHUNK_SIZE(c); c = NEXT_CHUNK(c))
		fprintf(stderr, "base %p size %zu (%d) flags %d/%d\n",
			c, CHUNK_SIZE(c), bin_index(CHUNK_SIZE(c)),
			c->csize & 15,
			NEXT_CHUNK(c)->psize & 15);
	for (i=0; i<64; i++) {
		if (mal.bins[i].head != BIN_TO_CHUNK(i) && mal.bins[i].head) {
			fprintf(stderr, "bin %d: %p\n", i, mal.bins[i].head);
			if (!(mal.binmap & 1ULL<<i))
				fprintf(stderr, "missing from binmap!\n");
		} else if (mal.binmap & 1ULL<<i)
			fprintf(stderr, "binmap wrongly contains %d!\n", i);
	}
}
#endif

static struct chunk *expand_heap(size_t n)
{
	struct chunk *w;
	uintptr_t new;

	lock(mal.brk_lock);

	if (n > SIZE_MAX - mal.brk - 2*PAGE_SIZE) goto fail;
	new = mal.brk + n + SIZE_ALIGN + PAGE_SIZE - 1 & -PAGE_SIZE;
	n = new - mal.brk;

	if (__brk(new) != new) {
		size_t min = (size_t)PAGE_SIZE << mal.mmap_step/2;
		n += -n & PAGE_SIZE-1;
		if (n < min) n = min;
		void *area = __mmap(0, n, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if (area == MAP_FAILED) goto fail;

		mal.mmap_step++;
		area = (char *)area + SIZE_ALIGN - OVERHEAD;
		w = area;
		n -= SIZE_ALIGN;
		w->psize = 0 | C_INUSE;
		w->csize = n | C_INUSE;
		w = NEXT_CHUNK(w);
		w->psize = n | C_INUSE;
		w->csize = 0 | C_INUSE;

		unlock(mal.brk_lock);

		return area;
	}

	w = MEM_TO_CHUNK(new);
	w->psize = n | C_INUSE;
	w->csize = 0 | C_INUSE;

	w = MEM_TO_CHUNK(mal.brk);
	w->csize = n | C_INUSE;
	mal.brk = new;
	
	unlock(mal.brk_lock);

	return w;
fail:
	unlock(mal.brk_lock);
	errno = ENOMEM;
	return 0;
}

static int init_malloc(size_t n)
{
	static int init, waiters;
	int state;
	struct chunk *c;

	if (init == 2) return 0;

	while ((state=a_swap(&init, 1)) == 1)
		__wait(&init, &waiters, 1, 1);
	if (state) {
		a_store(&init, 2);
		return 0;
	}

	mal.brk = __brk(0);
#ifdef SHARED
	mal.brk = mal.brk + PAGE_SIZE-1 & -PAGE_SIZE;
#endif
	mal.brk = mal.brk + 2*SIZE_ALIGN-1 & -SIZE_ALIGN;

	c = expand_heap(n);

	if (!c) {
		a_store(&init, 0);
		if (waiters) __wake(&init, 1, 1);
		return -1;
	}

	mal.heap = (void *)c;
	c->psize = 0 | C_INUSE;
	free(CHUNK_TO_MEM(c));

	a_store(&init, 2);
	if (waiters) __wake(&init, -1, 1);
	return 1;
}

static int adjust_size(size_t *n)
{
	/* Result of pointer difference must fit in ptrdiff_t. */
	if (*n-1 > PTRDIFF_MAX - SIZE_ALIGN - PAGE_SIZE) {
		if (*n) {
			errno = ENOMEM;
			return -1;
		} else {
			*n = SIZE_ALIGN;
			return 0;
		}
	}
	*n = (*n + OVERHEAD + SIZE_ALIGN - 1) & SIZE_MASK;
	return 0;
}

static void unbin(struct chunk *c, int i)
{
	if (c->prev == c->next)
		a_and_64(&mal.binmap, ~(1ULL<<i));
	c->prev->next = c->next;
	c->next->prev = c->prev;
	c->csize |= C_INUSE;
	NEXT_CHUNK(c)->psize |= C_INUSE;
}

static int alloc_fwd(struct chunk *c)
{
	int i;
	size_t k;
	while (!((k=c->csize) & C_INUSE)) {
		i = bin_index(k);
		lock_bin(i);
		if (c->csize == k) {
			unbin(c, i);
			unlock_bin(i);
			return 1;
		}
		unlock_bin(i);
	}
	return 0;
}

static int alloc_rev(struct chunk *c)
{
	int i;
	size_t k;
	while (!((k=c->psize) & C_INUSE)) {
		i = bin_index(k);
		lock_bin(i);
		if (c->psize == k) {
			unbin(PREV_CHUNK(c), i);
			unlock_bin(i);
			return 1;
		}
		unlock_bin(i);
	}
	return 0;
}


/* pretrim - trims a chunk _prior_ to removing it from its bin.
 * Must be called with i as the ideal bin for size n, j the bin
 * for the _free_ chunk self, and bin j locked. */
static int pretrim(struct chunk *self, size_t n, int i, int j)
{
	size_t n1;
	struct chunk *next, *split;

	/* We cannot pretrim if it would require re-binning. */
	if (j < 40) return 0;
	if (j < i+3) {
		if (j != 63) return 0;
		n1 = CHUNK_SIZE(self);
		if (n1-n <= MMAP_THRESHOLD) return 0;
	} else {
		n1 = CHUNK_SIZE(self);
	}
	if (bin_index(n1-n) != j) return 0;

	next = NEXT_CHUNK(self);
	split = (void *)((char *)self + n);

	split->prev = self->prev;
	split->next = self->next;
	split->prev->next = split;
	split->next->prev = split;
	split->psize = n | C_INUSE;
	split->csize = n1-n;
	next->psize = n1-n;
	self->csize = n | C_INUSE;
	return 1;
}

static void trim(struct chunk *self, size_t n)
{
	size_t n1 = CHUNK_SIZE(self);
	struct chunk *next, *split;

	if (n >= n1 - DONTCARE) return;

	next = NEXT_CHUNK(self);
	split = (void *)((char *)self + n);

	split->psize = n | C_INUSE;
	split->csize = n1-n | C_INUSE;
	next->psize = n1-n | C_INUSE;
	self->csize = n | C_INUSE;

	free(CHUNK_TO_MEM(split));
}

void *malloc(size_t n)
{
	struct chunk *c;
	int i, j;

	if (adjust_size(&n) < 0) return 0;

	if (n > MMAP_THRESHOLD) {
		size_t len = n + OVERHEAD + PAGE_SIZE - 1 & -PAGE_SIZE;
		char *base = __mmap(0, len, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if (base == (void *)-1) return 0;
		c = (void *)(base + SIZE_ALIGN - OVERHEAD);
		c->csize = len - (SIZE_ALIGN - OVERHEAD);
		c->psize = SIZE_ALIGN - OVERHEAD;
		return CHUNK_TO_MEM(c);
	}

	i = bin_index_up(n);
	for (;;) {
		uint64_t mask = mal.binmap & -(1ULL<<i);
		if (!mask) {
			if (init_malloc(n) > 0) continue;
			c = expand_heap(n);
			if (!c) return 0;
			if (alloc_rev(c)) {
				struct chunk *x = c;
				c = PREV_CHUNK(c);
				NEXT_CHUNK(x)->psize = c->csize =
					x->csize + CHUNK_SIZE(c);
			}
			break;
		}
		j = first_set(mask);
		lock_bin(j);
		c = mal.bins[j].head;
		if (c != BIN_TO_CHUNK(j) && j == bin_index(c->csize)) {
			if (!pretrim(c, n, i, j)) unbin(c, j);
			unlock_bin(j);
			break;
		}
		unlock_bin(j);
	}

	/* Now patch up in case we over-allocated */
	trim(c, n);

	return CHUNK_TO_MEM(c);
}

void *realloc(void *p, size_t n)
{
	struct chunk *self, *next;
	size_t n0, n1;
	void *new;

	if (!p) return malloc(n);

	if (adjust_size(&n) < 0) return 0;

	self = MEM_TO_CHUNK(p);
	n1 = n0 = CHUNK_SIZE(self);

	if (IS_MMAPPED(self)) {
		size_t extra = self->psize;
		char *base = (char *)self - extra;
		size_t oldlen = n0 + extra;
		size_t newlen = n + extra;
		/* Crash on realloc of freed chunk */
		if (extra & 1) a_crash();
		if (newlen < PAGE_SIZE && (new = malloc(n))) {
			memcpy(new, p, n-OVERHEAD);
			free(p);
			return new;
		}
		newlen = (newlen + PAGE_SIZE-1) & -PAGE_SIZE;
		if (oldlen == newlen) return p;
		base = __mremap(base, oldlen, newlen, MREMAP_MAYMOVE);
		if (base == (void *)-1)
			return newlen < oldlen ? p : 0;
		self = (void *)(base + extra);
		self->csize = newlen - extra;
		return CHUNK_TO_MEM(self);
	}

	next = NEXT_CHUNK(self);

	/* Crash on corrupted footer (likely from buffer overflow) */
	if (next->psize != self->csize) a_crash();

	/* Merge adjacent chunks if we need more space. This is not
	 * a waste of time even if we fail to get enough space, because our
	 * subsequent call to free would otherwise have to do the merge. */
	if (n > n1 && alloc_fwd(next)) {
		n1 += CHUNK_SIZE(next);
		next = NEXT_CHUNK(next);
	}
	/* FIXME: find what's wrong here and reenable it..? */
	if (0 && n > n1 && alloc_rev(self)) {
		self = PREV_CHUNK(self);
		n1 += CHUNK_SIZE(self);
	}
	self->csize = n1 | C_INUSE;
	next->psize = n1 | C_INUSE;

	/* If we got enough space, split off the excess and return */
	if (n <= n1) {
		//memmove(CHUNK_TO_MEM(self), p, n0-OVERHEAD);
		trim(self, n);
		return CHUNK_TO_MEM(self);
	}

	/* As a last resort, allocate a new chunk and copy to it. */
	new = malloc(n-OVERHEAD);
	if (!new) return 0;
	memcpy(new, p, n0-OVERHEAD);
	free(CHUNK_TO_MEM(self));
	return new;
}

void free(void *p)
{
	struct chunk *self = MEM_TO_CHUNK(p);
	struct chunk *next;
	size_t final_size, new_size, size;
	int reclaim=0;
	int i;

	if (!p) return;

	if (IS_MMAPPED(self)) {
		size_t extra = self->psize;
		char *base = (char *)self - extra;
		size_t len = CHUNK_SIZE(self) + extra;
		/* Crash on double free */
		if (extra & 1) a_crash();
		__munmap(base, len);
		return;
	}

	final_size = new_size = CHUNK_SIZE(self);
	next = NEXT_CHUNK(self);

	/* Crash on corrupted footer (likely from buffer overflow) */
	if (next->psize != self->csize) a_crash();

	for (;;) {
		/* Replace middle of large chunks with fresh zero pages */
		if (reclaim && (self->psize & next->csize & C_INUSE)) {
			uintptr_t a = (uintptr_t)self + SIZE_ALIGN+PAGE_SIZE-1 & -PAGE_SIZE;
			uintptr_t b = (uintptr_t)next - SIZE_ALIGN & -PAGE_SIZE;
#if 1
			__madvise((void *)a, b-a, MADV_DONTNEED);
#else
			__mmap((void *)a, b-a, PROT_READ|PROT_WRITE,
				MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0);
#endif
		}

		if (self->psize & next->csize & C_INUSE) {
			self->csize = final_size | C_INUSE;
			next->psize = final_size | C_INUSE;
			i = bin_index(final_size);
			lock_bin(i);
			lock(mal.free_lock);
			if (self->psize & next->csize & C_INUSE)
				break;
			unlock(mal.free_lock);
			unlock_bin(i);
		}

		if (alloc_rev(self)) {
			self = PREV_CHUNK(self);
			size = CHUNK_SIZE(self);
			final_size += size;
			if (new_size+size > RECLAIM && (new_size+size^size) > size)
				reclaim = 1;
		}

		if (alloc_fwd(next)) {
			size = CHUNK_SIZE(next);
			final_size += size;
			if (new_size+size > RECLAIM && (new_size+size^size) > size)
				reclaim = 1;
			next = NEXT_CHUNK(next);
		}
	}

	self->csize = final_size;
	next->psize = final_size;
	unlock(mal.free_lock);

	self->next = BIN_TO_CHUNK(i);
	self->prev = mal.bins[i].tail;
	self->next->prev = self;
	self->prev->next = self;

	if (!(mal.binmap & 1ULL<<i))
		a_or_64(&mal.binmap, 1ULL<<i);

	unlock_bin(i);
}

#endif

