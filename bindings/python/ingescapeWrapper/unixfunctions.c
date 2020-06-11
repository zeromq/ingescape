
#include <unixfunctions.h>

#include <malloc.h>

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

struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
	struct tm * return_val = NULL;
	if (timep != NULL
		&& result != NULL
		&& gmtime_s(result, timep) == 0) {
		return_val = result;
	}
	return return_val;
}

int pthread_mutex_init(pthread_mutex_t *mutex)
{
    *mutex = CreateMutex(NULL, FALSE, NULL);
    return (*mutex == NULL);
}

int pthread_mutex_destroy(pthread_mutex_t mutex)
{
	return !CloseHandle(mutex);
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
