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
 * \file igsyajl_parse.h
 * Interface to YAJL's JSON stream parsing facilities.
 */

#include "yajl_common.h"

#ifndef __YAJL_PARSE_H__
#define __YAJL_PARSE_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** error codes returned from this interface */
    typedef enum {
        /** no error was encountered */
        igsyajl_status_ok,
        /** a client callback returned zero, stopping the parse */
        igsyajl_status_client_canceled,
        /** An error occured during the parse.  Call igsyajl_get_error for
         *  more information about the encountered error */
        igsyajl_status_error
    } igsyajl_status;

    /** attain a human readable, english, string for an error */
    IGSYAJL_API const char * igsyajl_status_to_string(igsyajl_status code);

    /** an opaque handle to a parser */
    typedef struct igsyajl_handle_t * igsyajl_handle;

    /** yajl is an event driven parser.  this means as json elements are
     *  parsed, you are called back to do something with the data.  The
     *  functions in this table indicate the various events for which
     *  you will be called back.  Each callback accepts a "context"
     *  pointer, this is a void * that is passed into the igsyajl_parse
     *  function which the client code may use to pass around context.
     *
     *  All callbacks return an integer.  If non-zero, the parse will
     *  continue.  If zero, the parse will be canceled and
     *  igsyajl_status_client_canceled will be returned from the parse.
     *
     *  \attention {
     *    A note about the handling of numbers:
     *
     *    yajl will only convert numbers that can be represented in a
     *    double or a 64 bit (long long) int.  All other numbers will
     *    be passed to the client in string form using the igsyajl_number
     *    callback.  Furthermore, if igsyajl_number is not NULL, it will
     *    always be used to return numbers, that is igsyajl_integer and
     *    igsyajl_double will be ignored.  If igsyajl_number is NULL but one
     *    of igsyajl_integer or igsyajl_double are defined, parsing of a
     *    number larger than is representable in a double or 64 bit
     *    integer will result in a parse error.
     *  }
     */
    typedef struct {
        int (* igsyajl_null)(void * ctx);
        int (* igsyajl_boolean)(void * ctx, int boolVal);
        int (* igsyajl_integer)(void * ctx, long long integerVal);
        int (* igsyajl_double)(void * ctx, double doubleVal);
        /** A callback which passes the string representation of the number
         *  back to the client.  Will be used for all numbers when present */
        int (* igsyajl_number)(void * ctx, const char * numberVal,
                            size_t numberLen);

        /** strings are returned as pointers into the JSON text when,
         * possible, as a result, they are _not_ null padded */
        int (* igsyajl_string)(void * ctx, const unsigned char * stringVal,
                            size_t stringLen);

        int (* igsyajl_start_map)(void * ctx);
        int (* igsyajl_map_key)(void * ctx, const unsigned char * key,
                             size_t stringLen);
        int (* igsyajl_end_map)(void * ctx);

        int (* igsyajl_start_array)(void * ctx);
        int (* igsyajl_end_array)(void * ctx);
    } igsyajl_callbacks;

    /** allocate a parser handle
     *  \param callbacks  a yajl callbacks structure specifying the
     *                    functions to call when different JSON entities
     *                    are encountered in the input text.  May be NULL,
     *                    which is only useful for validation.
     *  \param afs        memory allocation functions, may be NULL for to use
     *                    C runtime library routines (malloc and friends) 
     *  \param ctx        a context pointer that will be passed to callbacks.
     */
    IGSYAJL_API igsyajl_handle igsyajl_alloc(const igsyajl_callbacks * callbacks,
                                    igsyajl_alloc_funcs * afs,
                                    void * ctx);


    /** configuration parameters for the parser, these may be passed to
     *  igsyajl_config() along with option specific argument(s).  In general,
     *  all configuration parameters default to *off*. */
    typedef enum {
        /** Ignore javascript style comments present in
         *  JSON input.  Non-standard, but rather fun
         *  arguments: toggled off with integer zero, on otherwise.
         *
         *  example:
         *    igsyajl_config(h, igsyajl_allow_comments, 1); // turn comment support on
         */
        igsyajl_allow_comments = 0x01,
        /**
         * When set the parser will verify that all strings in JSON input are
         * valid UTF8 and will emit a parse error if this is not so.  When set,
         * this option makes parsing slightly more expensive (~7% depending
         * on processor and compiler in use)
         *
         * example:
         *   igsyajl_config(h, igsyajl_dont_validate_strings, 1); // disable utf8 checking
         */
        igsyajl_dont_validate_strings     = 0x02,
        /**
         * By default, upon calls to igsyajl_complete_parse(), yajl will
         * ensure the entire input text was consumed and will raise an error
         * otherwise.  Enabling this flag will cause yajl to disable this
         * check.  This can be useful when parsing json out of a that contains more
         * than a single JSON document.
         */
        igsyajl_allow_trailing_garbage = 0x04,
        /**
         * Allow multiple values to be parsed by a single handle.  The
         * entire text must be valid JSON, and values can be seperated
         * by any kind of whitespace.  This flag will change the
         * behavior of the parser, and cause it continue parsing after
         * a value is parsed, rather than transitioning into a
         * complete state.  This option can be useful when parsing multiple
         * values from an input stream.
         */
        igsyajl_allow_multiple_values = 0x08,
        /**
         * When igsyajl_complete_parse() is called the parser will
         * check that the top level value was completely consumed.  I.E.,
         * if called whilst in the middle of parsing a value
         * yajl will enter an error state (premature EOF).  Setting this
         * flag suppresses that check and the corresponding error.
         */
        igsyajl_allow_partial_values = 0x10
    } igsyajl_option;

    /** allow the modification of parser options subsequent to handle
     *  allocation (via igsyajl_alloc)
     *  \returns zero in case of errors, non-zero otherwise
     */
    IGSYAJL_API int igsyajl_config(igsyajl_handle h, igsyajl_option opt, ...);

    /** free a parser handle */
    IGSYAJL_API void igsyajl_free(igsyajl_handle handle);

    /** Parse some json!
     *  \param hand - a handle to the json parser allocated with igsyajl_alloc
     *  \param jsonText - a pointer to the UTF8 json text to be parsed
     *  \param jsonTextLength - the length, in bytes, of input text
     */
    IGSYAJL_API igsyajl_status igsyajl_parse(igsyajl_handle hand,
                                    const unsigned char * jsonText,
                                    size_t jsonTextLength);

    /** Parse any remaining buffered json.
     *  Since yajl is a stream-based parser, without an explicit end of
     *  input, yajl sometimes can't decide if content at the end of the
     *  stream is valid or not.  For example, if "1" has been fed in,
     *  yajl can't know whether another digit is next or some character
     *  that would terminate the integer token.
     *
     *  \param hand - a handle to the json parser allocated with igsyajl_alloc
     */
    IGSYAJL_API igsyajl_status igsyajl_complete_parse(igsyajl_handle hand);

    /** get an error string describing the state of the
     *  parse.
     *
     *  If verbose is non-zero, the message will include the JSON
     *  text where the error occured, along with an arrow pointing to
     *  the specific char.
     *
     *  \returns A dynamically allocated string will be returned which should
     *  be freed with igsyajl_free_error
     */
    IGSYAJL_API unsigned char * igsyajl_get_error(igsyajl_handle hand, int verbose,
                                            const unsigned char * jsonText,
                                            size_t jsonTextLength);

    /**
     * get the amount of data consumed from the last chunk passed to YAJL.
     *
     * In the case of a successful parse this can help you understand if
     * the entire buffer was consumed (which will allow you to handle
     * "junk at end of input").
     *
     * In the event an error is encountered during parsing, this function
     * affords the client a way to get the offset into the most recent
     * chunk where the error occured.  0 will be returned if no error
     * was encountered.
     */
    IGSYAJL_API size_t igsyajl_get_bytes_consumed(igsyajl_handle hand);

    /** free an error returned from igsyajl_get_error */
    IGSYAJL_API void igsyajl_free_error(igsyajl_handle hand, unsigned char * str);

#ifdef __cplusplus
}
#endif

#endif
