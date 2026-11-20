/**
 * @file c89stringutils_string_extras.c
 * @brief string functions helpful on Linux (and sometimes BSD)
 * are now made available on other platforms (Windows, SunOS, &etc.)
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* clang-format off */
#include "c89stringutils_string_extras.h"
#include "c89stringutils_safecrt.h"
#include "c89stringutils_log.h"

#if defined(_MSC_VER)
#undef C89STRINGUTILS_HAVE_VSNPRINTF
#undef C89STRINGUTILS_HAVE__VSNPRINTF
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h> /* for INT_MAX */

/* stb_sprintf integration for portability on 32/64-bit targets */
#if defined(C89STRINGUTILS_TEST_MOCKS) || defined(__I86__) || defined(_M_I86)
/* Under test mocks or 16-bit DOS, avoid compiling third-party stb_sprintf */
#elif (defined(_WIN32) && defined(C89STRINGUTILS_HAVE_PRINTF_I64D) &&          \
       defined(C89STRINGUTILS_HAVE_PRINTF_IU) &&                               \
       (defined(C89STRINGUTILS_HAVE__VSNPRINTF) ||                             \
        defined(C89STRINGUTILS_HAVE__VSNPRINTF_S)))
/* Sufficient MSVC native features to avoid stb_sprintf fallback */
#elif !defined(C89STRINGUTILS_HAVE_PRINTF_ZU) ||                               \
    !defined(C89STRINGUTILS_HAVE_PRINTF_LLD) ||                                \
    !defined(C89STRINGUTILS_HAVE_VSNPRINTF_NULL) ||                            \
    !defined(C89STRINGUTILS_HAVE_VSNPRINTF)
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_DECORATE(name) c89stringutils_stbsp_##name
#define STB_SPRINTF_NOFLOAT
#include "stb_sprintf.h"
#define C89STRINGUTILS_USE_STB_SPRINTF 1
#endif
/* clang-format on */

#ifndef RSIZE_MAX
#define RSIZE_MAX ((size_t) - 1 >> 1)
#endif

#ifdef C89STRINGUTILS_TEST_MOCKS
/** @brief External mock for malloc
 * @param size size
 */
extern void *mock_malloc(size_t size);
/** @brief External mock for realloc
 * @param ptr pointer
 * @param size size
 */
extern void *mock_realloc(void *ptr, size_t size);
/** @brief External mock for strerror
 * @param errnum error number
 */
extern char *mock_strerror(int errnum);
/** @brief External mock for vsnprintf
 * @param str string
 * @param size size
 * @param format format
 * @param ap va_list
 */
extern int mock_vsnprintf(char *str, size_t size, const char *format,
                          va_list ap);
/** @brief External mock for vscprintf
 * @param format format
 * @param ap va_list
 */
extern int mock_vscprintf(const char *format, va_list ap);
/** @brief External mock for vsnprintf_s
 * @param buffer buffer
 * @param sizeOfBuffer size of buffer
 * @param count count
 * @param format format
 * @param ap va_list
 */
extern int mock_vsnprintf_s(char *buffer, size_t sizeOfBuffer, size_t count,
                            const char *format, va_list ap);
/** @brief External mock for vsprintf
 * @param buffer buffer
 * @param format format
 * @param ap va_list
 */
extern int mock_vsprintf(char *buffer, const char *format, va_list ap);
/** @brief External mock for vfprintf
 * @param stream stream
 * @param format format
 * @param ap va_list
 */
extern int mock_vfprintf(FILE *stream, const char *format, va_list ap);
/** @brief External mock for fprintf
 * @param stream stream
 * @param format format
 * @param ... args
 */
extern int mock_fprintf(FILE *stream, const char *format, ...);
#if defined(C89STRINGUTILS_HAVE_STRERROR_S)
/** @brief External mock for strerror_s
 * @param buffer buffer
 * @param sizeInBytes size
 * @param errnum error number
 */
extern int mock_strerror_s(char *buffer, size_t sizeInBytes, int errnum);
/** @brief Mock macro for strerror_s */
#define strerror_s mock_strerror_s
#endif
#if defined(C89STRINGUTILS_HAVE_STRERROR_R)
#if defined(C89STRINGUTILS_STRERROR_R_CHAR_P)
/** @brief External mock for strerror_r (GNU)
 * @param errnum error number
 * @param buf buffer
 * @param buflen size
 */
extern char *mock_strerror_r(int errnum, char *buf, size_t buflen);
#else
/** @brief External mock for strerror_r (POSIX)
 * @param errnum error number
 * @param buf buffer
 * @param buflen size
 */
extern int mock_strerror_r(int errnum, char *buf, size_t buflen);
#endif
/** @brief Mock macro for strerror_r */
#define strerror_r mock_strerror_r
#endif
#if defined(C89STRINGUTILS_HAVE_VFPRINTF_S)
/** @brief External mock for vfprintf_s
 * @param stream stream
 * @param format format
 * @param ap va_list
 */
extern int mock_vfprintf_s(FILE *stream, const char *format, va_list ap);
/** @brief Mock macro for vfprintf_s */
#define vfprintf_s mock_vfprintf_s
#endif
#if defined(C89STRINGUTILS_HAVE_FPRINTF_S)
/** @brief External mock for fprintf_s
 * @param stream stream
 * @param format format
 * @param ... args
 */
