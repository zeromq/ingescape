#include <windows.h>
#include <unixfunctions.h>

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
	mutex = CreateMutex(NULL, FALSE, NULL);
	if (mutex == NULL) return 1; else return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
	while (OpenMutex(MUTEX_ALL_ACCESS, FALSE, NULL) == NULL) {
		WaitForSingleObject(mutex, INFINITE);
	}
	return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	ReleaseMutex(*mutex);
	return 0;
}