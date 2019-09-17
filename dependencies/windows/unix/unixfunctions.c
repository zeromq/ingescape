
#include <unixfunctions.h>
#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno iff they return -1 */
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef ptrdiff_t handle_type; /* C99's intptr_t not sufficiently portable */

struct DIR
{
    handle_type         handle; /* -1 for failed rewind */
    struct _finddata_t  info;
    struct dirent       result; /* d_name null iff first time */
    char                *name;  /* null-terminated char string */
};

char *strndup(const char *str, size_t chars)
{
    char *buffer;
    size_t n;

    buffer = (char *) malloc(chars +1);
    if (buffer)
    {
        for (n = 0; ((n < chars) && (str[n] != 0)) ; n++) buffer[n] = str[n];
        buffer[n] = 0;
    }

    return buffer;
}


int gettimeofday(struct timeval* p, void* tz) {
	ULARGE_INTEGER ul; // As specified on MSDN.
	FILETIME ft;

	// Returns a 64-bit value representing the number of
	// 100-nanosecond intervals since January 1, 1601 (UTC).
	GetSystemTimeAsFileTime(&ft);

	// Fill ULARGE_INTEGER low and high parts.
	ul.LowPart = ft.dwLowDateTime;
	ul.HighPart = ft.dwHighDateTime;
	// Convert to microseconds.
	ul.QuadPart /= 10ULL;
	// Remove Windows to UNIX Epoch delta.
	ul.QuadPart -= 11644473600000000ULL;
	// Modulo to retrieve the microseconds.
	p->tv_usec = (long)(ul.QuadPart % 1000000LL);
	// Divide to retrieve the seconds.
	p->tv_sec = (long)(ul.QuadPart / 1000000LL);

	return 0;
}


int pthread_mutex_init(pthread_mutex_t *mutex)
{
    *mutex = CreateMutex(NULL, FALSE, NULL);
    return (*mutex == NULL);
}


int pthread_mutex_lock(pthread_mutex_t mutex)
{
    if (mutex != NULL) {
        WaitForSingleObject(mutex, INFINITE);
	return 0;
    }
    return 1;
}


int pthread_mutex_unlock(pthread_mutex_t mutex)
{
    if (mutex != NULL) {
        ReleaseMutex(mutex);
        return 0;
    }
    return 1;
}

DIR *opendir(const char *name)
{
    DIR *dir = 0;
    
    if(name && name[0])
    {
        size_t base_length = strlen(name);
        const char *all = /* search pattern must end with suitable wildcard */
        strchr("/\\", name[base_length - 1]) ? "*" : "/*";
        
        if((dir = (DIR *) malloc(sizeof *dir)) != 0 &&
           (dir->name = (char *) malloc(base_length + strlen(all) + 1)) != 0)
        {
            strcat(strcpy(dir->name, name), all);
            
            if((dir->handle =
                (handle_type) _findfirst(dir->name, &dir->info)) != -1)
            {
                dir->result.d_name = 0;
            }
            else /* rollback */
            {
                free(dir->name);
                free(dir);
                dir = 0;
            }
        }
        else /* rollback */
        {
            free(dir);
            dir   = 0;
            errno = ENOMEM;
        }
    }
    else
    {
        errno = EINVAL;
    }
    
    return dir;
}

int closedir(DIR *dir)
{
    int result = -1;
    
    if(dir)
    {
        if(dir->handle != -1)
        {
            result = _findclose(dir->handle);
        }
        
        free(dir->name);
        free(dir);
    }
    
    if(result == -1) /* map all errors to EBADF */
    {
        errno = EBADF;
    }
    
    return result;
}

struct dirent *readdir(DIR *dir)
{
    struct dirent *result = 0;
    
    if(dir && dir->handle != -1)
    {
        if(!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
        {
            result         = &dir->result;
            result->d_name = dir->info.name;
        }
    }
    else
    {
        errno = EBADF;
    }
    
    return result;
}

void rewinddir(DIR *dir)
{
    if(dir && dir->handle != -1)
    {
        _findclose(dir->handle);
        dir->handle = (handle_type) _findfirst(dir->name, &dir->info);
        dir->result.d_name = 0;
    }
    else
    {
        errno = EBADF;
    }
}

/*
 
 Copyright Kevlin Henney, 1997, 2003, 2012. All rights reserved.
 
 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose is hereby granted without fee, provided
 that this copyright and permissions notice appear in all copies and
 derivatives.
 
 This software is supplied "as is" without express or implied warranty.
 
 But that said, if there are any problems please get in touch.
 
 */