extern int mock_fprintf_s(FILE *stream, const char *format, ...);
/** @brief Mock macro for fprintf_s */
#define fprintf_s mock_fprintf_s
#endif
#if defined(C89STRINGUTILS_HAVE_REALLOCARRAY)
/** @brief External mock for reallocarray
 * @param ptr pointer
 * @param nmemb number of members
 * @param size size
 */
extern void *mock_reallocarray(void *ptr, size_t nmemb, size_t size);
/** @brief Mock macro for reallocarray */
#define reallocarray mock_reallocarray
#endif
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
extern int mock_strncpy_s(char *dest, size_t destsz, const char *src,
                          size_t count);
#define strncpy_s mock_strncpy_s
#endif
#if defined(C89STRINGUTILS_HAVE_STRCPY_S)
extern int mock_strcpy_s(char *dest, size_t destsz, const char *src);
#define strcpy_s mock_strcpy_s
#endif
#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
extern int mock_memcpy_s(void *dest, size_t destsz, const void *src,
                         size_t count);
#define memcpy_s mock_memcpy_s
#endif
/** @brief Mock macro for malloc */
#define malloc mock_malloc
/** @brief Mock macro for realloc */
#define realloc mock_realloc
/** @brief Mock macro for strerror */
#define strerror mock_strerror
/** @brief Mock macro for vsnprintf */
#undef vsnprintf
#define vsnprintf mock_vsnprintf
/** @brief Mock macro for _vsnprintf */
#ifdef _vsnprintf
#undef _vsnprintf
#endif
#define _vsnprintf mock_vsnprintf
/** @brief Mock macro for _vscprintf */
#define _vscprintf mock_vscprintf
/** @brief Mock macro for _vsnprintf_s */
#define _vsnprintf_s mock_vsnprintf_s
/** @brief Mock macro for vsprintf */
#undef vsprintf
#define vsprintf mock_vsprintf
/** @brief Mock macro for vfprintf */
#define vfprintf mock_vfprintf
/** @brief Mock macro for fprintf */
#define fprintf mock_fprintf
#endif

#ifndef VA_COPY
#if defined(__WATCOMC__)
/** @brief Safely copy va_list on OpenWatcom without dereferencing dest */
#define VA_COPY(dest, src) memcpy(&(dest), &(src), sizeof(va_list))
#elif defined(va_copy)
/** @brief Safely copy va_list */
#define VA_COPY(dest, src) va_copy(dest, src)
#elif defined(__va_copy)
/** @brief Safely copy va_list using __va_copy */
#define VA_COPY(dest, src) __va_copy(dest, src)
#elif defined(__GNUC__) || defined(__clang__)
/** @brief Safely copy va_list using compiler builtin */
#define VA_COPY(dest, src) __builtin_va_copy(dest, src)
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER) || defined(__TINYC__)
/** @brief Safely copy va_list using direct assignment */
#define VA_COPY(dest, src) ((dest) = (src))
#else
/** @brief Safely copy va_list using memcpy */
#define VA_COPY(dest, src) memcpy(&(dest), &(src), sizeof(va_list))
#endif
#endif /* ! VA_COPY */

/**
 * @brief Log debug message
 * @param fmt The format string.
 * @param ... The arguments.
 */
C89STRINGUTILS_EXPORT int c89stringutils_log_debug(const char *fmt, ...) {
  int rc = -1;
  int newline_rc = -1;
  va_list args;
  va_start(args, fmt);
#if defined(C89STRINGUTILS_HAVE_VFPRINTF_S)
  rc = vfprintf_s(stderr, fmt, args);
#else
  rc = vfprintf(stderr, fmt, args);
#endif
  va_end(args);
#if defined(C89STRINGUTILS_HAVE_FPRINTF_S)
  newline_rc = fprintf_s(stderr, "\n");
#else
  newline_rc = fprintf(stderr, "\n");
#endif
  if (rc < 0 || newline_rc < 0) {
    return -1;
  }
  return rc + newline_rc;
}

/**
 * @brief Implement vsnprintf for platforms that don't have it natively or lack
 * C99 support.
 * @param buffer The buffer to write to.
 * @param count The maximum number of characters to write.
 * @param format The format string.
 * @param args The va_list of arguments.
 * @return The number of characters written, or a negative value on error.
 */
static int fallback_vsnprintf(char *buffer, size_t count, const char *format,
                              va_list args) C89STRINGUTILS_FORMAT_PRINTF(3, 0);
