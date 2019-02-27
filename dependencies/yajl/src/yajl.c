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

#include "api/yajl_parse.h"
#include "yajl_lex.h"
#include "yajl_parser.h"
#include "yajl_alloc.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

const char *
igsyajl_status_to_string(igsyajl_status stat)
{
    const char * statStr = "unknown";
    switch (stat) {
        case igsyajl_status_ok:
            statStr = "ok, no error";
            break;
        case igsyajl_status_client_canceled:
            statStr = "client canceled parse";
            break;
        case igsyajl_status_error:
            statStr = "parse error";
            break;
    }
    return statStr;
}

igsyajl_handle
igsyajl_alloc(const igsyajl_callbacks * callbacks,
           igsyajl_alloc_funcs * afs,
           void * ctx)
{
    igsyajl_handle hand = NULL;
    igsyajl_alloc_funcs afsBuffer;

    /* first order of business is to set up memory allocation routines */
    if (afs != NULL) {
        if (afs->malloc == NULL || afs->realloc == NULL || afs->free == NULL)
        {
            return NULL;
        }
    } else {
        igsyajl_set_default_alloc_funcs(&afsBuffer);
        afs = &afsBuffer;
    }

    hand = (igsyajl_handle) YA_MALLOC(afs, sizeof(struct igsyajl_handle_t));

    /* copy in pointers to allocation routines */
    memcpy((void *) &(hand->alloc), (void *) afs, sizeof(igsyajl_alloc_funcs));

    hand->callbacks = callbacks;
    hand->ctx = ctx;
    hand->lexer = NULL; 
    hand->bytesConsumed = 0;
    hand->decodeBuf = igsyajl_buf_alloc(&(hand->alloc));
    hand->flags	    = 0;
    igsyajl_bs_init(hand->stateStack, &(hand->alloc));
    igsyajl_bs_push(hand->stateStack, igsyajl_state_start);

    return hand;
}

int
igsyajl_config(igsyajl_handle h, igsyajl_option opt, ...)
{
    int rv = 1;
    va_list ap;
    va_start(ap, opt);

    switch(opt) {
        case igsyajl_allow_comments:
        case igsyajl_dont_validate_strings:
        case igsyajl_allow_trailing_garbage:
        case igsyajl_allow_multiple_values:
        case igsyajl_allow_partial_values:
            if (va_arg(ap, int)) h->flags |= opt;
            else h->flags &= ~opt;
            break;
        default:
            rv = 0;
    }
    va_end(ap);

    return rv;
}

void
igsyajl_free(igsyajl_handle handle)
{
    igsyajl_bs_free(handle->stateStack);
    igsyajl_buf_free(handle->decodeBuf);
    if (handle->lexer) {
        igsyajl_lex_free(handle->lexer);
        handle->lexer = NULL;
    }
    YA_FREE(&(handle->alloc), handle);
}

igsyajl_status
igsyajl_parse(igsyajl_handle hand, const unsigned char * jsonText,
           size_t jsonTextLen)
{
    igsyajl_status status;

    /* lazy allocation of the lexer */
    if (hand->lexer == NULL) {
        hand->lexer = igsyajl_lex_alloc(&(hand->alloc),
                                     hand->flags & igsyajl_allow_comments,
                                     !(hand->flags & igsyajl_dont_validate_strings));
    }

    status = igsyajl_do_parse(hand, jsonText, jsonTextLen);
    return status;
}


igsyajl_status
igsyajl_complete_parse(igsyajl_handle hand)
{
    /* The lexer is lazy allocated in the first call to parse.  if parse is
     * never called, then no data was provided to parse at all.  This is a
     * "premature EOF" error unless igsyajl_allow_partial_values is specified.
     * allocating the lexer now is the simplest possible way to handle this
     * case while preserving all the other semantics of the parser
     * (multiple values, partial values, etc). */
    if (hand->lexer == NULL) {
        hand->lexer = igsyajl_lex_alloc(&(hand->alloc),
                                     hand->flags & igsyajl_allow_comments,
                                     !(hand->flags & igsyajl_dont_validate_strings));
    }

    return igsyajl_do_finish(hand);
}

unsigned char *
igsyajl_get_error(igsyajl_handle hand, int verbose,
               const unsigned char * jsonText, size_t jsonTextLen)
{
    return igsyajl_render_error_string(hand, jsonText, jsonTextLen, verbose);
}

size_t
igsyajl_get_bytes_consumed(igsyajl_handle hand)
{
    if (!hand) return 0;
    else return hand->bytesConsumed;
}


void
igsyajl_free_error(igsyajl_handle hand, unsigned char * str)
{
    /* use memory allocation functions if set */
    YA_FREE(&(hand->alloc), str);
}

/* XXX: add utility routines to parse from file */
