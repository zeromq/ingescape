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

/**
 * \file igsyajl_alloc.h
 * default memory allocation routines for yajl which use malloc/realloc and
 * free
 */

#include "yajl_alloc.h"
#include <stdlib.h>

static void * igsyajl_internal_malloc(void *ctx, size_t sz)
{
    (void)ctx;
    return malloc(sz);
}

static void * igsyajl_internal_realloc(void *ctx, void * previous,
                                    size_t sz)
{
    (void)ctx;
    return realloc(previous, sz);
}

static void igsyajl_internal_free(void *ctx, void * ptr)
{
    (void)ctx;
    free(ptr);
}

void igsyajl_set_default_alloc_funcs(igsyajl_alloc_funcs * yaf)
{
    yaf->malloc = igsyajl_internal_malloc;
    yaf->free = igsyajl_internal_free;
    yaf->realloc = igsyajl_internal_realloc;
    yaf->ctx = NULL;
}