static int fallback_vsnprintf(char *buffer, size_t count, const char *format,
                              va_list args) {
#if defined(C89STRINGUTILS_USE_STB_SPRINTF)
  return c89stringutils_stbsp_vsnprintf(buffer, (int)count, format, args);
#elif !defined(C89STRINGUTILS_HAVE_VSNPRINTF) &&                               \
    !defined(C89STRINGUTILS_HAVE__VSNPRINTF) &&                                \
    !defined(C89STRINGUTILS_HAVE__VSNPRINTF_S)
#if defined(C89STRINGUTILS_TEST_MOCKS)
  return mock_vsnprintf(buffer, count, format, args);
#else
  {
    char scratch[512];
    int rc = vsprintf(scratch, format, args);
    if (rc >= 0 && buffer != NULL && count > 0) {
      size_t copy_len = ((size_t)rc < count) ? (size_t)rc : (count - 1);
      memcpy(buffer, scratch, copy_len);
      buffer[copy_len] = '\0';
    }
    return rc;
  }
#endif
#else
  int rc = -1;
  if (buffer == NULL || count == 0) {
#if defined(C89STRINGUTILS_HAVE__VSCPRINTF)
    rc = _vscprintf(format, args);
#elif defined(__WATCOMC__)
    {
      char scratch[512];
      rc = vsnprintf(scratch, sizeof(scratch), format, args);
    }
#elif defined(C89STRINGUTILS_HAVE_VSNPRINTF_NULL)
#if defined(C89STRINGUTILS_HAVE_VSNPRINTF)
    rc = vsnprintf(NULL, 0, format, args);
#elif defined(C89STRINGUTILS_HAVE__VSNPRINTF) && !defined(_MSC_VER)
    rc = _vsnprintf(NULL, 0, format, args);
#endif
#endif
    if (rc < 0) {
      LOG_DEBUG("vsnprintf length calculation failed with rc=%d", rc);
    }
    return rc;
  }

#if defined(C89STRINGUTILS_HAVE__VSNPRINTF_S)
  {
    va_list args_copy;
    VA_COPY(args_copy, args);
    rc = _vsnprintf_s(buffer, count, _TRUNCATE, format, args_copy);
    va_end(args_copy);
    if (rc < 0) {
      rc = _vscprintf(format, args);
      if (rc < 0) {
        LOG_DEBUG("_vscprintf failed with rc=%d", rc);
        return rc;
      }
      buffer[count - 1] = '\0';
    }
  }
#elif defined(C89STRINGUTILS_HAVE_VSNPRINTF)
  rc = vsnprintf(buffer, count, format, args);
  if (rc < 0) {
    LOG_DEBUG("vsnprintf failed with rc=%d", rc);
    return rc;
  }
#elif defined(C89STRINGUTILS_HAVE__VSNPRINTF)
  rc = _vsnprintf(buffer, count, format, args);
  if (rc < 0 || (size_t)rc == count) {
#if defined(C89STRINGUTILS_HAVE__VSCPRINTF)
    rc = _vscprintf(format, args);
#endif
    buffer[count - 1] = '\0';
  }
#endif
  return rc;
#endif
}

#if !defined(C89STRINGUTILS_HAVE_STRERROR_S)
C89STRINGUTILS_EXPORT errno_t c89stringutils_strerror_s(char *s,
                                                        rsize_t maxsize,
                                                        errno_t errnum) {
  if (s == NULL || maxsize == 0)
    return (errno_t)C89STRINGUTILS_EINVAL;
#if defined(C89STRINGUTILS_HAVE_STRERROR_R)
#if defined(C89STRINGUTILS_STRERROR_R_CHAR_P)
  {
    char *res = strerror_r(errnum, s, maxsize);
    if (res != s) {
      if (res == NULL) {
        return (errno_t)C89STRINGUTILS_EINVAL;
      }
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
      {
        errno_t rc_cpy;
        rc_cpy = strncpy_s(s, maxsize, res, maxsize - 1);
        if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
          LOG_DEBUG("strncpy_s failed with rc=%d", (int)rc_cpy);
          return rc_cpy;
        }
      }
#else
      strncpy(s, res, maxsize);
      s[maxsize - 1] = '\0';
#endif
    }
    return (errno_t)C89STRINGUTILS_SUCCESS;
  }
#else
  {
    int rc = strerror_r(errnum, s, maxsize);
    if (rc != 0) {
      LOG_DEBUG("strerror_r failed with rc=%d", rc);
      return (errno_t)C89STRINGUTILS_EINVAL;
    }
    return (errno_t)C89STRINGUTILS_SUCCESS;
  }
#endif
#else
  {
    const char *errstr = strerror(errnum);
    if (errstr) {
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
      {
        errno_t rc_cpy;
        rc_cpy = strncpy_s(s, maxsize, errstr, maxsize - 1);
        if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
          LOG_DEBUG("strncpy_s failed with rc=%d", (int)rc_cpy);
          return rc_cpy;
        }
      }
#else
      (strncpy)(s, errstr, maxsize);
      s[maxsize - 1] = '\0';
#endif
      return (errno_t)C89STRINGUTILS_SUCCESS;
    }
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
#endif
}
#endif

