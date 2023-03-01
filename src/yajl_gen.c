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

#include "yajl_gen.h"
#include "yajl_buf.h"
#include "yajl_encode.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

typedef enum {
    igsyajl_gen_start,
    igsyajl_gen_map_start,
    igsyajl_gen_map_key,
    igsyajl_gen_map_val,
    igsyajl_gen_array_start,
    igsyajl_gen_in_array,
    igsyajl_gen_complete,
    igsyajl_gen_error
} igsyajl_gen_state;

struct igsyajl_gen
{
    unsigned int flags;
    unsigned int depth;
    const char * indentString;
    igsyajl_gen_state state[IGSYAJL_MAX_DEPTH];
    igsyajl_print_t print;
    void * ctx; /* igsyajl_buf */
    /* memory allocation routines */
    igsyajl_alloc_funcs alloc;
};

int
igsyajl_gen_config(igsyajl_gen_t *g, igsyajl_gen_option opt, ...)
{
    int rv = 1;
    va_list ap;
    va_start(ap, opt);

    switch(opt) {
        case igsyajl_gen_beautify:
        case igsyajl_gen_validate_utf8:
        case igsyajl_gen_escape_solidus:
            if (va_arg(ap, int)) g->flags |= opt;
            else g->flags &= ~opt;
            break;
        case igsyajl_gen_indent_string: {
            const char *indent = va_arg(ap, const char *);
            g->indentString = indent;
            for (; *indent; indent++) {
                if (*indent != '\n'
                    && *indent != '\v'
                    && *indent != '\f'
                    && *indent != '\t'
                    && *indent != '\r'
                    && *indent != ' ')
                {
                    g->indentString = NULL;
                    rv = 0;
                }
            }
            break;
        }
        case igsyajl_gen_print_callback:
            igsyajl_buf_free(g->ctx);
            g->print = va_arg(ap, const igsyajl_print_t);
            g->ctx = va_arg(ap, void *);
            break;
        default:
            rv = 0;
    }

    va_end(ap);

    return rv;
}



igsyajl_gen_t*
igsyajl_gen_alloc(const igsyajl_alloc_funcs * afs)
{
    igsyajl_gen_t *g = NULL;
    igsyajl_alloc_funcs afsBuffer;

    /* first order of business is to set up memory allocation routines */
    if (afs) {
        if (afs->malloc == NULL || afs->realloc == NULL || afs->free == NULL)
        {
            return NULL;
        }
    } else {
        igsyajl_set_default_alloc_funcs(&afsBuffer);
        afs = &afsBuffer;
    }

    g = (igsyajl_gen_t*) YA_MALLOC(afs, sizeof(struct igsyajl_gen));
    if (!g) return NULL;

    memset((void *) g, 0, sizeof(struct igsyajl_gen));
    /* copy in pointers to allocation routines */
    memcpy((void *) &(g->alloc), (void *) afs, sizeof(igsyajl_alloc_funcs));

    g->print = (igsyajl_print_t)&igsyajl_buf_append;
    g->ctx = igsyajl_buf_alloc(&(g->alloc));
    g->indentString = "    ";

    return g;
}

void
igsyajl_gen_reset(igsyajl_gen_t *g, const char * sep)
{
    g->depth = 0;
    memset((void *) &(g->state), 0, sizeof(g->state));
    if (sep) g->print(g->ctx, sep, strlen(sep));
}

void
igsyajl_gen_free(igsyajl_gen_t *g)
{
    if (g->print == (igsyajl_print_t)&igsyajl_buf_append) igsyajl_buf_free((igsyajl_buf)g->ctx);
    YA_FREE(&(g->alloc), g);
}

#define INSERT_SEP \
    if (g->state[g->depth] == igsyajl_gen_map_key ||               \
        g->state[g->depth] == igsyajl_gen_in_array) {              \
        g->print(g->ctx, ",", 1);                               \
        if ((g->flags & igsyajl_gen_beautify)) g->print(g->ctx, "\n", 1);               \
    } else if (g->state[g->depth] == igsyajl_gen_map_val) {        \
        g->print(g->ctx, ":", 1);                               \
        if ((g->flags & igsyajl_gen_beautify)) g->print(g->ctx, " ", 1);                \
   }

#define INSERT_WHITESPACE                                               \
    if ((g->flags & igsyajl_gen_beautify)) {                                                    \
        if (g->state[g->depth] != igsyajl_gen_map_val) {                   \
            unsigned int _i;                                            \
            for (_i=0;_i<g->depth;_i++)                                 \
                g->print(g->ctx,                                        \
                         g->indentString,                               \
                         (unsigned int)strlen(g->indentString));        \
        }                                                               \
    }

#define ENSURE_NOT_KEY \
    if (g->state[g->depth] == igsyajl_gen_map_key ||       \
        g->state[g->depth] == igsyajl_gen_map_start)  {    \
        return igsyajl_gen_keys_must_be_strings;           \
    }                                                   \

/* check that we're not complete, or in error state.  in a valid state
 * to be generating */
