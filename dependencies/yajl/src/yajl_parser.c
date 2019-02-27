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
#include "yajl_encode.h"
#include "yajl_bytestack.h"

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#define MAX_VALUE_TO_MULTIPLY ((LLONG_MAX / 10) + (LLONG_MAX % 10))

 /* same semantics as strtol */
long long
igsyajl_parse_integer(const unsigned char *number, size_t length)
{
    long long ret  = 0;
    long sign = 1;
    const unsigned char *pos = number;
    if (*pos == '-') { pos++; sign = -1; }
    if (*pos == '+') { pos++; }

    while (pos < number + length) {
        if ( ret > MAX_VALUE_TO_MULTIPLY ) {
            errno = ERANGE;
            return sign == 1 ? LLONG_MAX : LLONG_MIN;
        }
        ret *= 10;
        if (LLONG_MAX - ret < (*pos - '0')) {
            errno = ERANGE;
            return sign == 1 ? LLONG_MAX : LLONG_MIN;
        }
        if (*pos < '0' || *pos > '9') {
            errno = ERANGE;
            return sign == 1 ? LLONG_MAX : LLONG_MIN;
        }
        ret += (*pos++ - '0');
    }

    return sign * ret;
}

unsigned char *
igsyajl_render_error_string(igsyajl_handle hand, const unsigned char * jsonText,
                         size_t jsonTextLen, int verbose)
{
    size_t offset = hand->bytesConsumed;
    unsigned char * str;
    const char * errorType = NULL;
    const char * errorText = NULL;
    char text[72];
    const char * arrow = "                     (right here) ------^\n";

    if (igsyajl_bs_current(hand->stateStack) == igsyajl_state_parse_error) {
        errorType = "parse";
        errorText = hand->parseError;
    } else if (igsyajl_bs_current(hand->stateStack) == igsyajl_state_lexical_error) {
        errorType = "lexical";
        errorText = igsyajl_lex_error_to_string(igsyajl_lex_get_error(hand->lexer));
    } else {
        errorType = "unknown";
    }

    {
        size_t memneeded = 0;
        memneeded += strlen(errorType);
        memneeded += strlen(" error");
        if (errorText != NULL) {
            memneeded += strlen(": ");
            memneeded += strlen(errorText);
        }
        str = (unsigned char *) YA_MALLOC(&(hand->alloc), memneeded + 2);
        if (!str) return NULL;
        str[0] = 0;
        strcat((char *) str, errorType);
        strcat((char *) str, " error");
        if (errorText != NULL) {
            strcat((char *) str, ": ");
            strcat((char *) str, errorText);
        }
        strcat((char *) str, "\n");
    }

    /* now we append as many spaces as needed to make sure the error
     * falls at char 41, if verbose was specified */
    if (verbose) {
        size_t start, end, i;
        size_t spacesNeeded;

        spacesNeeded = (offset < 30 ? 40 - offset : 10);
        start = (offset >= 30 ? offset - 30 : 0);
        end = (offset + 30 > jsonTextLen ? jsonTextLen : offset + 30);

        for (i=0;i<spacesNeeded;i++) text[i] = ' ';

        for (;start < end;start++, i++) {
            if (jsonText[start] != '\n' && jsonText[start] != '\r')
            {
                text[i] = jsonText[start];
            }
            else
            {
                text[i] = ' ';
            }
        }
        assert(i <= 71);
        text[i++] = '\n';
        text[i] = 0;
        {
            char * newStr = (char *)
                YA_MALLOC(&(hand->alloc), (unsigned int)(strlen((char *) str) +
                                                         strlen((char *) text) +
                                                         strlen(arrow) + 1));
            if (newStr) {
                newStr[0] = 0;
                strcat((char *) newStr, (char *) str);
                strcat((char *) newStr, text);
                strcat((char *) newStr, arrow);
            }
            YA_FREE(&(hand->alloc), str);
            str = (unsigned char *) newStr;
        }
    }
    return str;
}

