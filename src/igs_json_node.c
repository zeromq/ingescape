/*  =========================================================================
    json_node - JSON node api

    Copyright (c) 2021 Ingenuity io

    This file is part of the Ingescape library. See https://ingescape.com.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    igs_json_node - JSON framework - tree node api that can be queried
@discuss
@end
*/

#include "ingescape_classes.h"
#include "yajl_tree.h"

void
s_json_node_iterate (igs_json_t *json, igs_json_node_t *value)
{
    assert(json);
    assert(value);
    switch (value->type) {
        case IGS_JSON_STRING:
            igs_json_add_string (json, value->u.string);
            break;
        case IGS_JSON_NUMBER:
            if (igs_json_node_is_integer (value))
                igs_json_add_int (json, value->u.number.i);
            else
                igs_json_add_double (json, value->u.number.d);
            break;
        case IGS_JSON_MAP: // igsyajl_t_object
            igs_json_open_map (json);
            for (size_t i = 0; i < value->u.object.len; i++) {
                igs_json_add_string (json, value->u.object.keys[i]);
                s_json_node_iterate (json, value->u.object.values[i]);
            }
            igs_json_close_map (json);
            break;
        case IGS_JSON_ARRAY: // igsyajl_t_array
            igs_json_open_array (json);
            for (size_t i = 0; i < value->u.array.len; i++) {
                s_json_node_iterate (json, value->u.array.values[i]);
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


//  --------------------------------------------------------------------------
//  Create a new igs_json_node

igs_json_node_t *
igs_json_node_new (void)
{
    igs_json_node_t *self = (igs_json_node_t *) zmalloc (sizeof (igs_json_node_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the igs_json_node

void
igs_json_node_destroy (igs_json_node_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        igs_json_node_t *self = *self_p;
        igsyajl_tree_free ((igsyajl_val)self);
        *self_p = NULL;
    }
}

igs_json_node_t *
igs_json_node_parse_from_file (const char *path)
{
    assert (path);
    zfile_t *file = zfile_new (NULL, path);
    if (file == NULL || !zfile_is_regular (file) || !zfile_is_readable (file)
        || zfile_input (file) != 0) {
        if (!zfile_is_regular (file))
            igs_error ("not a regular file : %s", path);
        if (!zfile_is_readable (file))
            igs_error ("not readable file : %s", path);
        if (zfile_input (file) != 0)
            igs_error ("could not open %s", path);
        return NULL;
    }
    char errbuf[1024] = "unknown error";
    zchunk_t *data = zfile_read (file, zfile_size (path), 0);
    igs_json_node_t *node = (igs_json_node_t *) igsyajl_tree_parse (
      (const char *) zchunk_data (data), errbuf, sizeof (errbuf));
    if (node == NULL)
        igs_error ("parsing error (%s) : %s", path, errbuf);
    zchunk_destroy (&data);
    zfile_destroy (&file);
    return node;
}

igs_json_node_t *
igs_json_node_parse_from_str (const char *content)
{
    assert (content);
    char errbuf[1024] = "unknown error";
    igs_json_node_t *node =
      (igs_json_node_t *) igsyajl_tree_parse (content, errbuf, sizeof (errbuf));
    if (node == NULL)
        igs_error ("parsing error (%s) : %s", content, errbuf);
    return node;
}

igs_json_node_t *
igs_json_node_dup (igs_json_node_t *root)
{
    assert(root);
    igs_json_node_t *result =
      (igs_json_node_t *) zmalloc (sizeof (igs_json_node_t));
    result->type = root->type;

    switch (root->type) {
        case IGS_JSON_STRING:
            result->u.string = strdup (root->u.string);
            break;
        case IGS_JSON_NUMBER:
            if (igs_json_node_is_integer (root))
                result->u.number.i = root->u.number.i;
            else
                result->u.number.d = root->u.number.d;
            if (root->u.number.r)
                result->u.number.r = strdup (root->u.number.r);
            result->u.number.flags = root->u.number.flags;
            break;
        case IGS_JSON_MAP: // igsyajl_t_object
            result->u.object.len = root->u.object.len;
            result->u.object.keys =
              (const char **) zmalloc (root->u.object.len * sizeof (char *));
            result->u.object.values =
              (igs_json_node_t **) zmalloc (root->u.object.len * sizeof (igs_json_node_t *));
            for (size_t i = 0; i < root->u.object.len; i++) {
                result->u.object.keys[i] = strdup (root->u.object.keys[i]);
                result->u.object.values[i] =
                  igs_json_node_dup (root->u.object.values[i]);
            }
            break;
        case IGS_JSON_ARRAY: // igsyajl_t_array
            result->u.array.len = root->u.array.len;
            result->u.array.values =
              (igs_json_node_t **) zmalloc (root->u.array.len * sizeof (igs_json_node_t *));
            for (size_t i = 0; i < root->u.array.len; i++) {
                result->u.array.values[i] =
                igs_json_node_dup (root->u.array.values[i]);
            }
            break;
        case IGS_JSON_TRUE:
            result->u.number.i = true;
            if (root->u.number.r)
                result->u.number.r = strdup (root->u.number.r);
            result->u.number.flags = root->u.number.flags;
            break;
        case IGS_JSON_FALSE:
            result->u.number.i = false;
            if (root->u.number.r)
                result->u.number.r = strdup (root->u.number.r);
            result->u.number.flags = root->u.number.flags;
            break;
        case IGS_JSON_NULL:
            break;
        default:
            break;
    }
    return result;
}

char *
igs_json_node_dump (igs_json_node_t *value)
{
    assert(value);
    igs_json_t *json = igs_json_new ();
    s_json_node_iterate (json, value);
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}

void
igs_json_node_insert (igs_json_node_t *parent,
                      const char *key,
                      igs_json_node_t *node_to_insert)
{
    assert (parent);
    assert (node_to_insert);
    if (parent->type == IGS_JSON_ARRAY) {
        size_t size = parent->u.array.len;
        parent->u.array.values = realloc (parent->u.array.values, size + 1);
        assert (parent->u.array.values);
        parent->u.array.values[size] = igs_json_node_dup (node_to_insert);
        parent->u.array.len += 1;
    }
    else
    if (parent->type == IGS_JSON_MAP) {
        assert (key);
        size_t size = parent->u.object.len;
        bool known_key = false;
        size_t index = size;
        for (size_t i = 0; i < size; i++) {
            if (streq (parent->u.object.keys[i], key)) {
                known_key = true;
                index = i;
                break;
            }
        }
        if (known_key)
            igs_json_node_destroy (&(parent->u.object.values[index]));
        else {
            parent->u.object.values =
              realloc (parent->u.object.values, size + 1);
            parent->u.object.keys =
              (const char **) realloc (parent->u.object.keys, size + 1);
            assert (parent->u.object.values);
            assert (parent->u.object.keys);
            parent->u.object.keys[index] = strdup (key);
            parent->u.object.len += 1;
        }
        parent->u.object.values[index] = igs_json_node_dup (node_to_insert);
    }
    else
        igs_error ("parent node must be an array or a map");
}


igs_json_node_t *
igs_json_node_find (igs_json_node_t *node, const char **path)
{
    assert(node);
    if (node == NULL) {
        igs_warn ("passed node is NULL");
        return NULL;
    }
    igsyajl_val v = igsyajl_tree_get ((igsyajl_val) node, path, igsyajl_t_any);
    return (igs_json_node_t *) v;
}

bool
igs_json_node_is_integer (igs_json_node_t *value)
{
    assert(value);
    return IGSYAJL_IS_INTEGER ((igsyajl_val) value);
}

bool
igs_json_node_is_double (igs_json_node_t *value)
{
    assert(value);
    return IGSYAJL_IS_DOUBLE ((igsyajl_val) value);
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

void
igs_json_node_test (bool verbose)
{
    IGS_UNUSED(verbose)
    printf (" * igs_json_node: ");

    //  @selftest
    //  Simple create/destroy test
//    igs_json_node_t *node = igs_json_node_parse_from_file (
//      "/users/steph/documents/inge_scape/agents/philips.json");
//
//    const char *path[] = {"lights", "1", "state", "xy", NULL};
//    igs_json_node_t *value = igs_json_node_find (node, path);
//    printf("is node double ? %s\n", (igs_json_node_is_double (value)? "yes": "no"));
//    printf("is node integer ? %s\n", (igs_json_node_is_integer (value)? "yes": "no"));
//    igs_json_node_destroy (&value);
//    igs_json_node_destroy (&node);
    //  @end
    printf ("OK\n");
}
