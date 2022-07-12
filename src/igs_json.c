/*  =========================================================================
    json - JSON API

    Copyright (c) 2021 Ingenuity io

    This file is part of the Ingescape library. See https://ingescape.com.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    igs_json - JSON framework
@discuss
@end
*/

#include "ingescape_classes.h"
#include "yajl_parse.h"
#include "yajl_gen.h"

//  Structure of our class
//defined as an alias to igsyajl_gen

#define JSON_MAX_BUFFER_SIZE 65536

typedef struct json_parsing_elements
{
    igsyajl_handle handle;
    void *my_data;
    igs_json_fn *cb;
    unsigned char buffer[JSON_MAX_BUFFER_SIZE];
} json_parsing_elements_t;

void
s_igs_json_error (igsyajl_gen_status status,
                  const char *function,
                  void *value)
{
    IGS_UNUSED (value)
    const char *output = NULL;
    switch (status) {
        case igsyajl_gen_keys_must_be_strings:
            output = "a JSON key must be a string";
            break;
        case igsyajl_max_depth_exceeded:
            output = "maximum generation depth was exceeded";
            break;
        case igsyajl_gen_in_error_state:
            output =
              "impossible to add new elements while in error state (correct "
              "error first)";
            break;
        case igsyajl_gen_generation_complete:
            output = "a complete JSON document has been generated";
            break;
        case igsyajl_gen_invalid_number:
            output = "an invalid floating point value has been passed";
            break;
        case igsyajl_gen_no_buf:
            output =
              "a print callback was passed in, so there is no internal buffer "
              "to get from";
            break;
        case igsyajl_gen_invalid_string:
            output = "an invalid string was passed";
            break;

        default:
            break;
    }
    igs_error ("%s - %s", function, output);
}

void
s_json_free_parsing_elements (json_parsing_elements_t **e)
{
    if (*e) {
        if ((*e)->handle)
            igsyajl_free ((*e)->handle);
        free (*e);
        *e = NULL;
    }
}

static int
s_json_null (void *ctx)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    e->cb (IGS_JSON_NULL, NULL, 0, e->my_data);
    return 1;
}

static int
s_json_boolean (void *ctx, int boolean)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    e->cb (IGS_JSON_BOOL, &boolean, sizeof (int), e->my_data);
    return 1;
}

static int
s_json_number (void *ctx, const char *string_val, size_t string_len)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    char *tmp = (char *) zmalloc (string_len + 1);
    strncpy (tmp, string_val, string_len);
    e->cb (IGS_JSON_NUMBER, tmp, string_len, e->my_data);
    free (tmp);
    return 1;
}

static int
s_json_string (void *ctx, const unsigned char *string_val, size_t string_len)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    char *tmp = (char *) zmalloc (string_len + 1);
    strncpy (tmp, (char *) string_val, string_len);
    e->cb (IGS_JSON_STRING, tmp, string_len, e->my_data);
    free (tmp);
    return 1;
}

static int
s_json_map_key (void *ctx, const unsigned char *string_val, size_t string_len)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    char *tmp = (char *) zmalloc (string_len + 1);
    strncpy (tmp, (char *) string_val, string_len);
    e->cb (IGS_JSON_KEY, tmp, string_len, e->my_data);
    free (tmp);
    return 1;
}

static int
s_json_start_map (void *ctx)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    e->cb (IGS_JSON_MAP, NULL, 0, e->my_data);
    return 1;
}

static int
s_json_end_map (void *ctx)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    e->cb (IGS_JSON_MAP_END, NULL, 0, e->my_data);
    return 1;
}

static int
s_json_start_array (void *ctx)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    e->cb (IGS_JSON_ARRAY, NULL, 0, e->my_data);
    return 1;
}

static int
s_json_end_array (void *ctx)
{
    json_parsing_elements_t *e = (json_parsing_elements_t *) ctx;
    e->cb (IGS_JSON_ARRAY_END, NULL, 0, e->my_data);
    return 1;
}

static igsyajl_callbacks json_callbacks = {
    s_json_null,    s_json_boolean,     NULL,           NULL,
    s_json_number,  s_json_string,      s_json_start_map, s_json_map_key,
    s_json_end_map, s_json_start_array, s_json_end_array};

//  --------------------------------------------------------------------------
//  Create a new igs_json