#if !defined(C89STRINGUTILS_HAVE_STRCPY_S)
C89STRINGUTILS_EXPORT errno_t c89stringutils_strcpy_s(char *dest,
                                                      rsize_t destsz,
                                                      const char *src) {
  if (dest == NULL) {
    c89stringutils_invoke_constraint_handler_s("strcpy_s: dest is null", NULL,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (destsz == 0) {
    c89stringutils_invoke_constraint_handler_s("strcpy_s: destsz is zero", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (src == NULL) {
    dest[0] = '\0';
    c89stringutils_invoke_constraint_handler_s("strcpy_s: src is null", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  {
    size_t srclen = 0;
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
    srclen = strnlen_s(src, destsz);
#elif defined(C89STRINGUTILS_HAVE_STRNLEN)
    srclen = strnlen(src, destsz);
#else
    while (srclen < destsz && src[srclen] != '\0') {
      srclen++;
    }
#endif

    if (srclen >= destsz) {
      dest[0] = '\0';
      c89stringutils_invoke_constraint_handler_s(
          "strcpy_s: string too long", dest, (errno_t)C89STRINGUTILS_ERANGE);
      return (errno_t)C89STRINGUTILS_ERANGE;
    }

#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
    {
      errno_t rc_cpy;
      rc_cpy = memcpy_s(dest, destsz, src, srclen + 1);
      if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
        LOG_DEBUG("memcpy_s failed with rc=%d", (int)rc_cpy);
        return rc_cpy;
      }
    }
#elif defined(C89STRINGUTILS_HAVE_STRLCPY)
    (strlcpy)(dest, src, destsz);
#else
    memcpy(dest, src, srclen + 1);
#endif
    return (errno_t)C89STRINGUTILS_SUCCESS;
  }
}
#endif

#if !defined(C89STRINGUTILS_HAVE_STRNCPY_S)
C89STRINGUTILS_EXPORT errno_t c89stringutils_strncpy_s(char *dest,
                                                       rsize_t destsz,
                                                       const char *src,
                                                       rsize_t count) {
  if (dest == NULL) {
    c89stringutils_invoke_constraint_handler_s("dest is null", NULL,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (destsz == 0) {
    c89stringutils_invoke_constraint_handler_s("destsz is zero", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (src == NULL) {
    dest[0] = '\0';
    c89stringutils_invoke_constraint_handler_s("src is null", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  {
    size_t srclen = 0;
    size_t to_copy = 0;

    if (count == (rsize_t)-1) { /* _TRUNCATE */
#if defined(C89STRINGUTILS_HAVE_STRNLEN)
      srclen = strnlen(src, destsz);
#elif defined(C89STRINGUTILS_HAVE_STRNLEN_S)
      srclen = strnlen_s(src, destsz);
#else
      while (srclen < destsz && src[srclen] != '\0') {
        srclen++;
      }
#endif
      to_copy = srclen;
      if (to_copy >= destsz) {
        to_copy = destsz - 1;
#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
        {
          errno_t rc_cpy;
          rc_cpy = memcpy_s(dest, destsz, src, to_copy);
          if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
            LOG_DEBUG("memcpy_s failed with rc=%d", (int)rc_cpy);
            return rc_cpy;
          }
        }
#else
        memcpy(dest, src, to_copy);
#endif
        dest[to_copy] = '\0';
        return (errno_t)C89STRINGUTILS_STRUNCATE;
      }
#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
      {
        errno_t rc_cpy;
        rc_cpy = memcpy_s(dest, destsz, src, to_copy + 1);
        if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
          LOG_DEBUG("memcpy_s failed with rc=%d", (int)rc_cpy);
          return rc_cpy;
        }
      }
#else
      memcpy(dest, src, to_copy + 1);
#endif
      return (errno_t)C89STRINGUTILS_SUCCESS;
    } else {
#if defined(C89STRINGUTILS_HAVE_STRNLEN)
      srclen = strnlen(src, count);
#elif defined(C89STRINGUTILS_HAVE_STRNLEN_S)
      srclen = strnlen_s(src, count);
#else
      while (srclen < count && src[srclen] != '\0') {
        srclen++;
      }
#endif
      to_copy = srclen;
      if (to_copy >= destsz) {
        dest[0] = '\0';
        return (errno_t)C89STRINGUTILS_ERANGE;
      }
#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
      {
        errno_t rc_cpy;
        rc_cpy = memcpy_s(dest, destsz, src, to_copy);
        if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
          LOG_DEBUG("memcpy_s failed with rc=%d", (int)rc_cpy);
          return rc_cpy;
        }
      }
#else
      memcpy(dest, src, to_copy);
#endif
      dest[to_copy] = '\0';
      return (errno_t)C89STRINGUTILS_SUCCESS;
    }
  }
}
#endif

#if !defined(C89STRINGUTILS_HAVE_STRCAT_S)
C89STRINGUTILS_EXPORT errno_t c89stringutils_strcat_s(char *dest,
                                                      rsize_t destsz,
                                                      const char *src) {
  if (dest == NULL) {
    c89stringutils_invoke_constraint_handler_s("dest is null", NULL,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (destsz == 0) {
    c89stringutils_invoke_constraint_handler_s("destsz is zero", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (src == NULL) {
    dest[0] = '\0';
    c89stringutils_invoke_constraint_handler_s("src is null", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  {
    size_t destlen = 0;
    size_t srclen = 0;
#if defined(C89STRINGUTILS_HAVE_STRNLEN)
    destlen = strnlen(dest, destsz);
    srclen = strnlen(src, destsz);
#elif defined(C89STRINGUTILS_HAVE_STRNLEN_S)
    destlen = strnlen_s(dest, destsz);
    srclen = strnlen_s(src, destsz);
#else
    while (destlen < destsz && dest[destlen] != '\0') {
      destlen++;
    }
    while (srclen < destsz && src[srclen] != '\0') {
      srclen++;
    }
#endif

    if (destlen >= destsz) {
      dest[0] = '\0';
      return (errno_t)C89STRINGUTILS_EINVAL;
    }
    if (destlen + srclen >= destsz) {
      dest[0] = '\0';
      return (errno_t)C89STRINGUTILS_ERANGE;
    }
#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
    {
      errno_t rc_cpy;
      rc_cpy = memcpy_s(dest + destlen, destsz - destlen, src, srclen + 1);
      if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
        LOG_DEBUG("memcpy_s failed with rc=%d", (int)rc_cpy);
        return rc_cpy;
      }
    }
#elif defined(C89STRINGUTILS_HAVE_STRLCAT)
    (strlcat)(dest, src, destsz);
#else
    memcpy(dest + destlen, src, srclen + 1);
#endif
    return (errno_t)C89STRINGUTILS_SUCCESS;
  }
}
#endif

#if !defined(C89STRINGUTILS_HAVE_STRNCAT_S)
C89STRINGUTILS_EXPORT errno_t c89stringutils_strncat_s(char *dest,
                                                       rsize_t destsz,
                                                       const char *src,
                                                       rsize_t count) {
  if (dest == NULL) {
    c89stringutils_invoke_constraint_handler_s("dest is null", NULL,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (destsz == 0) {
    c89stringutils_invoke_constraint_handler_s("destsz is zero", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  if (src == NULL) {
    dest[0] = '\0';
    c89stringutils_invoke_constraint_handler_s("src is null", dest,
                                               (errno_t)C89STRINGUTILS_EINVAL);
    return (errno_t)C89STRINGUTILS_EINVAL;
  }
  {
    size_t destlen = 0;
    size_t srclen = 0;
    size_t to_copy;

    while (destlen < destsz && dest[destlen] != '\0') {
      destlen++;
    }
    if (destlen >= destsz) {
      dest[0] = '\0';
      return (errno_t)C89STRINGUTILS_EINVAL;
    }

    if (count == (rsize_t)-1) { /* _TRUNCATE */
      while (srclen < destsz && src[srclen] != '\0') {
        srclen++;
      }
      if (destlen + srclen >= destsz) {
        to_copy = destsz - destlen - 1;
        memcpy(dest + destlen, src, to_copy);
        dest[destlen + to_copy] = '\0';
        return (errno_t)C89STRINGUTILS_STRUNCATE;
      }
      memcpy(dest + destlen, src, srclen + 1);
      return (errno_t)C89STRINGUTILS_SUCCESS;
    } else {
      while (srclen < count && src[srclen] != '\0') {
        srclen++;
      }
      to_copy = srclen;
      if (destlen + to_copy >= destsz) {
        dest[0] = '\0';
        return (errno_t)C89STRINGUTILS_ERANGE;
      }
      memcpy(dest + destlen, src, to_copy);
      dest[destlen + to_copy] = '\0';
      return (errno_t)C89STRINGUTILS_SUCCESS;
    }
  }
}
#endif

#if !defined(C89STRINGUTILS_HAVE_VSNPRINTF_S) || defined(_WIN32)
C89STRINGUTILS_EXPORT int c89stringutils_vsnprintf_s(char *s, rsize_t n,
                                                     const char *format,
                                                     va_list arg) {
  if (s == NULL || n == 0)
    return -1;
#if defined(_WIN32) && defined(C89STRINGUTILS_HAVE_VSNPRINTF_S)
  return vsnprintf_s(s, n, (size_t)-1, format, arg);
#elif defined(C89STRINGUTILS_HAVE_VSNPRINTF_S)
  return vsnprintf_s(s, n, format, arg);
#elif defined(C89STRINGUTILS_HAVE__VSNPRINTF_S)
  return _vsnprintf_s(s, n, (size_t)-1, format, arg);
#else
  return fallback_vsnprintf(s, n, format, arg);
#endif
}
#endif

C89STRINGUTILS_EXPORT int
c89stringutils_vsnprintf(char *s, size_t n, const char *format, va_list arg) {
  return fallback_vsnprintf(s, n, format, arg);
}

#if !defined(C89STRINGUTILS_HAVE_SNPRINTF_S) || defined(_WIN32)
C89STRINGUTILS_EXPORT int c89stringutils_snprintf_s(char *s, rsize_t n,
                                                    const char *format, ...) {
  int rc = -1;
  va_list args;
  va_start(args, format);
  rc = c89stringutils_vsnprintf_s(s, n, format, args);
  va_end(args);
  return rc;
}
#endif

C89STRINGUTILS_EXPORT int c89stringutils_snprintf(char *s, size_t n,
                                                  const char *format, ...) {
  int rc = -1;
  va_list args;
  va_start(args, format);
  rc = c89stringutils_vsnprintf(s, n, format, args);
  va_end(args);
  return rc;
}

#ifdef vsnprintf
#undef vsnprintf
#endif
/**
 * @brief Macro replacing vsnprintf with fallback_vsnprintf
 * @param buffer The buffer
 * @param count The size of the buffer
 * @param format The format string
 * @param args The arguments
 */
#define vsnprintf(buffer, count, format, args)                                 \
  fallback_vsnprintf(buffer, count, format, args)

#if !defined(C89STRINGUTILS_HAVE__STRNICMP) &&                                 \
    !defined(C89STRINGUTILS_HAVE_STRNCASECMP)
/**
 * @brief Compare at most n characters of two strings, ignoring case.
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @param n The maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 */
C89STRINGUTILS_EXPORT int c89stringutils_strncasecmp(const char *s1,
                                                     const char *s2, size_t n) {
  if (s1 == s2)
    return 0;
  if (s1 == NULL)
    return -1;
  if (s2 == NULL)
    return 1;
  while (n-- > 0) {
    int c1 = tolower((unsigned char)*s1);
    int c2 = tolower((unsigned char)*s2);
    if (c1 != c2)
      return c1 - c2;
    if (c1 == '\0')
      break;
    s1++;
    s2++;
  }
  return 0;
}
#endif

#if !defined(C89STRINGUTILS_HAVE__STRICMP) &&                                  \
    !defined(C89STRINGUTILS_HAVE_STRCASECMP)
/**
 * @brief Compare two strings, ignoring case.
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 */
C89STRINGUTILS_EXPORT int c89stringutils_strcasecmp(const char *s1,
                                                    const char *s2) {
  if (s1 == s2)
    return 0;
  if (s1 == NULL)
    return -1;
  if (s2 == NULL)
    return 1;
  while (*s1 && *s2) {
    int c1 = tolower((unsigned char)*s1);
    int c2 = tolower((unsigned char)*s2);
    if (c1 != c2)
      return c1 - c2;
    s1++;
    s2++;
  }
  return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}
#endif

/* Helper to remove const without triggering -Wcast-qual or truncating 16-bit
 * DOS far pointers */
#if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
#define C89STRINGUTILS_UINTPTR_T unsigned long
#else
#define C89STRINGUTILS_UINTPTR_T size_t
#endif
#define C89STRINGUTILS_UNCONST(type, ptr)                                      \
  ((type)(C89STRINGUTILS_UINTPTR_T)(const void *)(ptr))

#if !defined(C89STRINGUTILS_HAVE_STRNSTR)
/**
 * @brief Locate a substring in a string, looking at no more than len
 * characters.
 * @param buffer The string to search.
 * @param target The substring to find.
 * @param bufferLength The maximum number of characters to search.
 * @return A pointer to the first occurrence of little in big, or NULL if not
 * found.
 */
C89STRINGUTILS_EXPORT char *c89stringutils_strnstr(const char *buffer,
                                                   const char *target,
                                                   size_t bufferLength) {
  size_t targetLength;
  const char *start;
  size_t remaining;
  int rc = -1;
  if (buffer == NULL || target == NULL) {
    LOG_DEBUG("buffer or target is NULL");
    return NULL;
  }
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
  targetLength = strnlen_s(target, RSIZE_MAX);
#else
  targetLength = strlen(target);
#endif
  if (targetLength == 0) {
    return C89STRINGUTILS_UNCONST(char *, buffer);
  }
  remaining = bufferLength;
  for (start = buffer; *start && remaining >= targetLength;
       start++, remaining--) {
    if (*start == *target) {
      rc = strncmp(start + 1, target + 1, targetLength - 1);
      if (rc == 0) {
        return C89STRINGUTILS_UNCONST(char *, start);
      }
    }
  }
  return NULL;
}
#endif

#if !defined(C89STRINGUTILS_HAVE_STRCASESTR)
/**
 * @brief Locate a substring in a string, ignoring case.
 * @param h The string to search.
 * @param n The substring to find.
 * @return A pointer to the first occurrence of little in big, or NULL if not
 * found.
 */
C89STRINGUTILS_EXPORT char *c89stringutils_strcasestr(const char *h,
                                                      const char *n) {
  size_t l;
  int cmp;
  if (h == NULL || n == NULL) {
    LOG_DEBUG("h or n is NULL");
    return NULL;
  }
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
  l = strnlen_s(n, RSIZE_MAX);
#else
  l = strlen(n);
#endif
  if (l == 0) {
    return C89STRINGUTILS_UNCONST(char *, h);
  }
  for (; *h; h++) {
    cmp = c89stringutils_strncasecmp(h, n, l);
    if (cmp == 0) {
      return C89STRINGUTILS_UNCONST(char *, h);
    }
  }
  return NULL;
}
#endif

/**
 * @brief Get the length of a string describing an error number.
 * @param errnum The error number.
 * @return The length of the string describing the error.
 */
C89STRINGUTILS_EXPORT size_t c89stringutils_strerrorlen_s(errno_t errnum) {
/** @brief Error code for null pointer */
#ifndef ESNULLP
#define ESNULLP (400) /* null ptr                    */
#endif

/** @brief Error code for wrong size */
#ifndef ESLEWRNG
#define ESLEWRNG (410) /* wrong size                */
#endif

/** @brief Last error code */
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
    return (size_t)(len_errmsgs_s[errnum - ESNULLP] - 1);
  } else {
#if defined(C89STRINGUTILS_HAVE_STRERROR_S)
    char errbuf[256];
    int rc = strerror_s(errbuf, sizeof(errbuf), errnum);
    if (rc != 0) {
      LOG_DEBUG("strerror_s failed with rc=%d", rc);
      return 0;
    }
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
    return strnlen_s(errbuf, sizeof(errbuf));
#else
    return strlen(errbuf);
#endif
#elif defined(C89STRINGUTILS_HAVE_STRERROR_R)
    char errbuf[256];
#if defined(C89STRINGUTILS_STRERROR_R_CHAR_P)
    char *res = strerror_r(errnum, errbuf, sizeof(errbuf));
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
    return res ? strnlen_s(res, sizeof(errbuf)) : 0;
#else
    return res ? strlen(res) : 0;
#endif
#else
    int rc = -1;
    errbuf[0] = '\0';
    rc = strerror_r(errnum, errbuf, sizeof(errbuf));
    errbuf[sizeof(errbuf) - 1] = '\0';
    if (rc != 0) {
      return 0;
    }
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
    return strnlen_s(errbuf, sizeof(errbuf));
#else
    return strlen(errbuf);
#endif
#endif
#else
    const char *buf = strerror(errnum);
#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
    return buf ? strnlen_s(buf, RSIZE_MAX) : 0;
#else
    return buf ? strlen(buf) : 0;
#endif
#endif
  }
}

#ifndef INIT_SZ
/** @brief Initial allocation size for strings */
#define INIT_SZ 128
#endif

/**
 * @brief Write formatted output to a dynamically allocated string using a
 * va_list.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ap The va_list of arguments.
 * @return The number of characters printed, or -1 on error.
 */
C89STRINGUTILS_EXPORT int c89stringutils_vasprintf(char **str, const char *fmt,
                                                   va_list ap) {
  int rc = -1;
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

  if (rc < 0) { /* Bad length or legacy compiler truncation */
    size_t current_sz = INIT_SZ;
    while (rc < 0 && current_sz <= C89STRINGUTILS_VASPRINTF_MAX_SZ) {
      char *t;
      current_sz *= 2;
      t = (char *)realloc(string, current_sz);
      if (t == NULL) {
        free(string);
        rc = -1;
        goto fail;
      }
      string = t;
      VA_COPY(ap2, ap);
      rc = vsnprintf(string, current_sz, fmt, ap2);
      va_end(ap2);
    }
    if (rc < 0 || (size_t)rc >= current_sz) {
      free(string);
      rc = -1;
      goto fail;
    }
    *str = string;
    return 0;
  } else if (rc == INT_MAX) { /* Bad length */
    free(string);
    rc = -1;
    goto fail;
  } else if (rc < INIT_SZ) { /* succeeded with initial alloc */
    *str = string;
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
    if (rc < 0) { /* failed with realloc'ed string */
      free(newstr);
      rc = -1;
      goto fail;
    } else if ((size_t)rc >= len) {
      free(newstr);
      rc = -1;
      goto fail;
    }
    *str = newstr;
  }
  return 0;

fail:
#if defined(C89STRINGUTILS_HAVE_STRERROR_S)
{
  char errbuf[256];
  int err_rc = strerror_s(errbuf, sizeof(errbuf), errno);
  if (err_rc != 0) {
    LOG_DEBUG("strerror_s failed with rc=%d", err_rc);
    errbuf[0] = '\0';
  }
  LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, errbuf);
}
#elif defined(C89STRINGUTILS_HAVE_STRERROR_R)
{
  char errbuf[256];
#if defined(C89STRINGUTILS_STRERROR_R_CHAR_P)
  char *res = strerror_r(errno, errbuf, sizeof(errbuf));
  LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, res ? res : "");
#else
  int err_rc;
  errbuf[0] = '\0';
  err_rc = strerror_r(errno, errbuf, sizeof(errbuf));
  errbuf[sizeof(errbuf) - 1] = '\0';
  if (err_rc != 0) {
    errbuf[0] = '\0';
  }
  LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, errbuf);
#endif
}
#else
{
  const char *errstr = strerror(errno);
  if (errstr != NULL) {
    LOG_DEBUG("vasprintf failed with rc=%d, error=%s", rc, errstr);
  } else {
    LOG_DEBUG("vasprintf failed with rc=%d, error=(null)", rc);
  }
}
#endif
  *str = NULL;
  errno = ENOMEM;
  return -1;
}

/**
 * @brief Write formatted output to a dynamically allocated string.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return The number of characters printed, or -1 on error.
 */
C89STRINGUTILS_EXPORT int c89stringutils_asprintf(char **str, const char *fmt,
                                                  ...) {
  int rc = -1;
  va_list ap;

  if (str == NULL || fmt == NULL) {
    LOG_DEBUG("str or fmt is NULL");
    return -1;
  }

  *str = NULL;
  va_start(ap, fmt);
  rc = c89stringutils_vasprintf(str, fmt, ap);
  va_end(ap);

  if (rc < 0) {
#if defined(C89STRINGUTILS_HAVE_STRERROR_S)
    char errbuf[256];
    int err_rc = strerror_s(errbuf, sizeof(errbuf), errno);
    if (err_rc != 0) {
      LOG_DEBUG("strerror_s failed with rc=%d", err_rc);
      errbuf[0] = '\0';
    }
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, errbuf);
#elif defined(C89STRINGUTILS_HAVE_STRERROR_R)
    char errbuf[256];
#if defined(C89STRINGUTILS_STRERROR_R_CHAR_P)
    char *res = strerror_r(errno, errbuf, sizeof(errbuf));
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, res ? res : "");
#else
    int err_rc;
    errbuf[0] = '\0';
    err_rc = strerror_r(errno, errbuf, sizeof(errbuf));
    errbuf[sizeof(errbuf) - 1] = '\0';
    if (err_rc != 0) {
      errbuf[0] = '\0';
    }
    LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, errbuf);
#endif
#else
    const char *errstr = strerror(errno);
    if (errstr != NULL) {
      LOG_DEBUG("asprintf failed with rc=%d, error=%s", rc, errstr);
    } else {
      LOG_DEBUG("asprintf failed with rc=%d, error=(null)", rc);
    }
#endif
  }

  return rc;
}

static int c89stringutils_vjasprintf(char **unto, const char *fmt, va_list args)
    C89STRINGUTILS_FORMAT_PRINTF(2, 0);
static int c89stringutils_vjasprintf(char **unto, const char *fmt,
                                     va_list args) {
  char *new_part = NULL;
  char *result;
  size_t base_length;
  size_t new_length;
  int rc = -1;

  rc = c89stringutils_vasprintf(&new_part, fmt, args);

  if (rc != 0) {
    return -1;
  }

  if (*unto == NULL) {
    *unto = new_part;
    return 0;
  }

#if defined(C89STRINGUTILS_HAVE_STRNLEN_S)
  base_length = strnlen_s(*unto, RSIZE_MAX);
  new_length = strnlen_s(new_part, RSIZE_MAX);
#else
  base_length = strlen(*unto);
  new_length = strlen(new_part);
#endif

#if defined(C89STRINGUTILS_HAVE_REALLOCARRAY)
  result = (char *)reallocarray(*unto, 1, base_length + new_length + 1);
#else
  result = (char *)realloc(*unto, base_length + new_length + 1);
#endif
  if (result == NULL) {
    LOG_DEBUG("realloc failed with rc=-1");
    free(new_part);
    return -1;
  }

#if defined(C89STRINGUTILS_HAVE_MEMCPY_S)
  {
    errno_t rc_cpy;
    rc_cpy = memcpy_s(result + base_length, new_length + 1, new_part,
                      new_length + 1);
    if (rc_cpy != (errno_t)C89STRINGUTILS_SUCCESS) {
      free(new_part);
      free(result);
      *unto = NULL;
      return -1;
    }
  }
#else
  memcpy(result + base_length, new_part, new_length + 1);
#endif
  free(new_part);
  *unto = result;
  return 0;
}

/**
 * @brief `jasprintf`, a version of `asprintf` that concatenates on successive
 * calls: char *s = NULL; c89stringutils_jasprintf(&s, "foo%s", "bar");
 * c89stringutils_jasprintf(&s, "can%s", "haz"); free(s);
 * @param unto The string to append to.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return 0 on success, or -1 on error.
 */
C89STRINGUTILS_EXPORT int c89stringutils_jasprintf(char **unto, const char *fmt,
                                                   ...) {
  va_list args;
  int rc = -1;

  if (unto == NULL || fmt == NULL) {
    LOG_DEBUG("unto or fmt is NULL");
    return -1;
  }

  va_start(args, fmt);
  rc = c89stringutils_vjasprintf(unto, fmt, args);
  va_end(args);

  return rc;
}

#if !defined(C89STRINGUTILS_HAVE_VASPRINTF) ||                                 \
    !defined(C89STRINGUTILS_HAVE_ASPRINTF)
/**
 * @brief Write formatted output to a dynamically allocated string using a
 * va_list.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ap The va_list of arguments.
 * @return The number of characters printed, or -1 on error.
 */
C89STRINGUTILS_EXPORT int vasprintf(char **str, const char *fmt, va_list ap) {
  return c89stringutils_vasprintf(str, fmt, ap);
}

/**
 * @brief Write formatted output to a dynamically allocated string.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return The number of characters printed, or -1 on error.
 */
C89STRINGUTILS_EXPORT int asprintf(char **str, const char *fmt, ...) {
  int rc = -1;
  va_list args;
  va_start(args, fmt);
  rc = c89stringutils_vasprintf(str, fmt, args);
  va_end(args);
  return rc;
}
#endif

/**
 * @brief `jasprintf`, a version of `asprintf` that concatenates on successive
 * calls.
 * @param unto The string to append to.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return 0 on success, or -1 on error.
 */
C89STRINGUTILS_EXPORT int jasprintf(char **unto, const char *fmt, ...) {
  va_list args;
  int rc = -1;

  if (unto == NULL || fmt == NULL) {
    return -1;
  }

  va_start(args, fmt);
  rc = c89stringutils_vjasprintf(unto, fmt, args);
  va_end(args);

  return rc;
}

/**
 * @brief Free heap memory allocated by c89stringutils dynamic string functions.
 * @param ptr Pointer to the allocated buffer to free, or NULL.
 */
C89STRINGUTILS_EXPORT void c89stringutils_free(void *ptr) { free(ptr); }

/* Prevent macro leakage in amalgamation builds */
#ifdef snprintf
#undef snprintf
#endif
#ifdef vsnprintf
#undef vsnprintf
#endif
