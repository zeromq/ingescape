/*
Copyright (c) 2008-2014, Troy D. Hanson   http://troydhanson.github.com/uthash/
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* a dynamic string implementation using macros
 */
#ifndef UTSTRING_H
#define UTSTRING_H

#define UTSTRING_VERSION 1.9.9

#ifdef __GNUC__
#define _UNUSED_ __attribute__ ((__unused__))
#else
#define _UNUSED_
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef oom
#define oom() exit(-1)
#endif

typedef struct {
    char *d;
    size_t n; /* allocd size */
    size_t i; /* index of first unused byte */
} UT_string;

#define utstring_reserve(s,amt)                            \
do {                                                       \
  if (((s)->n - (s)->i) < (size_t)(amt)) {                 \
    char *utstring_tmp = (char*)realloc(                   \
      (s)->d, (s)->n + (amt));                             \
    if (utstring_tmp == NULL) oom();                       \
    (s)->d = utstring_tmp;                                 \
    (s)->n += (amt);                                       \
  }                                                        \
} while(0)

#define utstring_init(s)                                   \
do {                                                       \
  (s)->n = 0; (s)->i = 0; (s)->d = NULL;                   \
  utstring_reserve(s,100);                                 \
  (s)->d[0] = '\0'; \
} while(0)

#define utstring_done(s)                                   \
do {                                                       \
  if ((s)->d != NULL) free((s)->d);                        \
  (s)->n = 0;                                              \
} while(0)

#define utstring_free(s)                                   \
do {                                                       \
  utstring_done(s);                                        \
  free(s);                                                 \
} while(0)

#define utstring_new(s)                                    \
do {                                                       \
   s = (UT_string*)calloc(sizeof(UT_string),1);            \
   if (!s) oom();                                          \
   utstring_init(s);                                       \
} while(0)

#define utstring_renew(s)                                  \
do {                                                       \
   if (s) {                                                \
     utstring_clear(s);                                    \
   } else {                                                \
     utstring_new(s);                                      \
   }                                                       \
} while(0)

#define utstring_clear(s)                                  \
do {                                                       \
  (s)->i = 0;                                              \
  (s)->d[0] = '\0';                                        \
} while(0)

#define utstring_bincpy(s,b,l)                             \
do {                                                       \
  utstring_reserve((s),(l)+1);                               \
  if (l) memcpy(&(s)->d[(s)->i], b, l);                    \
  (s)->i += (l);                                           \
  (s)->d[(s)->i]='\0';                                         \
} while(0)

#define utstring_concat(dst,src)                                 \
do {                                                             \
  utstring_reserve((dst),((src)->i)+1);                          \
  if ((src)->i) memcpy(&(dst)->d[(dst)->i], (src)->d, (src)->i); \
  (dst)->i += (src)->i;                                          \
  (dst)->d[(dst)->i]='\0';                                       \
} while(0)

#define utstring_len(s) ((unsigned)((s)->i))

#define utstring_body(s) ((s)->d)

_UNUSED_ static void utstring_printf_va(UT_string *s, const char *fmt, va_list ap) {
   int n;
   va_list cp;
   for (;;) {
#ifdef _WIN32
      cp = ap;
#else
      va_copy(cp, ap);
#endif
      n = vsnprintf (&s->d[s->i], s->n-s->i, fmt, cp);
      va_end(cp);

      if ((n > -1) && ((size_t) n < (s->n-s->i))) {
        s->i += n;
        return;
      }

      /* Else try again with more space. */
      if (n > -1) utstring_reserve(s,n+1); /* exact */
      else utstring_reserve(s,(s->n)*2);   /* 2x */
   }
}
#ifdef __GNUC__
/* support printf format checking (2=the format string, 3=start of varargs) */
static void utstring_printf(UT_string *s, const char *fmt, ...)
  __attribute__ (( format( printf, 2, 3) ));
#endif
_UNUSED_ static void utstring_printf(UT_string *s, const char *fmt, ...) {
   va_list ap;
   va_start(ap,fmt);
   utstring_printf_va(s,fmt,ap);
   va_end(ap);
}

/*******************************************************************************
 * begin substring search functions                                            *
 ******************************************************************************/
/* Build KMP table from left to right. */
_UNUSED_ static void _utstring_build_table(
    const char *P_Needle,
    size_t P_needle_len,
    long *P_KMP_Table)
{
    long i, j;

    i = 0;
    j = i - 1;
    P_KMP_Table[i] = j;
    while (i < (long) P_needle_len)
    {
        while ( (j > -1) && (P_Needle[i] != P_Needle[j]) )
        {
           j = P_KMP_Table[j];
        }
        i++;
        j++;
        if (i < (long) P_needle_len)
        {
            if (P_Needle[i] == P_Needle[j])
            {
                P_KMP_Table[i] = P_KMP_Table[j];
            }
            else
            {
                P_KMP_Table[i] = j;
            }
        }
        else
        {
            P_KMP_Table[i] = j;
        }
    }

    return;
}


