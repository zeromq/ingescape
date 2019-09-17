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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "api/yajl_tree.h"
#include "api/yajl_parse.h"

#include "yajl_parser.h"

#if defined(_WIN32) || defined(WIN32)
#define snprintf sprintf_s
#endif

#define STATUS_CONTINUE 1
#define STATUS_ABORT    0

struct stack_elem_s;
typedef struct stack_elem_s stack_elem_t;
struct stack_elem_s
{
    char * key;
    igsyajl_val value;
    stack_elem_t *next;
};

struct context_s
{
    stack_elem_t *stack;
    igsyajl_val root;
    char *errbuf;
    size_t errbuf_size;
};
typedef struct context_s context_t;

#define RETURN_ERROR(ctx,retval,...) {                                  \
        if ((ctx)->errbuf != NULL)                                      \
            snprintf ((ctx)->errbuf, (ctx)->errbuf_size, __VA_ARGS__);  \
        return (retval);                                                \
    }

static igsyajl_val value_alloc (igsyajl_type type)
{
    igsyajl_val v;

    v = malloc (sizeof (*v));
    if (v == NULL) return (NULL);
    memset (v, 0, sizeof (*v));
    v->type = type;

    return (v);
}

static void igsyajl_object_free (igsyajl_val v)
{
    size_t i;

    if (!IGSYAJL_IS_OBJECT(v)) return;

    for (i = 0; i < v->u.object.len; i++)
    {
        free((char *) v->u.object.keys[i]);
        v->u.object.keys[i] = NULL;
        igsyajl_tree_free (v->u.object.values[i]);
        v->u.object.values[i] = NULL;
    }

    free((void*) v->u.object.keys);
    free(v->u.object.values);
    free(v);
}

static void igsyajl_array_free (igsyajl_val v)
{
    size_t i;

    if (!IGSYAJL_IS_ARRAY(v)) return;

    for (i = 0; i < v->u.array.len; i++)
    {
        igsyajl_tree_free (v->u.array.values[i]);
        v->u.array.values[i] = NULL;
    }

    free(v->u.array.values);
    free(v);
}

/*
 * Parsing nested objects and arrays is implemented using a stack. When a new
 * object or array starts (a curly or a square opening bracket is read), an
 * appropriate value is pushed on the stack. When the end of the object is
 * reached (an appropriate closing bracket has been read), the value is popped
 * off the stack and added to the enclosing object using "context_add_value".
 */
