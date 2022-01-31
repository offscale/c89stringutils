/*
 * string functions helpful on Linux (and sometimes BSD)
 * are now made available on other platforms (Windows, SunOS, &etc.)
 * */

#ifndef C89STRINGUTILS_STRING_EXTRAS_H
#define C89STRINGUTILS_STRING_EXTRAS_H

#include <string.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#   if defined(_MSC_VER) && !defined(__INTEL_COMPILER)

#       if _MSC_VER >= 1900

/* snprintf is implemented in VS 2015 */
#               define HAVE_SNPRINTF_H

#       endif /* _MSC_VER >= 1900 */

#   else

#       define HAVE_STRNCASECMP

#   endif /* defined(_MSC_VER) && !defined(__INTEL_COMPILER) */

#else

#   include <sys/param.h>

#   if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L || \
      defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || \
      defined(__DragonFly__) || defined(BSD)
#       define HAVE_SNPRINTF_H
#       define HAVE_STRCASESTR_H
#   endif /* _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L */

#   if defined(__APPLE__) && defined(__MACH__)
#       define HAVE_SNPRINTF_H
#       define HAVE_STRNCASECMP_H
#   endif /* defined(__APPLE__) && defined(__MACH__) */

#   if (defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200112L) || defined(BSD) && (BSD >= 199306)) && \
           (!defined(__linux__) && !defined(linux) && !defined(__linux))
#      define HAVE_STRNSTR_H
#   endif /* (defined(_POSIX_VERSION) && (_POSIX_VERSION >= 200112L) || defined(BSD) && (BSD >= 199306)) && \
                 (!defined(__linux__) && !defined(linux) && !defined(__linux)) */

#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && defined(__STDC_LIB_EXT1__) && \
        __STDC_WANT_LIB_EXT1__ || defined(__APPLE__) || defined(__APPLE_CC__)

#   define HAVE_STRERRORLEN_S

#else

#   if !defined(__APPLE__) && !defined(__APPLE_CC__)
        typedef int errno_t;
#   endif /* !defined(__APPLE__) && !defined(__APPLE_CC__) */

#endif /* defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && defined(__STDC_LIB_EXT1__) && \
              __STDC_WANT_LIB_EXT1__ || defined(__APPLE__) || defined(__APPLE_CC__) */

#ifndef _MSC_VER
#   define HAVE_STRINGS_H
#   define HAVE_STRNCASECMP_H
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#   include <strings.h>
#endif /* HAVE_STRINGS_H */

#if !defined(HAVE_SNPRINTF_H) && defined(LIBACQUIRE_IMPLEMENTATION)

/*
 * `snprintf`, `strncasecmp`, `vsnprintf`, `strnstr` taken from:
 * https://chromium.googlesource.com/chromium/blink/+/5cedd2fd208daf119b9ea47c7c1e22d760a586eb/Source/wtf/StringExtras.h
 * …then modified to remove C++ specifics and WebKit specific macros
 *
 * Copyright (C) 2006, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2020 Offscale.io. All rights reserved.
 *
 * SPDX-License-Identifier:  BSD-2-Clause
 */

inline int snprintf(char* buffer, size_t count, const char* format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = _vsnprintf(buffer, count, format, args);
    va_end(args);
    /* In the case where the string entirely filled the buffer, _vsnprintf will not
       null-terminate it, but snprintf must. */
    if (count > 0)
        buffer[count - 1] = '\0';
    return result;
}

inline double wtf_vsnprintf(char* buffer, size_t count, const char* format, va_list args)
{
    int result = _vsnprintf(buffer, count, format, args);
    /* In the case where the string entirely filled the buffer, _vsnprintf will not
       null-terminate it, but vsnprintf must. */
    if (count > 0)
        buffer[count - 1] = '\0';
    return result;
}

/* Work around a difference in Microsoft's implementation of vsnprintf, where
   vsnprintf does not null terminate the buffer. WebKit can rely on the null
   termination. Microsoft's implementation is fixed in VS 2015. */
#define vsnprintf(buffer, count, format, args) wtf_vsnprintf(buffer, count, format, args)