igs_json_t *
igs_json_new (void)
{
    igsyajl_gen_t *g = igsyajl_gen_alloc (NULL);
    assert(g);
    igsyajl_gen_config (g, igsyajl_gen_beautify, 1);
    igsyajl_gen_config (g, igsyajl_gen_validate_utf8, 1);
    return (igs_json_t*) g;
}


//  --------------------------------------------------------------------------
//  Destroy the igs_json

void
igs_json_destroy (igs_json_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        igsyajl_gen_t *g = (igsyajl_gen_t *) *self_p;
        igsyajl_gen_free (g);
        *self_p = NULL;
    }
}

void
igs_json_open_map (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_map_open (g);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, NULL);
}

void
igs_json_close_map (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_map_close (g);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, NULL);
}

void
igs_json_open_array (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_array_open (g);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, NULL);
}

void
igs_json_close_array (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_array_close (g);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, NULL);
}

void
igs_json_add_null (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_null (g);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, NULL);
}

void
igs_json_add_bool (igs_json_t *json, bool value)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_bool (g, value);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, &value);
}

void
igs_json_add_int (igs_json_t *json, int64_t value)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_integer (g, value);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, &value);
}

void
igs_json_add_double (igs_json_t *json, double value)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status = igsyajl_gen_double (g, value);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, &value);
}

void
igs_json_add_string (igs_json_t *json, const char *value)
{
    assert(json);
    if (value == NULL)
        value = "";
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    igsyajl_gen_status status =
      igsyajl_gen_string (g, (unsigned const char *) value, strlen (value));
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, &value);
}

void
igs_json_print (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    const unsigned char *buffer = NULL;
    size_t buffer_size = 0;
    igsyajl_gen_status status = igsyajl_gen_get_buf (g, &buffer, &buffer_size);
    if (status != igsyajl_gen_status_ok)
        s_igs_json_error (status, __func__, NULL);
    else
        printf ("%s\n", buffer);
}

char *
igs_json_dump (igs_json_t *json)
{
    assert(json);
    igsyajl_gen_t *g = (struct igsyajl_gen *) json;
    const unsigned char *buffer = NULL;
    size_t buffer_size = 0;
    igsyajl_gen_status status = igsyajl_gen_get_buf (g, &buffer, &buffer_size);
    if (status != igsyajl_gen_status_ok) {
        s_igs_json_error (status, __func__, NULL);
        return NULL;
    }
    return strdup ((char *) buffer);
}

char *
igs_json_compact_dump (igs_json_t *json)
{
    assert(json);
    igs_json_t *dup = igs_json_new ();
    igsyajl_gen_config ((igsyajl_gen_t *)dup, igsyajl_gen_beautify, 0);
    igs_json_node_t *node = igs_json_node_for (json);
    igs_json_insert_node (dup, node);
    igs_json_node_destroy (&node);
    char *pretty_dump = igs_json_dump (dup);
    igs_json_destroy (&dup);
    return pretty_dump;
}

void
igs_json_parse_from_file (const char *path, igs_json_fn cb, void *my_data)
{
    assert(path);
    assert(cb);
    FILE *fp = NULL;
    if (path)
        fp = fopen (path, "r");
    if (!fp) {
        igs_error ("could not open %s", path);
        return;
    }

    json_parsing_elements_t *elements =
      (json_parsing_elements_t *) zmalloc (sizeof (json_parsing_elements_t));
    igsyajl_handle handle = igsyajl_alloc (&json_callbacks, NULL, elements);
    igsyajl_config (handle, igsyajl_allow_trailing_garbage, 1);
    elements->handle = handle;
    elements->my_data = my_data;
    elements->cb = cb;

    igsyajl_status status;
    while (1) {
        elements->buffer[0] = 0;
        fgets ((char *) elements->buffer, sizeof (elements->buffer) - 1, fp);
        if (!feof (fp) && strlen ((char *) elements->buffer) == 0) {
            igs_error ("could not read %s", path);
            break;
        }
        // printf("reading: %s\n", elements->buffer);
        status = igsyajl_parse (handle, elements->buffer,
                                strlen ((char *) (elements->buffer)));
        if (status != igsyajl_status_ok || feof (fp))
            break;
    }
    status = igsyajl_complete_parse (handle);

    if (status != igsyajl_status_ok) {
        unsigned char *str = igsyajl_get_error (
          handle, 1, elements->buffer, strlen ((char *) elements->buffer));
        igs_error ("%s", str);
        igsyajl_free_error (handle, str);
    }

    fclose (fp);
    s_json_free_parsing_elements (&elements);
}