static int context_push(context_t *ctx, igsyajl_val v)
{
    stack_elem_t *stack;

    stack = malloc (sizeof (*stack));
    if (stack == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    memset (stack, 0, sizeof (*stack));

    assert ((ctx->stack == NULL)
            || IGSYAJL_IS_OBJECT (v)
            || IGSYAJL_IS_ARRAY (v));

    stack->value = v;
    stack->next = ctx->stack;
    ctx->stack = stack;

    return (0);
}

static igsyajl_val context_pop(context_t *ctx)
{
    stack_elem_t *stack;
    igsyajl_val v;

    if (ctx->stack == NULL)
        RETURN_ERROR (ctx, NULL, "context_pop: "
                      "Bottom of stack reached prematurely");

    stack = ctx->stack;
    ctx->stack = stack->next;

    v = stack->value;

    free (stack);

    return (v);
}

static int object_add_keyval(context_t *ctx,
                             igsyajl_val obj, char *key, igsyajl_val value)
{
    const char **tmpk;
    igsyajl_val *tmpv;

    /* We're checking for NULL in "context_add_value" or its callers. */
    assert (ctx != NULL);
    assert (obj != NULL);
    assert (key != NULL);
    assert (value != NULL);

    /* We're assuring that "obj" is an object in "context_add_value". */
    assert(IGSYAJL_IS_OBJECT(obj));

    tmpk = realloc((void *) obj->u.object.keys, sizeof(*(obj->u.object.keys)) * (obj->u.object.len + 1));
    if (tmpk == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    obj->u.object.keys = tmpk;

    tmpv = realloc(obj->u.object.values, sizeof (*obj->u.object.values) * (obj->u.object.len + 1));
    if (tmpv == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    obj->u.object.values = tmpv;

    obj->u.object.keys[obj->u.object.len] = key;
    obj->u.object.values[obj->u.object.len] = value;
    obj->u.object.len++;

    return (0);
}

static int array_add_value (context_t *ctx,
                            igsyajl_val array, igsyajl_val value)
{
    igsyajl_val *tmp;

    /* We're checking for NULL pointers in "context_add_value" or its
     * callers. */
    assert (ctx != NULL);
    assert (array != NULL);
    assert (value != NULL);

    /* "context_add_value" will only call us with array values. */
    assert(IGSYAJL_IS_ARRAY(array));

    tmp = realloc(array->u.array.values,
                  sizeof(*(array->u.array.values)) * (array->u.array.len + 1));
    if (tmp == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    array->u.array.values = tmp;
    array->u.array.values[array->u.array.len] = value;
    array->u.array.len++;

    return 0;
}

/*
 * Add a value to the value on top of the stack or the "root" member in the
 * context if the end of the parsing process is reached.
 */
static int context_add_value (context_t *ctx, igsyajl_val v)
{
    /* We're checking for NULL values in all the calling functions. */
    assert (ctx != NULL);
    assert (v != NULL);

    /*
     * There are three valid states in which this function may be called:
     *   - There is no value on the stack => This is the only value. This is the
     *     last step done when parsing a document. We assign the value to the
     *     "root" member and return.
     *   - The value on the stack is an object. In this case store the key on the
     *     stack or, if the key has already been read, add key and value to the
     *     object.
     *   - The value on the stack is an array. In this case simply add the value
     *     and return.
     */
    if (ctx->stack == NULL)
    {
        assert (ctx->root == NULL);
        ctx->root = v;
        return (0);
    }
    else if (IGSYAJL_IS_OBJECT (ctx->stack->value))
    {
        if (ctx->stack->key == NULL)
        {
            if (!IGSYAJL_IS_STRING (v))
                RETURN_ERROR (ctx, EINVAL, "context_add_value: "
                              "Object key is not a string (%#04x)",
                              v->type);

            ctx->stack->key = v->u.string;
            v->u.string = NULL;
            free(v);
            return (0);
        }
        else /* if (ctx->key != NULL) */
        {
            char * key;

            key = ctx->stack->key;
            ctx->stack->key = NULL;
            return (object_add_keyval (ctx, ctx->stack->value, key, v));
        }
    }
    else if (IGSYAJL_IS_ARRAY (ctx->stack->value))
    {
        return (array_add_value (ctx, ctx->stack->value, v));
    }
    else
    {
        RETURN_ERROR (ctx, EINVAL, "context_add_value: Cannot add value to "
                      "a value of type %#04x (not a composite type)",
                      ctx->stack->value->type);
    }
}

static int handle_string (void *ctx,
                          const unsigned char *string, size_t string_length)
{
    igsyajl_val v;

    v = value_alloc (igsyajl_t_string);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.string = malloc (string_length + 1);
    if (v->u.string == NULL)
    {
        free (v);
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.string, string, string_length);
    v->u.string[string_length] = 0;

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_number (void *ctx, const char *string, size_t string_length)
{
    igsyajl_val v;
    char *endptr;

    v = value_alloc(igsyajl_t_number);
    if (v == NULL)
        RETURN_ERROR((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.number.r = malloc(string_length + 1);
    if (v->u.number.r == NULL)
    {
        free(v);
        RETURN_ERROR((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.number.r, string, string_length);
    v->u.number.r[string_length] = 0;

    v->u.number.flags = 0;

    errno = 0;
    v->u.number.i = igsyajl_parse_integer((const unsigned char *) v->u.number.r,
                                       strlen(v->u.number.r));
    if (errno == 0)
        v->u.number.flags |= IGSYAJL_NUMBER_INT_VALID;

    endptr = NULL;
    errno = 0;
    v->u.number.d = strtod(v->u.number.r, &endptr);
    if ((errno == 0) && (endptr != NULL) && (*endptr == 0))
        v->u.number.flags |= IGSYAJL_NUMBER_DOUBLE_VALID;

    return ((context_add_value(ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_map (void *ctx)
{
    igsyajl_val v;

    v = value_alloc(igsyajl_t_object);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.object.keys = NULL;
    v->u.object.values = NULL;
    v->u.object.len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_map (void *ctx)
{
    igsyajl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_array (void *ctx)
{
    igsyajl_val v;

    v = value_alloc(igsyajl_t_array);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.array.values = NULL;
    v->u.array.len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_array (void *ctx)
{
    igsyajl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_boolean (void *ctx, int boolean_value)
{
    igsyajl_val v;

    v = value_alloc (boolean_value ? igsyajl_t_true : igsyajl_t_false);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_null (void *ctx)
{
    igsyajl_val v;

    v = value_alloc (igsyajl_t_null);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

/*
 * Public functions
 */
igsyajl_val igsyajl_tree_parse (const char *input,
                          char *error_buffer, size_t error_buffer_size)
{
    static const igsyajl_callbacks callbacks =
        {
            /* null        = */ handle_null,
            /* boolean     = */ handle_boolean,
            /* integer     = */ NULL,
            /* double      = */ NULL,
            /* number      = */ handle_number,
            /* string      = */ handle_string,
            /* start map   = */ handle_start_map,
            /* map key     = */ handle_string,
            /* end map     = */ handle_end_map,
            /* start array = */ handle_start_array,
            /* end array   = */ handle_end_array
        };

    igsyajl_handle handle;
    igsyajl_status status;
    char * internal_err_str;
	context_t ctx = { NULL, NULL, NULL, 0 };

	ctx.errbuf = error_buffer;
	ctx.errbuf_size = error_buffer_size;

    if (error_buffer != NULL)
        memset (error_buffer, 0, error_buffer_size);

    handle = igsyajl_alloc (&callbacks, NULL, &ctx);
    igsyajl_config(handle, igsyajl_allow_comments, 1);
    igsyajl_config(handle, igsyajl_allow_trailing_garbage, 1);

    status = igsyajl_parse(handle,
                        (unsigned char *) input,
                        strlen (input));
    status = igsyajl_complete_parse (handle);
    if (status != igsyajl_status_ok) {
        if (error_buffer != NULL && error_buffer_size > 0) {
               internal_err_str = (char *) igsyajl_get_error(handle, 1,
                     (const unsigned char *) input,
                     strlen(input));
             snprintf(error_buffer, error_buffer_size, "%s", internal_err_str);
             YA_FREE(&(handle->alloc), internal_err_str);
        }
        igsyajl_free (handle);
        return NULL;
    }

    igsyajl_free (handle);
    return (ctx.root);
}

igsyajl_val igsyajl_tree_get(igsyajl_val n, const char ** path, igsyajl_type type)
{
    if (!path) return NULL;
    while (n && *path) {
        size_t i;
        size_t len;
        if (n->type == igsyajl_t_object){
            len = n->u.object.len;
            for (i = 0; i < len; i++) {
                if (!strcmp(*path, n->u.object.keys[i])) {
                    n = n->u.object.values[i];
                    break;
                }
            }
            if (i == len) return NULL;
        } else if (n->type == igsyajl_t_array){
            len = n->u.array.len;
            char *end = (char *)*path;
            unsigned long index = (unsigned long) strtol(*path, &end, 10);
            if (end != *path && index >= 0){
                //we got a valid index
                if (index < len){
                    //we are in range
                    n = n->u.array.values[index];
                }else{
                    //out of range
                    return NULL;
                }
            }else{
                //invalid index format
                return NULL;
            }
        }
        path++;
    }
    if (n && type != igsyajl_t_any && type != n->type) n = NULL;
    return n;
}

void igsyajl_tree_free (igsyajl_val v)
{
    if (v == NULL) return;

    if (IGSYAJL_IS_STRING(v))
    {
        free(v->u.string);
        free(v);
    }
    else if (IGSYAJL_IS_NUMBER(v))
    {
        free(v->u.number.r);
        free(v);
    }
    else if (IGSYAJL_GET_OBJECT(v))
    {
        igsyajl_object_free(v);
    }
    else if (IGSYAJL_GET_ARRAY(v))
    {
        igsyajl_array_free(v);
    }
    else /* if (igsyajl_t_true or igsyajl_t_false or igsyajl_t_null) */
    {
        free(v);
    }
}