#endif /* !defined(HAVE_SNPRINTF_H) && defined(LIBACQUIRE_IMPLEMENTATION) */

#ifndef HAVE_STRNCASECMP_H

extern int strncasecmp(const char *, const char *, size_t);

extern int strcasecmp(const char *, const char *);

#ifdef LIBACQUIRE_IMPLEMENTATION

int strncasecmp(const char *s1, const char *s2, size_t len) {
    return _strnicmp(s1, s2, len);
}

int strcasecmp(const char *s1, const char *s2) {
    return _stricmp(s1, s2);
}

#endif /* LIBACQUIRE_IMPLEMENTATION */

#endif /* !HAVE_STRNCASECMP_H */

#ifndef HAVE_STRNSTR_H

extern char *strnstr(const char *, const char *, size_t);

#ifdef LIBACQUIRE_IMPLEMENTATION
char *strnstr(const char *buffer, const char *target, size_t bufferLength) {
    /*
       Find the first occurrence of find in s, where the search is limited to the
       first slen characters of s.

    DESCRIPTION
         The strnstr() function locates the	first occurrence of the	null-termi-
         nated string little in the	string big, where not more than	len characters
         are searched.  Characters that appear after a `\0'	character are not
         searched.

    RETURN VALUES
         If	little is an empty string, big is returned; if little occurs nowhere
         in	big, NULL is returned; otherwise a pointer to the first	character of
         the first occurrence of little is returned.

     [this doc (c) FreeBSD <3 clause BSD license> from their manpage]  */
    const size_t targetLength = strlen(target);
    const char *start;
    if (targetLength == 0)
        return (char *) buffer;
    for (start = buffer; *start && start + targetLength <= buffer + bufferLength; start++) {
        if (*start == *target && strncmp(start + 1, target + 1, targetLength - 1) == 0)
            return (char *) (start);
    }
    return 0;
}
#endif /* LIBACQUIRE_IMPLEMENTATION */

#endif /* ! HAVE_STRNSTR_H */

#ifndef HAVE_STRCASESTR_H
extern char *strcasestr(const char *, const char *);

#ifdef LIBACQUIRE_IMPLEMENTATION

/* `strcasestr` from MUSL */

char *strcasestr(const char *h, const char *n)
{
    const size_t l = strlen(n);
    for (; *h; h++) if (!strncasecmp(h, n, l)) return (char *)h;
    return 0;
}

#endif /* LIBACQUIRE_IMPLEMENTATION */

#endif /* ! HAVE_STRCASESTR_H */

#ifndef HAVE_STRERRORLEN_S

/* MIT licensed function from Safe C Library */

size_t strerrorlen_s(errno_t errnum)
{
#ifndef ESNULLP
#define ESNULLP         ( 400 )       /* null ptr                    */
#endif

#ifndef ESLEWRNG
#define ESLEWRNG        ( 410 )       /* wrong size                */
#endif

#ifndef ESLAST
#define ESLAST ESLEWRNG
#endif

    static const int len_errmsgs_s[] = {
            sizeof "null ptr",               /* ESNULLP */
            sizeof "length is zero",         /* ESZEROL */
            sizeof "length is below min",    /* ESLEMIN */
            sizeof "length exceeds RSIZE_MAX",/* ESLEMAX */
            sizeof "overlap undefined",      /* ESOVRLP */
            sizeof "empty string",           /* ESEMPTY */
            sizeof "not enough space",       /* ESNOSPC */
            sizeof "unterminated string",    /* ESUNTERM */
            sizeof "no difference",          /* ESNODIFF */
            sizeof "not found",              /* ESNOTFND */
            sizeof "wrong size",             /* ESLEWRNG */
    };

    if (errnum >= ESNULLP && errnum <= ESLAST)
    {
        return len_errmsgs_s[errnum - ESNULLP] - 1;
    }
    else
    {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif /* _MSC_VER */
        const char *buf = strerror(errnum);
#ifdef _MSC_VER
#pragma warning(pop)
#endif /* _MSC_VER */
        return buf ? strlen(buf) : 0;
    }
}

#endif /* HAVE_STRERRORLEN_S */

#endif /* ! C89STRINGUTILS_STRING_EXTRAS_H */
