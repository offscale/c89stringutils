/*
 * string functions helpful on Linux (and sometimes BSD)
 * are now made available on other platforms (Windows, SunOS, &etc.)
 * */

#ifndef C89STRINGUTILS_STRING_EXTRAS_H
#define C89STRINGUTILS_STRING_EXTRAS_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "c89stringutils_export.h"

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||     \
    defined(__bsdi__) || defined(__DragonFly__) || defined(BSD)
#define ANY_BSD
#define __BSD_VISIBLE 1
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)

/* snprintf is implemented in VS 2015 */
#if _MSC_VER >= 1900

#define HAVE_SNPRINTF_H

#else

#define OLD_MSVC
/* !defined(MSC_VER) || MSC_VER > 1400 *//* 1400 is MSVC 2005 */

#endif /* _MSC_VER >= 1900 */

#else
#ifdef __MINGW32__
#define HAVE_SNPRINTF_H
#endif /* __MINGW32__ */

#define HAVE_STRNCASECMP

#endif /* defined(_MSC_VER) && !defined(__INTEL_COMPILER) */

#elif defined(__WATCOMC__)

#else

#include <sys/param.h>

#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE ||                   \
    _POSIX_C_SOURCE >= 200112L
#define HAVE_SNPRINTF_H
#endif /* _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE ||             \
          _POSIX_C_SOURCE >= 200112L */
#if defined(_GNU_SOURCE) || defined(ANY_BSD)
#define HAVE_STRCASESTR_H
#endif /* defined(_GNU_SOURCE) || defined(ANY_BSD) */

#if defined(__APPLE__) && defined(__MACH__)
#define HAVE_SNPRINTF_H
#define HAVE_STRNCASECMP_H
#endif /* defined(__APPLE__) && defined(__MACH__) */

#if defined(BSD) && (BSD >= 199306) && !defined(__linux__) &&                  \
    !defined(linux) && !defined(__linux)
#define HAVE_STRNSTR
#endif /* defined(BSD) && (BSD >= 199306) && !defined(__linux__) &&            \
          !defined(linux) && !defined(__linux) */

#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||           \
          defined(__NT__) */

#if defined(__STDC_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__

#define HAVE_STRERRORLEN_S

#else

#if !defined(__APPLE__) && !defined(__APPLE_CC__)
typedef int errno_t;
#endif /* !defined(__APPLE__) && !defined(__APPLE_CC__) */

#if defined(__linux__) || defined(linux) || defined(__linux) || defined(ANY_BSD)
#define strerror_s strerror_r
#define HAVE_STRERRORLEN_S
#endif /* defined(__linux__) || defined(linux) || defined(__linux) ||          \
          defined(ANY_BSD) */

#endif /* defined(__STDC_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__ */

#ifndef _MSC_VER
#define HAVE_STRINGS_H
#define HAVE_STRNCASECMP_H
#endif

#if defined(ANY_BSD) || defined(__APPLE__) && defined(__MACH__) ||             \
    defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define HAVE_ASPRINTF
#endif /* defined(ANY_BSD) || defined(__APPLE__) && defined(__MACH__) ||       \
          defined(_GNU_SOURCE) || defined(_BSD_SOURCE) */

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */

#ifndef HAVE_STRNCASECMP_H

extern C89STRINGUTILS_EXPORT int strncasecmp(const char *, const char *, size_t);

extern C89STRINGUTILS_EXPORT int strcasecmp(const char *, const char *);

#endif /* !HAVE_STRNCASECMP_H */

#ifndef HAVE_STRNSTR

extern C89STRINGUTILS_EXPORT char *strnstr(const char *, const char *, size_t);

#endif /* !HAVE_STRNSTR */

#ifndef HAVE_STRCASESTR_H

extern C89STRINGUTILS_EXPORT char *strcasestr(const char *, const char *);

#endif /* !HAVE_STRCASESTR_H */

#ifndef HAVE_STRERRORLEN_S

extern C89STRINGUTILS_EXPORT size_t strerrorlen_s(errno_t);

#endif /* !HAVE_STRERRORLEN_S */

#ifndef HAVE_ASPRINTF

extern C89STRINGUTILS_EXPORT int vasprintf(char **str, const char *fmt, va_list ap);

extern C89STRINGUTILS_EXPORT int asprintf(char **str, const char *fmt, ...);

#endif /* !HAVE_ASPRINTF */

#ifndef HAVE_JASPRINTF

/* `jasprintf`, a version of `asprintf` that concatenates on successive calls:
 * char *s; jasprintf(&s, "foo%s", "bar");
 * jasprintf(&s, "can%s", "haz"); free(s);
 * */
extern C89STRINGUTILS_EXPORT char *jasprintf(char **unto, const char *fmt, ...);

#endif /* !HAVE_JASPRINTF */

#endif /* !C89STRINGUTILS_STRING_EXTRAS_H */
