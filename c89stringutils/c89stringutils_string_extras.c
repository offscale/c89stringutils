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
  int rc;
  va_list args;
  va_start(args, fmt);
#if defined(_MSC_VER)
  rc = vfprintf_s(stderr, fmt, args);
#else
  rc = vfprintf(stderr, fmt, args);
#endif
  if (rc < 0) {
    /* ignore error in log */
  }
#if defined(_MSC_VER)
  rc = fprintf_s(stderr, "\n");
#else
  rc = fprintf(stderr, "\n");
#endif
  if (rc < 0) {
    /* ignore error in log */
  }
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
    if (rc < 0) {
      LOG_DEBUG("_vscprintf failed with rc=%d", rc);
      return rc;
    }
  }
#else
  rc = _vsnprintf(buffer, count, format, args);
  if (rc < 0) {
    LOG_DEBUG("_vsnprintf failed with rc=%d", rc);
    return rc;
  }
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
  int rc;
  if (s1 == NULL || s2 == NULL) {
    LOG_DEBUG("s1 or s2 is NULL");
    return -1;
  }
  rc = _strnicmp(s1, s2, n);
  return rc;
}

int strcasecmp(const char *s1, const char *s2) {
  int rc;
  if (s1 == NULL || s2 == NULL) {
    LOG_DEBUG("s1 or s2 is NULL");
    return -1;
  }
  rc = _stricmp(s1, s2);
  return rc;
}

#endif /* !HAVE_STRNCASECMP_H */

#ifndef HAVE_STRNSTR
#define HAVE_STRNSTR

char *strnstr(const char *buffer, const char *target, size_t bufferLength) {
  size_t targetLength;
  const char *start;
  int rc;
  if (buffer == NULL || target == NULL) {
    LOG_DEBUG("buffer or target is NULL");
    return NULL;
  }
  targetLength = strlen(target);
  if (targetLength == 0)
    return (char *)buffer;
  for (start = buffer; *start && start + targetLength <= buffer + bufferLength;
       start++) {
    if (*start == *target) {
      rc = strncmp(start + 1, target + 1, targetLength - 1);
      if (rc == 0) {
        return (char *)(start);
      }
    }
  }
  return 0;
}

#endif /* !HAVE_STRNSTR */

#ifndef HAVE_STRCASESTR_H

#define HAVE_STRCASESTR_H

/* `strcasestr` from MUSL */

char *strcasestr(const char *h, const char *n) {
  size_t l;
  int rc;
  if (h == NULL || n == NULL) {
    LOG_DEBUG("h or n is NULL");
    return NULL;
  }
  l = strlen(n);
  for (; *h; h++) {
    rc = strncasecmp(h, n, l);
    if (rc == 0)
      return (char *)h;
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
    int rc;
    rc = strerror_s(errbuf, sizeof(errbuf), errnum);
    if (rc != 0) {
      LOG_DEBUG("strerror_s failed with rc=%d", rc);
      return 0;
    }
    return strlen(errbuf);
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
#if defined(HAVE_VA_COPY) || defined(va_copy)
#define VA_COPY(dest, src) va_copy(dest, src)
#else
#if defined(HAVE___VA_COPY) || defined(__va_copy)
#define VA_COPY(dest, src) __va_copy(dest, src)
#else
#define VA_COPY(dest, src) (dest) = (src)
#endif
#endif
#endif /* ! VA_COPY */

#define INIT_SZ 128

extern int vasprintf(char **str, const char *fmt, va_list ap) {
  int rc;
  va_list ap2;
  char *string, *newstr;
  size_t len;

  if (str == NULL || fmt == NULL) {
    LOG_DEBUG("str or fmt is NULL");
    return -1;
  }

  string = (char *)malloc(INIT_SZ);
  if (string == NULL) {
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
    newstr = (char *)realloc(string, len);
    if (newstr == NULL) {
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
    int err_rc;
    err_rc = strerror_s(errbuf, sizeof(errbuf), errno);
    if (err_rc != 0) {
      LOG_DEBUG("strerror_s failed with rc=%d", err_rc);
      errbuf[0] = '\0';
    }
    LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, errbuf);
#else
    const char *errstr;
    errstr = strerror(errno);
    LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc,
              errstr ? errstr : "");
#endif
  }
  *str = NULL;
  errno = ENOMEM;
  return rc;
}

extern int asprintf(char **str, const char *fmt, ...) {
  int rc;
  va_list ap;

  if (str == NULL || fmt == NULL) {
    LOG_DEBUG("str or fmt is NULL");
    return -1;
  }

  *str = NULL;
  va_start(ap, fmt);
  rc = vasprintf(str, fmt, ap);
  va_end(ap);

  if (rc < 0) {
#ifdef _MSC_VER
    char errbuf[256];
    int err_rc;
    err_rc = strerror_s(errbuf, sizeof(errbuf), errno);
    if (err_rc != 0) {
      LOG_DEBUG("strerror_s failed with rc=%d", err_rc);
      errbuf[0] = '\0';
    }
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, errbuf);
#else
    const char *errstr;
    errstr = strerror(errno);
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, errstr ? errstr : "");
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

  if (fmt == NULL) {
    LOG_DEBUG("fmt is NULL");
    return NULL;
  }

  base_length = unto && *unto ? strlen(*unto) : 0;

  va_start(args, fmt);
  /* check length for failure */
#if defined(_MSC_VER) && _MSC_VER < 1900
  length = _vscprintf(fmt, args);
  if (length < 0) {
    LOG_DEBUG("_vscprintf failed with rc=%d", length);
  }
#else
  length = vsnprintf(NULL, 0, fmt, args);
  if (length < 0) {
    LOG_DEBUG("vsnprintf failed with rc=%d", length);
  }
#endif
  va_end(args);

  if (length < 0)
    return NULL;

  /* check result for failure */
  result =
      (char *)realloc(unto ? *unto : NULL, base_length + (size_t)length + 1);

  if (result == NULL) {
    LOG_DEBUG("realloc failed with rc=-1");
    return NULL;
  }

  va_start(args, fmt);
  /* check for failure*/
#if defined(_MSC_VER)
  rc = vsprintf_s(result + base_length, (size_t)length + 1, fmt, args);
  if (rc < 0) {
    /* handle error, printing the nonzero exit code for debug purposes */
    LOG_DEBUG("vsprintf_s failed with rc=%d", rc);
    free(result);
    if (unto)
      *unto = NULL;
    va_end(args);
    return NULL;
  }
#else
  rc = vsprintf(result + base_length, fmt, args);
  if (rc < 0) {
    LOG_DEBUG("vsprintf failed with rc=%d", rc);
    free(result);
    if (unto)
      *unto = NULL;
    va_end(args);
    return NULL;
  }
#endif
  va_end(args);

  if (unto)
    *unto = result;

  return result;
}
#endif /* !HAVE_JASPRINTF */
