
#ifndef ingescape_unix_functions_h
#define ingescape_unix_functions_h

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#include <time.h>

char *strndup(const char *str, size_t chars);

int gettimeofday(struct timeval* p, void* tz);

struct tm *gmtime_r(const time_t *timep, struct tm *result);


#define pthread_mutex_t HANDLE

int pthread_mutex_init(pthread_mutex_t* mutex);
int pthread_mutex_destroy(pthread_mutex_t mutex);
int pthread_mutex_lock(pthread_mutex_t mutex);
int pthread_mutex_unlock(pthread_mutex_t mutex);

#endif
