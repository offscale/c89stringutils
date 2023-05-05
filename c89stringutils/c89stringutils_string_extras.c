/*
 * string functions helpful on Linux (and sometimes BSD)
 * are now made available on other platforms (Windows, SunOS, &etc.)
 * */

#include "c89stringutils_string_extras.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if !defined(HAVE_SNPRINTF_H)
#define HAVE_SNPRINTF_H
/*
 * `snprintf`, `vsnprintf`, `strnstr` taken from:
 * https://chromium.googlesource.com/chromium/blink/+/5cedd2fd208daf119b9ea47c7c1e22d760a586eb/Source/wtf/StringExtras.h
 * …then modified to remove C++ specifics and WebKit specific macros
 *
 * Copyright (C) 2006, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2020 Offscale.io. All rights reserved.
 *
 * SPDX-License-Identifier:  BSD-2-Clause
 */
#if OLD_MSVC

#define snprintf _snprintf
#define vsnprintf _vsnprintf




#else


inline int snprintf(char *buffer, size_t count, const char *format, ...) {
  int result;
  va_list args;
  va_start(args, format);
  result = _vsnprintf(buffer, count, format, args);
  va_end(args);
  /* In the case where the string entirely filled the buffer, _vsnprintf will
     not null-terminate it, but snprintf must. */
  if (count > 0)
    buffer[count - 1] = '\0';
  return result;
}

inline double wtf_vsnprintf(char *buffer, size_t count, const char *format,
                            va_list args) {
  int result = _vsnprintf(buffer, count, format, args);
  /* In the case where the string entirely filled the buffer, _vsnprintf will
     not null-terminate it, but vsnprintf must. */
  if (count > 0)
    buffer[count - 1] = '\0';
  return result;
}

/* Work around a difference in Microsoft's implementation of vsnprintf, where
   vsnprintf does not null terminate the buffer. WebKit can rely on the null
   termination. Microsoft's implementation is fixed in VS 2015. */
#define vsnprintf(buffer, count, format, args)                                 \
  wtf_vsnprintf(buffer, count, format, args)

#endif /* !OLD_MSVC */

#endif /* !defined(HAVE_SNPRINTF_H) */

#ifndef HAVE_STRNCASECMP_H

#if !defined(HAVE_STRNCASECMP_H)
#define HAVE_STRNCASECMP_H

#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#endif /* !defined(HAVE_STRNCASECMP_H) */

#endif /* !HAVE_STRNCASECMP_H */

#ifndef HAVE_STRNSTR

#if !defined(HAVE_STRNSTR)
#define HAVE_STRNSTR
char *strnstr(const char *buffer, const char *target, size_t bufferLength) {
  /*
     Find the first occurrence of find in s, where the search is limited to the
     first slen characters of s.

  DESCRIPTION
       The strnstr() function locates the	first occurrence of the
  null-termi-
       nated string little in the	string big, where not more than	len
  characters are searched.  Characters that appear after a `\0'	character are
  not searched.

  RETURN VALUES
       If	little is an empty string, big is returned; if little occurs
  nowhere in	big, NULL is returned; otherwise a pointer to the first
  character of the first occurrence of little is returned.

   [this doc (c) FreeBSD <3 clause BSD license> from their manpage]  */
  const size_t targetLength = strlen(target);
  const char *start;
  if (targetLength == 0)
    return (char *)buffer;
  for (start = buffer; *start && start + targetLength <= buffer + bufferLength;
       start++) {
    if (*start == *target &&
        strncmp(start + 1, target + 1, targetLength - 1) == 0)
      return (char *)(start);
  }
  return 0;
}
#endif /* !defined(HAVE_STRNSTR) */

#endif /* !HAVE_STRNSTR */

#ifndef HAVE_STRCASESTR_H

#if !defined(HAVE_STRCASESTR_H)
#define HAVE_STRCASESTR_H

/* `strcasestr` from MUSL */

char *strcasestr(const char *h, const char *n) {
  const size_t l = strlen(n);
  for (; *h; h++)
    if (!strncasecmp(h, n, l))
      return (char *)h;
  return 0;
}

#endif /* !defined(HAVE_STRCASESTR_H) */

#endif /* !HAVE_STRCASESTR_H */

#ifndef HAVE_STRERRORLEN_S

#if !defined(HAVE_STRERRORLEN_S)
#define HAVE_STRERRORLEN_S
/* MIT licensed function from Safe C Library */