void
igs_json_parse_from_str (const char *content,
                         igs_json_fn cb,
                         void *my_data)
{
    assert(content);
    assert(cb);

    json_parsing_elements_t *elements =
      (json_parsing_elements_t *) zmalloc (sizeof (json_parsing_elements_t));
    igsyajl_handle handle = igsyajl_alloc (&json_callbacks, NULL, elements);
    igsyajl_config (handle, igsyajl_allow_trailing_garbage, 1);
    elements->handle = handle;
    elements->my_data = my_data;
    elements->cb = cb;

    igsyajl_status status;
    igsyajl_parse (handle, (const unsigned char *) content, strlen (content));
    status = igsyajl_complete_parse (handle);

    if (status != igsyajl_status_ok) {
        unsigned char *str = igsyajl_get_error (
          handle, 1, elements->buffer, strlen ((char *) elements->buffer));
        igs_error ("%s", str);
        igsyajl_free_error (handle, str);
    }
    s_json_free_parsing_elements (&elements);
}

void
igs_json_insert_node (igs_json_t *json, igs_json_node_t *node)
{
    assert(json);
    assert(node);
    switch (node->type) {
        case IGS_JSON_STRING:
            igs_json_add_string (json, node->u.string);
            break;
        case IGS_JSON_NUMBER:
            if (igs_json_node_is_integer (node))
                igs_json_add_int (json, node->u.number.i);
            else
                igs_json_add_double (json, node->u.number.d);
            break;
        case IGS_JSON_MAP: // igsyajl_t_object
            igs_json_open_map (json);
            for (size_t i = 0; i < node->u.object.len; i++) {
                igs_json_add_string (json, node->u.object.keys[i]);
                igs_json_insert_node (json, node->u.object.values[i]);
            }
            igs_json_close_map (json);
            break;
        case IGS_JSON_ARRAY: // igsyajl_t_array
            igs_json_open_array (json);
            for (size_t i = 0; i < node->u.array.len; i++) {
                igs_json_insert_node (json, node->u.array.values[i]);
            }
            igs_json_close_array (json);
            break;
        case IGS_JSON_TRUE:
            igs_json_add_bool (json, true);
            break;
        case IGS_JSON_FALSE:
            igs_json_add_bool (json, false);
            break;
        case IGS_JSON_NULL:
            igs_json_add_null (json);
            break;
        default:
            break;
    }
}

igs_json_node_t *
igs_json_node_for (igs_json_t *json)
{
    assert(json);
    char *dump = igs_json_dump (json);
    igs_json_node_t *res = igs_json_node_parse_from_str (dump);
    free (dump);
    return res;
}

//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

