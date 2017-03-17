//
//  kvsimple.h
//  Mastic
//
//  Created by Vincent Deliencourt on 26/08/2016.
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef kvsimple_h
#define kvsimple_h

#include <stdio.h>

#include <zmq.h>
#include <zyre.h>
#include <zyre_event.h>

//  Keys are short strings
#define KVMSG_KEY_MAX   255

#define KVMSG_BODY_MAX   255

//  Message is formatted on wire as 4 frames:
//  frame 0: key (0MQ string)
//  frame 1: sequence (8 bytes, network order)
//  frame 2: body (blob)
#define FRAME_KEY       0
#define FRAME_SEQ       1
#define FRAME_BODY      2
#define KVMSG_FRAMES    3

//  The kvmsg class holds a single key-value message consisting of a
//  list of 0 or more frames:

typedef struct _kvmsg {
    //  Presence indicators for each frame
    int present [KVMSG_FRAMES];
    //  Corresponding 0MQ message frames, if any
    zmq_msg_t frame [KVMSG_FRAMES];
    //  Key, copied into safe C string
    char key [KVMSG_KEY_MAX + 1];
} kvmsg_t;

// Contructors and destructors
kvmsg_t *kvmsg_new (int64_t sequence);
void kvmsg_free (void *ptr);
void kvmsg_destroy (kvmsg_t **self_p);

// Connection methods
kvmsg_t * kvmsg_recv (void *socket);
void kvmsg_send (kvmsg_t *self, void *socket);

// Key field management
char * kvmsg_key (kvmsg_t *self);
void kvmsg_set_key (kvmsg_t *self, char *key);
void kvmsg_fmt_key (kvmsg_t *self, char *format, ...);

// Sequence field management
int64_t kvmsg_sequence (kvmsg_t *self);
void kvmsg_set_sequence (kvmsg_t *self, int64_t sequence);

// Body field management
byte * kvmsg_body (kvmsg_t *self);
void kvmsg_set_body (kvmsg_t *self, byte *body, size_t size);
void kvmsg_fmt_body (kvmsg_t *self, char *format, ...);
size_t kvmsg_size (kvmsg_t *self);

// Data management
void kvmsg_store (kvmsg_t **self_p, zhash_t *hash);
void kvmsg_dump (kvmsg_t *self);
int kvmsg_test (int verbose);



#endif /* kvsimple_h */