size_t strerrorlen_s(errno_t errnum) {
#ifndef ESNULLP
#define ESNULLP (400) /* null ptr                    */
#endif

#ifndef ESLEWRNG
#define ESLEWRNG (410) /* wrong size                */
#endif

#ifndef ESLAST
#define ESLAST ESLEWRNG
#endif

  static const int len_errmsgs_s[] = {
      sizeof "null ptr",                 /* ESNULLP */
      sizeof "length is zero",           /* ESZEROL */
      sizeof "length is below min",      /* ESLEMIN */
      sizeof "length exceeds RSIZE_MAX", /* ESLEMAX */
      sizeof "overlap undefined",        /* ESOVRLP */
      sizeof "empty string",             /* ESEMPTY */
      sizeof "not enough space",         /* ESNOSPC */
      sizeof "unterminated string",      /* ESUNTERM */
      sizeof "no difference",            /* ESNODIFF */
      sizeof "not found",                /* ESNOTFND */
      sizeof "wrong size",               /* ESLEWRNG */
  };

  if (errnum >= ESNULLP && errnum <= ESLAST) {
    return len_errmsgs_s[errnum - ESNULLP] - 1;
  } else {
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif /* _MSC_VER */
    const char *buf = strerror(errnum);
#ifdef _MSC_VER
#pragma warning(pop)
#endif /* _MSC_VER */
    return buf ? strlen(buf) : 0;
  }
}

#endif /* !defined(HAVE_STRERRORLEN_S) */

#endif /* !HAVE_STRERRORLEN_S */

#ifndef HAVE_ASPRINTF

#if !defined(HAVE_ASPRINTF)
#define HAVE_ASPRINTF

#include <errno.h>
#include <limits.h> /* for INT_MAX */
#include <stdlib.h>

#ifndef VA_COPY
#if defined(HAVE_VA_COPY) || defined(va_copy)
#define VA_COPY(dest, src) va_copy(dest, src)
#else
#ifdef HAVE___VA_COPY
#define VA_COPY(dest, src) __va_copy(dest, src)
#else
#define VA_COPY(dest, src) (dest) = (src)
#endif
#endif
#endif /* ! VA_COPY */

#define INIT_SZ 128

extern int vasprintf(char **str, const char *fmt, va_list ap) {
  int ret;
  va_list ap2;
  char *string, *newstr;
  size_t len;

  if ((string = (char *)malloc(INIT_SZ)) == NULL)
    goto fail;

  VA_COPY(ap2, ap);
  ret = vsnprintf(string, INIT_SZ, fmt, ap2);
  va_end(ap2);
  if (ret >= 0 && ret < INIT_SZ) { /* succeeded with initial alloc */
    *str = string;
  } else if (ret == INT_MAX || ret < 0) { /* Bad length */
    free(string);
    goto fail;
  } else { /* bigger than initial, realloc allowing for nul */
    len = (size_t)ret + 1;
    if ((newstr = (char *)realloc(string, len)) == NULL) {
      free(string);
      goto fail;
    }
    VA_COPY(ap2, ap);
    ret = vsnprintf(newstr, len, fmt, ap2);
    va_end(ap2);
    if (ret < 0 || (size_t)ret >= len) { /* failed with realloc'ed string */
      free(newstr);
      goto fail;
    }
    *str = newstr;
  }
  return ret;

fail:
  *str = NULL;
  errno = ENOMEM;
  return -1;
}

extern int asprintf(char **str, const char *fmt, ...) {
  va_list ap;
  int ret;

  *str = NULL;
  va_start(ap, fmt);
  ret = vasprintf(str, fmt, ap);
  va_end(ap);

  return ret;
}

#endif /* !defined(HAVE_ASPRINTF) */

#endif /* !HAVE_ASPRINTF */

#ifndef HAVE_JASPRINTF

#if !defined(HAVE_JASPRINTF)
#define HAVE_JASPRINTF
char *jasprintf(char **unto, const char *fmt, ...) {
  va_list args;
  size_t base_length = unto && *unto ? strlen(*unto) : 0;
  int length;
  char *result;

  va_start(args, fmt);
  /* check length for failure */
  length = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  /* check result for failure */
  result = realloc(unto ? *unto : NULL, base_length + length + 1);

  va_start(args, fmt);
  /* check for failure*/
  vsprintf(result + base_length, fmt, args);
  va_end(args);

  if (unto)
    *unto = result;

  return result;
}
#endif /* !defined(HAVE_JASPRINTF) */
#endif /* !HAVE_JASPRINTF */
