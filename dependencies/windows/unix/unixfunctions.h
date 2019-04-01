
#ifndef ingescape_unix_functions_h
#define ingescape_unix_functions_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX
#include <windows.h>
#include <winsock2.h>

char *strndup(const char *str, size_t chars);

int gettimeofday(struct timeval* p, void* tz);


#define pthread_mutex_t HANDLE

int pthread_mutex_init(pthread_mutex_t* mutex);
int pthread_mutex_lock(pthread_mutex_t mutex);
int pthread_mutex_unlock(pthread_mutex_t mutex);

typedef struct DIR DIR;

struct dirent
{
    char *d_name;
};

DIR           *opendir(const char *);
int           closedir(DIR *);
struct dirent *readdir(DIR *);
void          rewinddir(DIR *);

/*
 
 Copyright Kevlin Henney, 1997, 2003. All rights reserved.
 
 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose is hereby granted without fee, provided
 that this copyright and permissions notice appear in all copies and
 derivatives.
 
 This software is supplied "as is" without express or implied warranty.
 
 But that said, if there are any problems please get in touch.
 
 */

#endif
