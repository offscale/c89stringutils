/*
 * string functions helpful on Linux (and sometimes BSD)
 * are now made available on other platforms (Windows, SunOS, &etc.)
 * */

/* clang-format off */
#include "c89stringutils_string_extras.h"
#include "c89stringutils_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h> /* for INT_MAX */
/* clang-format on */

void c89stringutils_log_debug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

#ifndef HAVE_SNPRINTF_H
#define HAVE_SNPRINTF_H

/*
 * `snprintf`, `vsnprintf`, `strnstr` taken from:
 * https://chromium.googlesource.com/chromium/blink/+/5cedd2fd208daf119b9ea47c7c1e22d760a586eb/Source/wtf/StringExtras.h
 * ...then modified to remove C++ specifics and WebKit specific macros
 *
 * Copyright (C) 2006, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2020 Offscale.io. All rights reserved.
 *
 * SPDX-License-Identifier:  BSD-2-Clause
 */

#ifdef ANY_BSD
#define _vsnprintf vsnprintf
#endif /* ANY_BSD */

static int wtf_vsnprintf(char *buffer, size_t count, const char *format,
                         va_list args) {
  int rc;
#if defined(_MSC_VER)
  rc = _vsnprintf_s(buffer, count, _TRUNCATE, format, args);
  if (rc < 0) {
    rc = _vscprintf(format, args);
  }
#else
  rc = _vsnprintf(buffer, count, format, args);
#endif
  /* In the case where the string entirely filled the buffer, _vsnprintf will
     not null-terminate it, but vsnprintf must. */
  if (count > 0)
    buffer[count - 1] = '\0';
  return rc;
}

#define vsnprintf(buffer, count, format, args)                                 \
  wtf_vsnprintf(buffer, count, format, args)

#endif /* !HAVE_SNPRINTF_H */

#ifndef HAVE_STRNCASECMP_H

#define HAVE_STRNCASECMP_H

int strncasecmp(const char *s1, const char *s2, size_t n) {
  int rc = 0;
  size_t i;
  
  for (i = 0; i < n && s1[i] && s2[i]; i++) {
    char c1 = s1[i];
    char c2 = s2[i];
    
    /* Convert to lowercase for comparison */
    if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
    if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
    
    if (c1 != c2) {
      rc = (c1 < c2) ? -1 : 1;
      break;
    }
  }
  
  /* Handle end of string cases */
  if (rc == 0 && i < n) {
    if (!s1[i] && !s2[i]) rc = 0;
    else if (!s1[i]) rc = -1;
    else rc = 1;
  }
  
  return rc;
}

int strcasecmp(const char *s1, const char *s2) {
  int rc = 0;
  size_t i = 0;
  
  while (s1[i] && s2[i]) {
    char c1 = s1[i];
    char c2 = s2[i];
    
    /* Convert to lowercase for comparison */
    if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
    if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
    
    if (c1 != c2) {
      rc = (c1 < c2) ? -1 : 1;
      break;
    }
    i++;
  }
  
  /* Handle end of string cases */
  if (rc == 0) {
    if (!s1[i] && !s2[i]) rc = 0;
    else if (!s1[i]) rc = -1;
    else rc = 1;
  }
  
  return rc;
}

#endif /* !HAVE_STRNCASECMP_H */

#ifndef HAVE_STRNSTR
#define HAVE_STRNSTR

char *strnstr(const char *buffer, const char *target, size_t bufferLength) {
  /*
     Find the first occurrence of find in s, where the search is limited to the
     first slen characters of s.

  DESCRIPTION
       The strnstr() function locates the       first occurrence of the
  null-termi-
       nated string little in the       string big, where not more than len
  characters are searched.  Characters that appear after a `\0' character are
  not searched.

  RETURN VALUES
       If       little is an empty string, big is returned; if little occurs
  nowhere in    big, NULL is returned; otherwise a pointer to the first
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

#endif /* !HAVE_STRNSTR */

#ifndef HAVE_STRCASESTR_H

#define HAVE_STRCASESTR_H

/* `strcasestr` from MUSL */

char *strcasestr(const char *h, const char *n) {
  const size_t l = strlen(n);
  if (l == 0) return (char *)h;
  
  for (; *h; h++) {
    if (h[0] == n[0] || 
        (h[0] >= 'A' && h[0] <= 'Z' && h[0] + 32 == n[0]) ||
        (h[0] >= 'a' && h[0] <= 'z' && h[0] - 32 == n[0])) {
      if (!strncasecmp(h, n, l))
        return (char *)h;
    }
  }
  return 0;
}

#endif /* !HAVE_STRCASESTR_H */

#ifndef HAVE_STRERRORLEN_S

#define HAVE_STRERRORLEN_S
/* MIT licensed function from Safe C Library */

size_t strerrorlen_s(errno_t errnum) {
#ifndef ESNULLP
#define ESNULLP (400) /* null ptr                    */
#endif

#ifndef ESZEROL
#define ESZEROL (401) /* length is zero            */
#endif

#ifndef ESLEMIN
#define ESLEMIN (402) /* length is below min        */
#endif

#ifndef ESLEMAX
#define ESLEMAX (403) /* length exceeds RSIZE_MAX   */
#endif

#ifndef ESOVRLP
#define ESOVRLP (404) /* overlap undefined          */
#endif

#ifndef ESEMPTY
#define ESEMPTY (405) /* empty string               */
#endif

#ifndef ESNOSPC
#define ESNOSPC (406) /* not enough space           */
#endif

#ifndef ESUNTERM
#define ESUNTERM (407) /* unterminated string        */
#endif

#ifndef ESNODIFF
#define ESNODIFF (408) /* no difference              */
#endif

#ifndef ESNOTFND
#define ESNOTFND (409) /* not found                  */
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
    char errbuf[256];
    return strerror_s(errbuf, sizeof(errbuf), errnum) == 0 ? strlen(errbuf) : 0;
#else
    const char *buf;
    buf = strerror(errnum);
    return buf ? strlen(buf) : 0;
#endif /* _MSC_VER */
  }
}