/* Build KMP table from right to left. */
_UNUSED_ static void _utstring_build_tabler(
    const char *P_Needle,
    size_t P_needle_len,
    long *P_KMP_Table)
{
    long i, j;

    i = P_needle_len - 1;
    j = i + 1;
    P_KMP_Table[i + 1] = j;
    while (i >= 0)
    {
        while ( (j < (long) P_needle_len) && (P_Needle[i] != P_Needle[j]) )
        {
           j = P_KMP_Table[j + 1];
        }
        i--;
        j--;
        if (i >= 0)
        {
            if (P_Needle[i] == P_Needle[j])
            {
                P_KMP_Table[i + 1] = P_KMP_Table[j + 1];
            }
            else
            {
                P_KMP_Table[i + 1] = j;
            }
        }
        else
        {
            P_KMP_Table[i + 1] = j;
        }
    }

    return;
}


/* Search data from left to right. ( Multiple search mode. ) */
_UNUSED_ static long _utstring_find(
    const char *P_Haystack,
    size_t P_haystack_len,
    const char *P_Needle,
    size_t P_needle_len,
    long *P_KMP_Table)
{
    long i, j;
    long V_find_position = -1;

    /* Search from left to right. */
    i = j = 0;
    while ( (j < (int)P_haystack_len) && (((P_haystack_len - j) + i) >= P_needle_len) )
    {
        while ( (i > -1) && (P_Needle[i] != P_Haystack[j]) )
        {
            i = P_KMP_Table[i];
        }
        i++;
        j++;
        if (i >= (int)P_needle_len)
        {
            /* Found. */
            V_find_position = j - i;
            break;
        }
    }

    return V_find_position;
}


/* Search data from right to left. ( Multiple search mode. ) */
_UNUSED_ static long _utstring_findR(
    const char *P_Haystack,
    size_t P_haystack_len,
    const char *P_Needle,
    size_t P_needle_len,
    long *P_KMP_Table)
{
    long i, j;
    long V_find_position = -1;

    /* Search from right to left. */
    j = (P_haystack_len - 1);
    i = (P_needle_len - 1);
    while ( (j >= 0) && (j >= i) )
    {
        while ( (i < (int)P_needle_len) && (P_Needle[i] != P_Haystack[j]) )
        {
            i = P_KMP_Table[i + 1];
        }
        i--;
        j--;
        if (i < 0)
        {
            /* Found. */
            V_find_position = j + 1;
            break;
        }
    }

    return V_find_position;
}


/* Search data from left to right. ( One time search mode. ) */
_UNUSED_ static long utstring_find(
    UT_string *s,
    long P_start_position,   /* Start from 0. -1 means last position. */
    const char *P_Needle,
    size_t P_needle_len)
{
    long V_start_position;
    long V_haystack_len;
    long *V_KMP_Table;
    long V_find_position = -1;

    if (P_start_position < 0)
    {
        V_start_position = s->i + P_start_position;
    }
    else
    {
        V_start_position = P_start_position;
    }
    V_haystack_len = s->i - V_start_position;
    if ( (V_haystack_len >= (long) P_needle_len) && (P_needle_len > 0) )
    {
        V_KMP_Table = (long *)malloc(sizeof(long) * (P_needle_len + 1));
        if (V_KMP_Table != NULL)
        {
            _utstring_build_table(P_Needle, P_needle_len, V_KMP_Table);

            V_find_position = _utstring_find(s->d + V_start_position,
                                            V_haystack_len,
                                            P_Needle,
                                            P_needle_len,
                                            V_KMP_Table);
            if (V_find_position >= 0)
            {
                V_find_position += V_start_position;
            }

            free(V_KMP_Table);
        }
    }

    return V_find_position;
}


/* Search data from right to left. ( One time search mode. ) */
_UNUSED_ static long utstring_findR(
    UT_string *s,
    long P_start_position,   /* Start from 0. -1 means last position. */
    const char *P_Needle,
    size_t P_needle_len)
{
    long V_start_position;
    long V_haystack_len;
    long *V_KMP_Table;
    long V_find_position = -1;

    if (P_start_position < 0)
    {
        V_start_position = s->i + P_start_position;
    }
    else
    {
        V_start_position = P_start_position;
    }
    V_haystack_len = V_start_position + 1;
    if ( (V_haystack_len >= (long) P_needle_len) && (P_needle_len > 0) )
    {
        V_KMP_Table = (long *)malloc(sizeof(long) * (P_needle_len + 1));
        if (V_KMP_Table != NULL)
        {
            _utstring_build_tabler(P_Needle, P_needle_len, V_KMP_Table);

            V_find_position = _utstring_findR(s->d,
                                             V_haystack_len,
                                             P_Needle,
                                             P_needle_len,
                                             V_KMP_Table);

            free(V_KMP_Table);
        }
    }

    return V_find_position;
}
/*******************************************************************************
 * end substring search functions                                              *
 ******************************************************************************/

#endif /* UTSTRING_H */
