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

#ifndef __YAJL_LEX_H__
#define __YAJL_LEX_H__

#include "api/yajl_common.h"

typedef enum {
    igsyajl_tok_bool,
    igsyajl_tok_colon,
    igsyajl_tok_comma,
    igsyajl_tok_eof,
    igsyajl_tok_error,
    igsyajl_tok_left_brace,
    igsyajl_tok_left_bracket,
    igsyajl_tok_null,
    igsyajl_tok_right_brace,
    igsyajl_tok_right_bracket,

    /* we differentiate between integers and doubles to allow the
     * parser to interpret the number without re-scanning */
    igsyajl_tok_integer,
    igsyajl_tok_double,

    /* we differentiate between strings which require further processing,
     * and strings that do not */
    igsyajl_tok_string,
    igsyajl_tok_string_with_escapes,

    /* comment tokens are not currently returned to the parser, ever */
    igsyajl_tok_comment
} igsyajl_tok;

typedef struct igsyajl_lexer_t * igsyajl_lexer;

igsyajl_lexer igsyajl_lex_alloc(igsyajl_alloc_funcs * alloc,
                          unsigned int allowComments,
                          unsigned int validateUTF8);

void igsyajl_lex_free(igsyajl_lexer lexer);

/**
 * run/continue a lex. "offset" is an input/output parameter.
 * It should be initialized to zero for a
 * new chunk of target text, and upon subsetquent calls with the same
 * target text should passed with the value of the previous invocation.
 *
 * the client may be interested in the value of offset when an error is
 * returned from the lexer.  This allows the client to render useful
 * error messages.
 *
 * When you pass the next chunk of data, context should be reinitialized
 * to zero.
 *
 * Finally, the output buffer is usually just a pointer into the jsonText,
 * however in cases where the entity being lexed spans multiple chunks,
 * the lexer will buffer the entity and the data returned will be
 * a pointer into that buffer.
 *
 * This behavior is abstracted from client code except for the performance
 * implications which require that the client choose a reasonable chunk
 * size to get adequate performance.
 */
igsyajl_tok igsyajl_lex_lex(igsyajl_lexer lexer, const unsigned char * jsonText,
                      size_t jsonTextLen, size_t * offset,
                      const unsigned char ** outBuf, size_t * outLen);

/** have a peek at the next token, but don't move the lexer forward */
igsyajl_tok igsyajl_lex_peek(igsyajl_lexer lexer, const unsigned char * jsonText,
                       size_t jsonTextLen, size_t offset);


typedef enum {
    igsyajl_lex_e_ok = 0,
    igsyajl_lex_string_invalid_utf8,
    igsyajl_lex_string_invalid_escaped_char,
    igsyajl_lex_string_invalid_json_char,
    igsyajl_lex_string_invalid_hex_char,
    igsyajl_lex_invalid_char,
    igsyajl_lex_invalid_string,
    igsyajl_lex_missing_integer_after_decimal,
    igsyajl_lex_missing_integer_after_exponent,
    igsyajl_lex_missing_integer_after_minus,
    igsyajl_lex_unallowed_comment
} igsyajl_lex_error;

const char * igsyajl_lex_error_to_string(igsyajl_lex_error error);

/** allows access to more specific information about the lexical
 *  error when igsyajl_lex_lex returns igsyajl_tok_error. */
igsyajl_lex_error igsyajl_lex_get_error(igsyajl_lexer lexer);

/** get the current offset into the most recently lexed json string. */
size_t igsyajl_lex_current_offset(igsyajl_lexer lexer);

/** get the number of lines lexed by this lexer instance */
size_t igsyajl_lex_current_line(igsyajl_lexer lexer);

/** get the number of chars lexed by this lexer instance since the last
 *  \n or \r */
size_t igsyajl_lex_current_char(igsyajl_lexer lexer);

#endif