int s_igs_json_test_indent = 0;
void
s_igs_json_test_parsing_callback (igs_json_value_type_t type,
                                  void *value,
                                  size_t size,
                                  void *my_data)
{
    IGS_UNUSED(size);
    IGS_UNUSED(my_data);

    switch (type) {
        case IGS_JSON_NULL:
            printf ("null\n");
            break;
        case IGS_JSON_BOOL:
            printf ("%s\n", (*(bool *) value) ? "true" : "false");
            break;
        case IGS_JSON_NUMBER:
            printf ("%s\n", (char *) value);
            break;
        case IGS_JSON_STRING:
            printf ("%s\n", (char *) value);
            break;
        case IGS_JSON_KEY:
            for (int i = 0; i < s_igs_json_test_indent; i++) {
                printf ("  ");
            }
            printf ("K:%s : ", (char *) value);
            break;
        case IGS_JSON_MAP:
            printf ("\n");
            for (int i = 0; i < s_igs_json_test_indent; i++) {
                printf ("  ");
            }
            printf ("{\n");
            s_igs_json_test_indent++;
            break;
        case IGS_JSON_MAP_END:
            s_igs_json_test_indent--;
            for (int i = 0; i < s_igs_json_test_indent; i++) {
                printf ("  ");
            }
            printf ("}\n");
            break;
        case IGS_JSON_ARRAY:
            printf ("\n");
            for (int i = 0; i < s_igs_json_test_indent; i++) {
                printf ("  ");
            }
            printf ("[\n");
            s_igs_json_test_indent++;
            break;
        case IGS_JSON_ARRAY_END:
            s_igs_json_test_indent--;
            for (int i = 0; i < s_igs_json_test_indent; i++) {
                printf ("  ");
            }
            printf ("]\n");
            break;

        default:
            break;
    }
}
void
igs_json_test (bool verbose)
{
    IGS_UNUSED(verbose)
    printf (" * igs_json: ");

    //  @selftest
    //  Simple create/destroy test
    igs_json_t *json = igs_json_new ();
    igs_json_open_map (json);
    {
        igs_json_add_int (json, (long long) -15.15);
        igs_json_add_int (json, (long long) -15.15);
        igs_json_add_string (json, "null");
        igs_json_add_null (json);
        igs_json_add_string (json, "bool");
        igs_json_add_bool (json, -1);
        igs_json_add_string (json, "int");
        igs_json_add_int (json, (long long) -15.15);
        igs_json_add_string (json, "double");
        igs_json_add_double (json, 002.500);
        igs_json_add_string (json, "UTF8");
        igs_json_add_string (json, "˻");
        igs_json_add_string (json, "definition");
        igs_json_open_map (json);
        {
            igs_json_add_string (json, "name");
            igs_json_add_string (json, "igsDDS");
            igs_json_add_string (json, "description");
            igs_json_add_string (json,
                                 "dynamic definition for DDS storage agent");
            igs_json_add_string (json, "version");
            igs_json_add_string (json, "dynamic");
            igs_json_add_string (json, "parameters");
            igs_json_open_array (json);
            {
            }
            igs_json_close_array (json);
            igs_json_add_string (json, "inputs");
            igs_json_open_array (json);
            {
                igs_json_open_map (json);
                {
                    igs_json_add_string (json, "name");
                    igs_json_add_string (json, "my_variable");
                    igs_json_add_string (json, "type");
                    igs_json_add_string (json, "INTEGER");
                    igs_json_add_string (json, "value");
                    igs_json_add_string (json, "");
                }
                igs_json_close_map (json);
            }
            igs_json_close_array (json);
            igs_json_add_string (json, "outputs");
            igs_json_open_array (json);
            {
            }
            igs_json_close_array (json);
            igs_json_add_string (json, "services");
            igs_json_open_array (json);
            {
                igs_json_open_map (json);
                {
                    igs_json_add_string (json, "name");
                    igs_json_add_string (json, "add_variable");
                    igs_json_add_string (json, "arguments");
                    igs_json_open_array (json);
                    {
                        igs_json_open_map (json);
                        {
                            igs_json_add_string (json, "name");
                            igs_json_add_string (json, "name");
                            igs_json_add_string (json, "type");
                            igs_json_add_string (json, "STRING");
                        }
                        igs_json_close_map (json);
                        igs_json_open_map (json);
                        {
                            igs_json_add_string (json, "name");
                            igs_json_add_string (json, "type");
                            igs_json_add_string (json, "type");
                            igs_json_add_string (json, "STRING");
                        }
                        igs_json_close_map (json);
                    }
                    igs_json_close_array (json);
                }
                igs_json_close_map (json);
                igs_json_open_map (json);
                {
                    igs_json_add_string (json, "name");
                    igs_json_add_string (json, "remove_variable");
                    igs_json_add_string (json, "arguments");
                    igs_json_open_array (json);
                    {
                        igs_json_open_map (json);
                        {
                            igs_json_add_string (json, "name");
                            igs_json_add_string (json, "name");
                            igs_json_add_string (json, "type");
                            igs_json_add_string (json, "STRING");
                        }
                        igs_json_close_map (json);
                    }
                    igs_json_close_array (json);
                }
                igs_json_close_map (json);
            }
            igs_json_close_array (json);
        }
        igs_json_close_map (json);
    }
    igs_json_close_map (json);
    igs_json_add_string (json, "bool");
    igs_json_add_bool (json, -1);
    igs_json_print (json);
    printf ("\n\n***************\n\n");

    char plop[] = "plop";
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    printf ("%s\n\n***************\n\n", res);
    igs_json_parse_from_str (res, s_igs_json_test_parsing_callback, plop);
    free (res);

    printf ("\n\n***************\n\n");

    igs_json_parse_from_file (
      "/users/steph/documents/inge_scape/agents/igsdds_definition.json",
      s_igs_json_test_parsing_callback, plop);
    //  @end
    printf ("OK\n");
}
