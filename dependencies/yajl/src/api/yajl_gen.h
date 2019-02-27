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
 * \file igsyajl_gen.h
 * Interface to YAJL's JSON generation facilities.
 */

#include "yajl_common.h"

#ifndef __YAJL_GEN_H__
#define __YAJL_GEN_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    /** generator status codes */
    typedef enum {
        /** no error */
        igsyajl_gen_status_ok = 0,
        /** at a point where a map key is generated, a function other than
         *  igsyajl_gen_string was called */
        igsyajl_gen_keys_must_be_strings,
        /** YAJL's maximum generation depth was exceeded.  see
         *  IGSYAJL_MAX_DEPTH */
        igsyajl_max_depth_exceeded,
        /** A generator function (igsyajl_gen_XXX) was called while in an error
         *  state */
        igsyajl_gen_in_error_state,
        /** A complete JSON document has been generated */
        igsyajl_gen_generation_complete,
        /** igsyajl_gen_double was passed an invalid floating point value
         *  (infinity or NaN). */
        igsyajl_gen_invalid_number,
        /** A print callback was passed in, so there is no internal
         * buffer to get from */
        igsyajl_gen_no_buf,
        /** returned from igsyajl_gen_string() when the igsyajl_gen_validate_utf8
         *  option is enabled and an invalid was passed by client code.
         */
        igsyajl_gen_invalid_string
    } igsyajl_gen_status;

    /** an opaque handle to a generator */
    typedef struct igsyajl_gen_t * igsyajl_gen;

    /** a callback used for "printing" the results. */
    typedef void (*igsyajl_print_t)(void * ctx,
                                 const char * str,
                                 size_t len);

    /** configuration parameters for the parser, these may be passed to
     *  igsyajl_gen_config() along with option specific argument(s).  In general,
     *  all configuration parameters default to *off*. */
    typedef enum {
        /** generate indented (beautiful) output */
        igsyajl_gen_beautify = 0x01,
        /**
         * Set an indent string which is used when igsyajl_gen_beautify
         * is enabled.  Maybe something like \\t or some number of
         * spaces.  The default is four spaces ' '.
         */
        igsyajl_gen_indent_string = 0x02,
        /**
         * Set a function and context argument that should be used to
         * output generated json.  the function should conform to the
         * igsyajl_print_t prototype while the context argument is a
         * void * of your choosing.
         *
         * example:
         *   igsyajl_gen_config(g, igsyajl_gen_print_callback, myFunc, myVoidPtr);
         */
        igsyajl_gen_print_callback = 0x04,
        /**
         * Normally the generator does not validate that strings you
         * pass to it via igsyajl_gen_string() are valid UTF8.  Enabling
         * this option will cause it to do so.
         */
        igsyajl_gen_validate_utf8 = 0x08,
        /**
         * the forward solidus (slash or '/' in human) is not required to be
         * escaped in json text.  By default, YAJL will not escape it in the
         * iterest of saving bytes.  Setting this flag will cause YAJL to
         * always escape '/' in generated JSON strings.
         */
        igsyajl_gen_escape_solidus = 0x10
    } igsyajl_gen_option;

    /** allow the modification of generator options subsequent to handle
     *  allocation (via igsyajl_alloc)
     *  \returns zero in case of errors, non-zero otherwise
     */
    IGSYAJL_API int igsyajl_gen_config(igsyajl_gen g, igsyajl_gen_option opt, ...);

    /** allocate a generator handle
     *  \param allocFuncs an optional pointer to a structure which allows
     *                    the client to overide the memory allocation
     *                    used by yajl.  May be NULL, in which case
     *                    malloc/free/realloc will be used.
     *
     *  \returns an allocated handle on success, NULL on failure (bad params)
     */
    IGSYAJL_API igsyajl_gen igsyajl_gen_alloc(const igsyajl_alloc_funcs * allocFuncs);

    /** free a generator handle */
    IGSYAJL_API void igsyajl_gen_free(igsyajl_gen handle);

    IGSYAJL_API igsyajl_gen_status igsyajl_gen_integer(igsyajl_gen hand, long long int number);
    /** generate a floating point number.  number may not be infinity or
     *  NaN, as these have no representation in JSON.  In these cases the
     *  generator will return 'igsyajl_gen_invalid_number' */
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_double(igsyajl_gen hand, double number);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_number(igsyajl_gen hand,
                                             const char * num,
                                             size_t len);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_string(igsyajl_gen hand,
                                             const unsigned char * str,
                                             size_t len);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_null(igsyajl_gen hand);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_bool(igsyajl_gen hand, int boolean);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_map_open(igsyajl_gen hand);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_map_close(igsyajl_gen hand);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_array_open(igsyajl_gen hand);
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_array_close(igsyajl_gen hand);

    /** access the null terminated generator buffer.  If incrementally
     *  outputing JSON, one should call igsyajl_gen_clear to clear the
     *  buffer.  This allows stream generation. */
    IGSYAJL_API igsyajl_gen_status igsyajl_gen_get_buf(igsyajl_gen hand,
                                              const unsigned char ** buf,
                                              size_t * len);

    /** clear yajl's output buffer, but maintain all internal generation
     *  state.  This function will not "reset" the generator state, and is
     *  intended to enable incremental JSON outputing. */
    IGSYAJL_API void igsyajl_gen_clear(igsyajl_gen hand);

    /** Reset the generator state.  Allows a client to generate multiple
     *  json entities in a stream. The "sep" string will be inserted to
     *  separate the previously generated entity from the current,
     *  NULL means *no separation* of entites (clients beware, generating
     *  multiple JSON numbers without a separator, for instance, will result in ambiguous output)
     *
     *  Note: this call will not clear yajl's output buffer.  This
     *  may be accomplished explicitly by calling igsyajl_gen_clear() */
    IGSYAJL_API void igsyajl_gen_reset(igsyajl_gen hand, const char * sep);

#ifdef __cplusplus
}
#endif

#endif