#endif /* !HAVE_STRERRORLEN_S */

#ifndef HAVE_ASPRINTF
#define HAVE_ASPRINTF

#ifndef VA_COPY
# if defined(HAVE_VA_COPY)
#  define VA_COPY(dest, src) va_copy(dest, src)
# elif defined(HAVE___VA_COPY)
#  define VA_COPY(dest, src) __va_copy(dest, src)
# elif defined(HAVE_VA_LIST_ASSIGNMENT)
#  define VA_COPY(dest, src) ((dest) = (src))
# else
#  define VA_COPY(dest, src) memcpy(&(dest), &(src), sizeof(va_list))
# endif
#endif /* !VA_COPY */

#define INIT_SZ 128

extern int vasprintf(char **str, const char *fmt, va_list ap) {
  int rc;
  va_list ap2;
  char *string, *newstr;
  size_t len;

  if ((string = (char *)malloc(INIT_SZ)) == NULL) {
    rc = -1;
    goto fail;
  }

  VA_COPY(ap2, ap);
  rc = vsnprintf(string, INIT_SZ, fmt, ap2);
  va_end(ap2);
  if (rc >= 0 && rc < INIT_SZ) { /* succeeded with initial alloc */
    *str = string;
  } else if (rc == INT_MAX || rc < 0) { /* Bad length */
    free(string);
    rc = -1;
    goto fail;
  } else { /* bigger than initial, realloc allowing for nul */
    len = (size_t)rc + 1;
    if ((newstr = (char *)realloc(string, len)) == NULL) {
      free(string);
      rc = -1;
      goto fail;
    }
    VA_COPY(ap2, ap);
    rc = vsnprintf(newstr, len, fmt, ap2);
    va_end(ap2);
    if (rc < 0 || (size_t)rc >= len) { /* failed with realloc'ed string */
      free(newstr);
      rc = -1;
      goto fail;
    }
    *str = newstr;
  }
  return rc;

fail:
  if (rc != 0) {
#ifdef _MSC_VER
    char errbuf[256];
    strerror_s(errbuf, sizeof(errbuf), errno);
    LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, errbuf);
#else
    LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, strerror(errno));
#endif
  }
  *str = NULL;
  errno = ENOMEM;
  return rc;
}

extern int asprintf(char **str, const char *fmt, ...) {
  int rc;
  va_list ap;

  *str = NULL;
  va_start(ap, fmt);
  rc = vasprintf(str, fmt, ap);
  va_end(ap);

  if (rc < 0) {
#ifdef _MSC_VER
    char errbuf[256];
    strerror_s(errbuf, sizeof(errbuf), errno);
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, errbuf);
#else
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, strerror(errno));
#endif
  }

  return rc;
}

#endif /* !HAVE_ASPRINTF */

#ifndef HAVE_JASPRINTF
#define HAVE_JASPRINTF
char *jasprintf(char **unto, const char *fmt, ...) {
  va_list args;
  size_t base_length;
  int length;
  int rc;
  char *result;

  base_length = unto && *unto ? strlen(*unto) : 0;

  va_start(args, fmt);
  /* check length for failure */
#if defined(_MSC_VER) && _MSC_VER < 1900
  length = _vscprintf(fmt, args);
#else
  length = vsnprintf(NULL, 0, fmt, args);
#endif
  va_end(args);

  if (length < 0)
    return NULL;

  /* check result for failure */
  result = (char *)realloc(unto ? *unto : NULL, base_length + (size_t)length + 1);

  /* if realloc failed, check if we have a valid pointer to free */
  if (result == NULL) {
    if (unto && *unto) {
      LOG_DEBUG("Reallocation failed but we have a valid pointer to free");
    } else {
      LOG_DEBUG("Reallocation failed and no valid pointer to free");
    }
    return NULL;
  }

  va_start(args, fmt);
  /* check for failure*/
#if defined(_MSC_VER)
  rc = vsprintf_s(result + base_length, (size_t)length + 1, fmt, args);
  if (rc < 0) {
    /* handle error, printing the nonzero exit code for debug purposes */
    LOG_DEBUG("vsprintf_s failed with rc=%d", rc);
  }
#else
  rc = vsprintf(result + base_length, fmt, args);
  if (rc < 0) {
    LOG_DEBUG("vsprintf failed with rc=%d", rc);
  }
#endif
  va_end(args);

  if (unto)
    *unto = result;

  return result;
}
#endif /* !HAVE_JASPRINTF */