/* check for client cancelation */
#define _CC_CHK(x)                                                \
    if (!(x)) {                                                   \
        igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);    \
        hand->parseError =                                        \
            "client cancelled parse via callback return value";   \
        return igsyajl_status_client_canceled;                       \
    }


igsyajl_status
igsyajl_do_finish(igsyajl_handle hand)
{
    igsyajl_status stat;
    stat = igsyajl_do_parse(hand,(const unsigned char *) " ",1);

    if (stat != igsyajl_status_ok) return stat;

    switch(igsyajl_bs_current(hand->stateStack))
    {
        case igsyajl_state_parse_error:
        case igsyajl_state_lexical_error:
            return igsyajl_status_error;
        case igsyajl_state_got_value:
        case igsyajl_state_parse_complete:
            return igsyajl_status_ok;
        default:
            if (!(hand->flags & igsyajl_allow_partial_values))
            {
                igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                hand->parseError = "premature EOF";
                return igsyajl_status_error;
            }
            return igsyajl_status_ok;
    }
}

igsyajl_status
igsyajl_do_parse(igsyajl_handle hand, const unsigned char * jsonText,
              size_t jsonTextLen)
{
    igsyajl_tok tok;
    const unsigned char * buf;
    size_t bufLen;
    size_t * offset = &(hand->bytesConsumed);

    *offset = 0;

  around_again:
    switch (igsyajl_bs_current(hand->stateStack)) {
        case igsyajl_state_parse_complete:
            if (hand->flags & igsyajl_allow_multiple_values) {
                igsyajl_bs_set(hand->stateStack, igsyajl_state_got_value);
                goto around_again;
            }
            if (!(hand->flags & igsyajl_allow_trailing_garbage)) {
                if (*offset != jsonTextLen) {
                    tok = igsyajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                                       offset, &buf, &bufLen);
                    if (tok != igsyajl_tok_eof) {
                        igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                        hand->parseError = "trailing garbage";
                    }
                    goto around_again;
                }
            }
            return igsyajl_status_ok;
        case igsyajl_state_lexical_error:
        case igsyajl_state_parse_error:
            return igsyajl_status_error;
        case igsyajl_state_start:
        case igsyajl_state_got_value:
        case igsyajl_state_map_need_val:
        case igsyajl_state_array_need_val:
        case igsyajl_state_array_start:  {
            /* for arrays and maps, we advance the state for this
             * depth, then push the state of the next depth.
             * If an error occurs during the parsing of the nesting
             * enitity, the state at this level will not matter.
             * a state that needs pushing will be anything other
             * than state_start */

            igsyajl_state stateToPush = igsyajl_state_start;

            tok = igsyajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);

            switch (tok) {
                case igsyajl_tok_eof:
                    return igsyajl_status_ok;
                case igsyajl_tok_error:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_lexical_error);
                    goto around_again;
                case igsyajl_tok_string:
                    if (hand->callbacks && hand->callbacks->igsyajl_string) {
                        _CC_CHK(hand->callbacks->igsyajl_string(hand->ctx,
                                                             buf, bufLen));
                    }
                    break;
                case igsyajl_tok_string_with_escapes:
                    if (hand->callbacks && hand->callbacks->igsyajl_string) {
                        igsyajl_buf_clear(hand->decodeBuf);
                        igsyajl_string_decode(hand->decodeBuf, buf, bufLen);
                        _CC_CHK(hand->callbacks->igsyajl_string(
                                    hand->ctx, igsyajl_buf_data(hand->decodeBuf),
                                    igsyajl_buf_len(hand->decodeBuf)));
                    }
                    break;
                case igsyajl_tok_bool:
                    if (hand->callbacks && hand->callbacks->igsyajl_boolean) {
                        _CC_CHK(hand->callbacks->igsyajl_boolean(hand->ctx,
                                                              *buf == 't'));
                    }
                    break;
                case igsyajl_tok_null:
                    if (hand->callbacks && hand->callbacks->igsyajl_null) {
                        _CC_CHK(hand->callbacks->igsyajl_null(hand->ctx));
                    }
                    break;
                case igsyajl_tok_left_bracket:
                    if (hand->callbacks && hand->callbacks->igsyajl_start_map) {
                        _CC_CHK(hand->callbacks->igsyajl_start_map(hand->ctx));
                    }
                    stateToPush = igsyajl_state_map_start;
                    break;
                case igsyajl_tok_left_brace:
                    if (hand->callbacks && hand->callbacks->igsyajl_start_array) {
                        _CC_CHK(hand->callbacks->igsyajl_start_array(hand->ctx));
                    }
                    stateToPush = igsyajl_state_array_start;
                    break;
                case igsyajl_tok_integer:
                    if (hand->callbacks) {
                        if (hand->callbacks->igsyajl_number) {
                            _CC_CHK(hand->callbacks->igsyajl_number(
                                        hand->ctx,(const char *) buf, bufLen));
                        } else if (hand->callbacks->igsyajl_integer) {
                            long long int i = 0;
                            errno = 0;
                            i = igsyajl_parse_integer(buf, bufLen);
                            if ((i == LLONG_MIN || i == LLONG_MAX) &&
                                errno == ERANGE)
                            {
                                igsyajl_bs_set(hand->stateStack,
                                            igsyajl_state_parse_error);
                                hand->parseError = "integer overflow" ;
                                /* try to restore error offset */
                                if (*offset >= bufLen) *offset -= bufLen;
                                else *offset = 0;
                                goto around_again;
                            }
                            _CC_CHK(hand->callbacks->igsyajl_integer(hand->ctx,
                                                                  i));
                        }
                    }
                    break;
                case igsyajl_tok_double:
                    if (hand->callbacks) {
                        if (hand->callbacks->igsyajl_number) {
                            _CC_CHK(hand->callbacks->igsyajl_number(
                                        hand->ctx, (const char *) buf, bufLen));
                        } else if (hand->callbacks->igsyajl_double) {
                            double d = 0.0;
                            igsyajl_buf_clear(hand->decodeBuf);
                            igsyajl_buf_append(hand->decodeBuf, buf, bufLen);
                            buf = igsyajl_buf_data(hand->decodeBuf);
                            errno = 0;
                            d = strtod((char *) buf, NULL);
                            if ((d >= HUGE_VAL || d <= -HUGE_VAL) &&
                                errno == ERANGE)
                            {
                                igsyajl_bs_set(hand->stateStack,
                                            igsyajl_state_parse_error);
                                hand->parseError = "numeric (floating point) "
                                    "overflow";
                                /* try to restore error offset */
                                if (*offset >= bufLen) *offset -= bufLen;
                                else *offset = 0;
                                goto around_again;
                            }
                            _CC_CHK(hand->callbacks->igsyajl_double(hand->ctx,
                                                                 d));
                        }
                    }
                    break;
                case igsyajl_tok_right_brace: {
                    if (igsyajl_bs_current(hand->stateStack) ==
                        igsyajl_state_array_start)
                    {
                        if (hand->callbacks &&
                            hand->callbacks->igsyajl_end_array)
                        {
                            _CC_CHK(hand->callbacks->igsyajl_end_array(hand->ctx));
                        }
                        igsyajl_bs_pop(hand->stateStack);
                        goto around_again;
                    }
                    /* intentional fall-through */
                }
                case igsyajl_tok_colon:
                case igsyajl_tok_comma:
                case igsyajl_tok_right_bracket:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                    hand->parseError =
                        "unallowed token at this point in JSON text";
                    goto around_again;
                default:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                    hand->parseError = "invalid token, internal error";
                    goto around_again;
            }
            /* got a value.  transition depends on the state we're in. */
            {
                igsyajl_state s = igsyajl_bs_current(hand->stateStack);
                if (s == igsyajl_state_start || s == igsyajl_state_got_value) {
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_complete);
                } else if (s == igsyajl_state_map_need_val) {
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_map_got_val);
                } else {
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_array_got_val);
                }
            }
            if (stateToPush != igsyajl_state_start) {
                igsyajl_bs_push(hand->stateStack, stateToPush);
            }

            goto around_again;
        }
        case igsyajl_state_map_start:
        case igsyajl_state_map_need_key: {
            /* only difference between these two states is that in
             * start '}' is valid, whereas in need_key, we've parsed
             * a comma, and a string key _must_ follow */
            tok = igsyajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case igsyajl_tok_eof:
                    return igsyajl_status_ok;
                case igsyajl_tok_error:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_lexical_error);
                    goto around_again;
                case igsyajl_tok_string_with_escapes:
                    if (hand->callbacks && hand->callbacks->igsyajl_map_key) {
                        igsyajl_buf_clear(hand->decodeBuf);
                        igsyajl_string_decode(hand->decodeBuf, buf, bufLen);
                        buf = igsyajl_buf_data(hand->decodeBuf);
                        bufLen = igsyajl_buf_len(hand->decodeBuf);
                    }
                    /* intentional fall-through */
                case igsyajl_tok_string:
                    if (hand->callbacks && hand->callbacks->igsyajl_map_key) {
                        _CC_CHK(hand->callbacks->igsyajl_map_key(hand->ctx, buf,
                                                              bufLen));
                    }
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_map_sep);
                    goto around_again;
                case igsyajl_tok_right_bracket:
                    if (igsyajl_bs_current(hand->stateStack) ==
                        igsyajl_state_map_start)
                    {
                        if (hand->callbacks && hand->callbacks->igsyajl_end_map) {
                            _CC_CHK(hand->callbacks->igsyajl_end_map(hand->ctx));
                        }
                        igsyajl_bs_pop(hand->stateStack);
                        goto around_again;
                    }
                default:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                    hand->parseError =
                        "invalid object key (must be a string)"; 
                    goto around_again;
            }
        }
        case igsyajl_state_map_sep: {
            tok = igsyajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case igsyajl_tok_colon:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_map_need_val);
                    goto around_again;
                case igsyajl_tok_eof:
                    return igsyajl_status_ok;
                case igsyajl_tok_error:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_lexical_error);
                    goto around_again;
                default:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                    hand->parseError = "object key and value must "
                        "be separated by a colon (':')";
                    goto around_again;
            }
        }
        case igsyajl_state_map_got_val: {
            tok = igsyajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case igsyajl_tok_right_bracket:
                    if (hand->callbacks && hand->callbacks->igsyajl_end_map) {
                        _CC_CHK(hand->callbacks->igsyajl_end_map(hand->ctx));
                    }
                    igsyajl_bs_pop(hand->stateStack);
                    goto around_again;
                case igsyajl_tok_comma:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_map_need_key);
                    goto around_again;
                case igsyajl_tok_eof:
                    return igsyajl_status_ok;
                case igsyajl_tok_error:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_lexical_error);
                    goto around_again;
                default:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                    hand->parseError = "after key and value, inside map, "
                                       "I expect ',' or '}'";
                    /* try to restore error offset */
                    if (*offset >= bufLen) *offset -= bufLen;
                    else *offset = 0;
                    goto around_again;
            }
        }
        case igsyajl_state_array_got_val: {
            tok = igsyajl_lex_lex(hand->lexer, jsonText, jsonTextLen,
                               offset, &buf, &bufLen);
            switch (tok) {
                case igsyajl_tok_right_brace:
                    if (hand->callbacks && hand->callbacks->igsyajl_end_array) {
                        _CC_CHK(hand->callbacks->igsyajl_end_array(hand->ctx));
                    }
                    igsyajl_bs_pop(hand->stateStack);
                    goto around_again;
                case igsyajl_tok_comma:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_array_need_val);
                    goto around_again;
                case igsyajl_tok_eof:
                    return igsyajl_status_ok;
                case igsyajl_tok_error:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_lexical_error);
                    goto around_again;
                default:
                    igsyajl_bs_set(hand->stateStack, igsyajl_state_parse_error);
                    hand->parseError =
                        "after array element, I expect ',' or ']'";
                    goto around_again;
            }
        }
    }

    abort();
    return igsyajl_status_error;
}

