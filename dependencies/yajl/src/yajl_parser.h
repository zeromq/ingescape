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

#ifndef __YAJL_PARSER_H__
#define __YAJL_PARSER_H__

#include "api/yajl_parse.h"
#include "yajl_bytestack.h"
#include "yajl_buf.h"
#include "yajl_lex.h"


typedef enum {
    igsyajl_state_start = 0,
    igsyajl_state_parse_complete,
    igsyajl_state_parse_error,
    igsyajl_state_lexical_error,
    igsyajl_state_map_start,
    igsyajl_state_map_sep,
    igsyajl_state_map_need_val,
    igsyajl_state_map_got_val,
    igsyajl_state_map_need_key,
    igsyajl_state_array_start,
    igsyajl_state_array_got_val,
    igsyajl_state_array_need_val,
    igsyajl_state_got_value,
} igsyajl_state;

struct igsyajl_handle_t {
    const igsyajl_callbacks * callbacks;
    void * ctx;
    igsyajl_lexer lexer;
    const char * parseError;
    /* the number of bytes consumed from the last client buffer,
     * in the case of an error this will be an error offset, in the
     * case of an error this can be used as the error offset */
    size_t bytesConsumed;
    /* temporary storage for decoded strings */
    igsyajl_buf decodeBuf;
    /* a stack of states.  access with igsyajl_state_XXX routines */
    igsyajl_bytestack stateStack;
    /* memory allocation routines */
    igsyajl_alloc_funcs alloc;
    /* bitfield */
    unsigned int flags;
};

igsyajl_status
igsyajl_do_parse(igsyajl_handle handle, const unsigned char * jsonText,
              size_t jsonTextLen);

igsyajl_status
igsyajl_do_finish(igsyajl_handle handle);

unsigned char *
igsyajl_render_error_string(igsyajl_handle hand, const unsigned char * jsonText,
                         size_t jsonTextLen, int verbose);

/* A little built in integer parsing routine with the same semantics as strtol
 * that's unaffected by LOCALE. */
long long
igsyajl_parse_integer(const unsigned char *number, size_t length);


#endif
