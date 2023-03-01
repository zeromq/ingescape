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

#ifndef __YAJL_COMMON_H__
#define __YAJL_COMMON_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IGSYAJL_MAX_DEPTH 128

/* msft dll export gunk.  To build a DLL on windows, you
 * must define WIN32, IGSYAJL_SHARED, and IGSYAJL_BUILD.  To use a shared
 * DLL, you must define IGSYAJL_SHARED and WIN32 */
#if (defined(_WIN32) || defined(WIN32)) && defined(IGSYAJL_SHARED)
#  ifdef IGSYAJL_BUILD
#    define IGSYAJL_API __declspec(dllexport)
#  else
#    define IGSYAJL_API __declspec(dllimport)
#  endif
#else
#  if defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
#    define IGSYAJL_API __attribute__ ((visibility("default")))
#  else
#    define IGSYAJL_API
#  endif
#endif

/** pointer to a malloc function, supporting client overriding memory
 *  allocation routines */
typedef void * (*igsyajl_malloc_func)(void *ctx, size_t sz);

/** pointer to a free function, supporting client overriding memory
 *  allocation routines */
typedef void (*igsyajl_free_func)(void *ctx, void * ptr);

/** pointer to a realloc function which can resize an allocation. */
typedef void * (*igsyajl_realloc_func)(void *ctx, void * ptr, size_t sz);

/** A structure which can be passed to igsyajl_*_alloc routines to allow the
 *  client to specify memory allocation functions to be used. */
typedef struct
{
    /** pointer to a function that can allocate uninitialized memory */
    igsyajl_malloc_func malloc;
    /** pointer to a function that can resize memory allocations */
    igsyajl_realloc_func realloc;
    /** pointer to a function that can free memory allocated using
     *  reallocFunction or mallocFunction */
    igsyajl_free_func free;
    /** a context pointer that will be passed to above allocation routines */
    void * ctx;
} igsyajl_alloc_funcs;

/** Locale insensitive variants of snprintf and strtod to always use a point
 *  as decimal separator. */
int igsyajl_snprintf(char *s, size_t n, const char *format, ...);
double igsyajl_strtod (const char* str, char** endptr);

#ifdef __cplusplus
}
#endif

#endif
