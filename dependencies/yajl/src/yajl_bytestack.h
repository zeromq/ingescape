/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * A header only implementation of a simple stack of bytes, used in YAJL
 * to maintain parse state.
 */

#ifndef __YAJL_BYTESTACK_H__
#define __YAJL_BYTESTACK_H__

#include "api/yajl_common.h"

#define IGSYAJL_BS_INC 128

typedef struct igsyajl_bytestack_t
{
    unsigned char * stack;
    size_t size;
    size_t used;
    igsyajl_alloc_funcs * yaf;
} igsyajl_bytestack;

/* initialize a bytestack */
#define igsyajl_bs_init(obs, _yaf) {               \
        (obs).stack = NULL;                     \
        (obs).size = 0;                         \
        (obs).used = 0;                         \
        (obs).yaf = (_yaf);                     \
    }                                           \


/* initialize a bytestack */
#define igsyajl_bs_free(obs)                 \
    if ((obs).stack) (obs).yaf->free((obs).yaf->ctx, (obs).stack);

#define igsyajl_bs_current(obs)               \
    (assert((obs).used > 0), (obs).stack[(obs).used - 1])

#define igsyajl_bs_push(obs, byte) {                       \
    if (((obs).size - (obs).used) == 0) {               \
        (obs).size += IGSYAJL_BS_INC;                      \
        (obs).stack = (obs).yaf->realloc((obs).yaf->ctx,\
                                         (void *) (obs).stack, (obs).size);\
    }                                                   \
    (obs).stack[((obs).used)++] = (unsigned char)(byte);               \
}

/* removes the top item of the stack, returns nothing */
#define igsyajl_bs_pop(obs) { ((obs).used)--; }

#define igsyajl_bs_set(obs, byte)                          \
    (obs).stack[((obs).used) - 1] = (byte);


#endif