#define ENSURE_VALID_STATE \
    if (g->state[g->depth] == igsyajl_gen_error) {   \
        return igsyajl_gen_in_error_state;\
    } else if (g->state[g->depth] == igsyajl_gen_complete) {   \
        return igsyajl_gen_generation_complete;                \
    }

#define INCREMENT_DEPTH \
    if (++(g->depth) >= IGSYAJL_MAX_DEPTH) return igsyajl_max_depth_exceeded;

#define DECREMENT_DEPTH \
  if (--(g->depth) >= IGSYAJL_MAX_DEPTH) return igsyajl_gen_generation_complete;

#define APPENDED_ATOM \
    switch (g->state[g->depth]) {                   \
        case igsyajl_gen_start:                        \
            g->state[g->depth] = igsyajl_gen_complete; \
            break;                                  \
        case igsyajl_gen_map_start:                    \
        case igsyajl_gen_map_key:                      \
            g->state[g->depth] = igsyajl_gen_map_val;  \
            break;                                  \
        case igsyajl_gen_array_start:                  \
            g->state[g->depth] = igsyajl_gen_in_array; \
            break;                                  \
        case igsyajl_gen_map_val:                      \
            g->state[g->depth] = igsyajl_gen_map_key;  \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \

#define FINAL_NEWLINE                                        \
    if ((g->flags & igsyajl_gen_beautify) && g->state[g->depth] == igsyajl_gen_complete) \
        g->print(g->ctx, "\n", 1);

igsyajl_gen_status
igsyajl_gen_integer(igsyajl_gen_t *g, long long int number)
{
    char i[32];
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    snprintf(i, 32, "%lld", number);
    g->print(g->ctx, i, (unsigned int)strlen(i));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

#if defined(_WIN32) || defined(WIN32)
#include <float.h>
#define isnan _isnan
#define isinf !_finite
#endif

igsyajl_gen_status
igsyajl_gen_double(igsyajl_gen_t *g, double number)
{
    char i[32];
    ENSURE_VALID_STATE; ENSURE_NOT_KEY;
    if (isnan(number) || isinf(number)) return igsyajl_gen_invalid_number;
    INSERT_SEP; INSERT_WHITESPACE;
    igsyajl_snprintf(i, 32, "%.20g", number);
    if (strspn(i, "0123456789-") == strlen(i)) {
        strncat(i, ".0", 31);
    }
    g->print(g->ctx, i, (unsigned int)strlen(i));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_number(igsyajl_gen_t *g, const char * s, size_t l)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, s, l);
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_string(igsyajl_gen_t *g, const unsigned char * str,
                size_t len)
{
    // if validation is enabled, check that the string is valid utf8
    // XXX: This checking could be done a little faster, in the same pass as
    // the string encoding
    if (g->flags & igsyajl_gen_validate_utf8) {
        if (!igsyajl_string_validate_utf8(str, len)) {
            return igsyajl_gen_invalid_string;
        }
    }
    ENSURE_VALID_STATE; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, "\"", 1);
    igsyajl_string_encode(g->print, g->ctx, str, len, g->flags & igsyajl_gen_escape_solidus);
    g->print(g->ctx, "\"", 1);
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_null(igsyajl_gen_t *g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, "null", strlen("null"));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_bool(igsyajl_gen_t *g, int boolean)
{
    const char * val = boolean ? "true" : "false";

	ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    g->print(g->ctx, val, (unsigned int)strlen(val));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_map_open(igsyajl_gen_t *g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    INCREMENT_DEPTH;

    g->state[g->depth] = igsyajl_gen_map_start;
    g->print(g->ctx, "{", 1);
    if ((g->flags & igsyajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_map_close(igsyajl_gen_t *g)
{
    ENSURE_VALID_STATE;
    DECREMENT_DEPTH;

    if ((g->flags & igsyajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    APPENDED_ATOM;
    INSERT_WHITESPACE;
    g->print(g->ctx, "}", 1);
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_array_open(igsyajl_gen_t *g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    INCREMENT_DEPTH;
    g->state[g->depth] = igsyajl_gen_array_start;
    g->print(g->ctx, "[", 1);
    if ((g->flags & igsyajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_array_close(igsyajl_gen_t *g)
{
    ENSURE_VALID_STATE;
    DECREMENT_DEPTH;
    if ((g->flags & igsyajl_gen_beautify)) g->print(g->ctx, "\n", 1);
    APPENDED_ATOM;
    INSERT_WHITESPACE;
    g->print(g->ctx, "]", 1);
    FINAL_NEWLINE;
    return igsyajl_gen_status_ok;
}

igsyajl_gen_status
igsyajl_gen_get_buf(igsyajl_gen_t *g, const unsigned char ** buf,
                 size_t * len)
{
    if (g->print != (igsyajl_print_t)&igsyajl_buf_append) return igsyajl_gen_no_buf;
    *buf = igsyajl_buf_data((igsyajl_buf)g->ctx);
    *len = igsyajl_buf_len((igsyajl_buf)g->ctx);
    return igsyajl_gen_status_ok;
}

void
igsyajl_gen_clear(igsyajl_gen_t *g)
{
    if (g->print == (igsyajl_print_t)&igsyajl_buf_append) igsyajl_buf_clear((igsyajl_buf)g->ctx);
}
