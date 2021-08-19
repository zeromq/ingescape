/*
 * Copyright (c) 2010-2011  Florian Forster  <ff at octo.it>
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
 * \file igsyajl_tree.h
 *
 * Parses JSON data and returns the data in tree form.
 *
 * \author Florian Forster
 * \date August 2010
 *
 * This interface makes quick parsing and extraction of
 * smallish JSON docs trivial:
 *
 * \include example/parse_config.c
 */

#ifndef IGSYAJL_TREE_H
#define IGSYAJL_TREE_H 1

#include "yajl_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** possible data types that a igsyajl_val_s can hold */
typedef enum {
    igsyajl_t_string = 1,
    igsyajl_t_number = 2,
    igsyajl_t_object = 3,
    igsyajl_t_array = 4,
    igsyajl_t_true = 5,
    igsyajl_t_false = 6,
    igsyajl_t_null = 7,
    /** The any type isn't valid for igsyajl_val_s.type, but can be
     *  used as an argument to routines like igsyajl_tree_get().
     */
    igsyajl_t_any = 8
} igsyajl_type;

#define IGSYAJL_NUMBER_INT_VALID    0x01
#define IGSYAJL_NUMBER_DOUBLE_VALID 0x02

/** A pointer to a node in the parse tree */
typedef struct igsyajl_val_s * igsyajl_val;

/**
 * A JSON value representation capable of holding one of the seven
 * types above. For "string", "number", "object", and "array"
 * additional data is available in the union.  The "IGSYAJL_IS_*"
 * and "IGSYAJL_GET_*" macros below allow type checking and convenient
 * value extraction.
 */
struct igsyajl_val_s
{
    /** Type of the value contained. Use the "IGSYAJL_IS_*" macros to check for a
     * specific type. */
    igsyajl_type type;
    /** Type-specific data. You may use the "IGSYAJL_GET_*" macros to access these
     * members. */
    union
    {
        char * string;
        struct {
            long long i; /**< integer value, if representable. */
            double  d;   /**< double value, if representable. */
            char   *r;   /**< unparsed number in string form. */
            /** Signals whether the \em i and \em d members are
             * valid. See \c IGSYAJL_NUMBER_INT_VALID and
             * \c IGSYAJL_NUMBER_DOUBLE_VALID. */
            unsigned int flags;
        } number;
        struct {
            const char **keys; /**< Array of keys */
            igsyajl_val *values; /**< Array of values. */
            size_t len; /**< Number of key-value-pairs. */
        } object;
        struct {
            igsyajl_val *values; /**< Array of elements. */
            size_t len; /**< Number of elements. */
        } array;
    } u;
};

/**
 * Parse a string.
 *
 * Parses an null-terminated string containing JSON data and returns a pointer
 * to the top-level value (root of the parse tree).
 *
 * \param input              Pointer to a null-terminated utf8 string containing
 *                           JSON data.
 * \param error_buffer       Pointer to a buffer in which an error message will
 *                           be stored if \em igsyajl_tree_parse fails, or
 *                           \c NULL. The buffer will be initialized before
 *                           parsing, so its content will be destroyed even if
 *                           \em igsyajl_tree_parse succeeds.
 * \param error_buffer_size  Size of the memory area pointed to by
 *                           \em error_buffer_size. If \em error_buffer_size is
 *                           \c NULL, this argument is ignored.
 *
 * \returns Pointer to the top-level value or \c NULL on error. The memory
 * pointed to must be freed using \em igsyajl_tree_free. In case of an error, a
 * null terminated message describing the error in more detail is stored in
 * \em error_buffer if it is not \c NULL.
 */
IGSYAJL_API igsyajl_val igsyajl_tree_parse (const char *input,
                                   char *error_buffer, size_t error_buffer_size);


/**
 * Free a parse tree returned by "igsyajl_tree_parse".
 *
 * \param v Pointer to a JSON value returned by "igsyajl_tree_parse". Passing NULL
 * is valid and results in a no-op.
 */
IGSYAJL_API void igsyajl_tree_free (igsyajl_val v);

/**
 * Access a nested value inside a tree.
 *
 * \param parent the node under which you'd like to extract values.
 * \param path A null terminated array of strings, each the name of an object key
 * \param type the igsyajl_type of the object you seek, or igsyajl_t_any if any will do.
 *
 * \returns a pointer to the found value, or NULL if we came up empty.
 *
 * Future Ideas:  it'd be nice to move path to a string and implement support for
 * a teeny tiny micro language here, so you can extract array elements, do things
 * like .first and .last, even .length.  Inspiration from JSONPath and css selectors?
 * No it wouldn't be fast, but that's not what this API is about.
 */
IGSYAJL_API igsyajl_val igsyajl_tree_get(igsyajl_val parent, const char ** path, igsyajl_type type);

/* Various convenience macros to check the type of a `igsyajl_val` */
#define IGSYAJL_IS_STRING(v) (((v) != NULL) && ((v)->type == igsyajl_t_string))
#define IGSYAJL_IS_NUMBER(v) (((v) != NULL) && ((v)->type == igsyajl_t_number))
#define IGSYAJL_IS_INTEGER(v) (IGSYAJL_IS_NUMBER(v) && ((v)->u.number.flags & IGSYAJL_NUMBER_INT_VALID))
#define IGSYAJL_IS_DOUBLE(v) (IGSYAJL_IS_NUMBER(v) && ((v)->u.number.flags & IGSYAJL_NUMBER_DOUBLE_VALID))
#define IGSYAJL_IS_OBJECT(v) (((v) != NULL) && ((v)->type == igsyajl_t_object))
#define IGSYAJL_IS_ARRAY(v)  (((v) != NULL) && ((v)->type == igsyajl_t_array ))
#define IGSYAJL_IS_TRUE(v)   (((v) != NULL) && ((v)->type == igsyajl_t_true  ))
#define IGSYAJL_IS_FALSE(v)  (((v) != NULL) && ((v)->type == igsyajl_t_false ))
#define IGSYAJL_IS_NULL(v)   (((v) != NULL) && ((v)->type == igsyajl_t_null  ))

/** Given a igsyajl_val_string return a ptr to the bare string it contains,
 *  or NULL if the value is not a string. */
#define IGSYAJL_GET_STRING(v) (IGSYAJL_IS_STRING(v) ? (v)->u.string : NULL)

/** Get the string representation of a number.  You should check type first,
 *  perhaps using IGSYAJL_IS_NUMBER */
#define IGSYAJL_GET_NUMBER(v) ((v)->u.number.r)

/** Get the double representation of a number.  You should check type first,
 *  perhaps using IGSYAJL_IS_DOUBLE */
#define IGSYAJL_GET_DOUBLE(v) ((v)->u.number.d)

/** Get the 64bit (long long) integer representation of a number.  You should
 *  check type first, perhaps using IGSYAJL_IS_INTEGER */
#define IGSYAJL_GET_INTEGER(v) ((v)->u.number.i)

/** Get a pointer to a igsyajl_val_object or NULL if the value is not an object. */
#define IGSYAJL_GET_OBJECT(v) (IGSYAJL_IS_OBJECT(v) ? &(v)->u.object : NULL)

/** Get a pointer to a igsyajl_val_array or NULL if the value is not an object. */
#define IGSYAJL_GET_ARRAY(v)  (IGSYAJL_IS_ARRAY(v)  ? &(v)->u.array  : NULL)

#ifdef __cplusplus
}
#endif

#endif /* IGSYAJL_TREE_H */
