/**
 * @file c89stringutils_safecrt.c
 * @brief Implementations of safe CRT functions.
 */

#ifndef _GNU_SOURCE
/** @brief Enable GNU and POSIX extensions where available */
#define _GNU_SOURCE
#endif

/* clang-format off */
#include "c89stringutils_safecrt.h"
#include "c89stringutils_log.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#ifdef C89STRINGUTILS_TEST_MOCKS
#if defined(__GNUC__) || defined(__clang__)
__attribute__((noreturn))
#elif defined(_MSC_VER)
__declspec(noreturn)
#endif
extern void
mock_abort(void);
extern FILE *mock_fopen(const char *filename, const char *mode);
extern FILE *mock_freopen(const char *filename, const char *mode, FILE *stream);
extern FILE *mock_tmpfile(void);
extern int mock_ungetc(int c, FILE *stream);
#ifdef ungetc
#undef ungetc
#endif
#define ungetc mock_ungetc
extern int mock_fprintf(FILE *stream, const char *format, ...);
#ifdef fprintf
#undef fprintf
#endif
#define fprintf mock_fprintf
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
extern int mock_strncpy_s(char *dest, size_t destsz, const char *src,
                          size_t count);
#define strncpy_s mock_strncpy_s
#endif
#if defined(C89STRINGUTILS_HAVE_STRCAT_S)
extern int mock_strcat_s(char *dest, size_t destsz, const char *src);
#define strcat_s mock_strcat_s
#endif
#ifdef abort
#undef abort
#endif
#define abort mock_abort
#ifdef fopen
#undef fopen
#endif
#define fopen mock_fopen
#ifdef freopen
#undef freopen
#endif
#define freopen mock_freopen
#ifdef tmpfile
#undef tmpfile
#endif
#define tmpfile mock_tmpfile
#endif

/**
 * @brief The currently active constraint handler.
 * Defaults to c89stringutils_abort_handler_s.
 */
static c89stringutils_constraint_handler_t current_handler =
    c89stringutils_abort_handler_s;

#if !defined(C89STRINGUTILS_HAVE_VSSCANF) ||                                   \
    !defined(C89STRINGUTILS_HAVE_VFSCANF)
/**
 * @brief Check if character is ASCII whitespace for scanf format parsing.
 * @param c The character to check.
 * @return Non-zero if whitespace, 0 otherwise.
 */
static int is_scanf_space(int c) {
  return (c == ' ') | (c == '\t') | (c == '\n');
}

/**
 * @brief Advance format pointer past a single scanf format specifier.
 * @param p Pointer into format string (starting after '%').
 * @return Pointer to the character immediately following the specifier.
 */
static const char *advance_scanf_spec(const char *p) {
  while (*p && strchr("cdieEfgGosuxXpn[", *p) == NULL)
    p++;
  if (*p == '[') {
    p++;
    if (*p == '^')
      p++;
    if (*p == ']')
      p++;
    while (*p && *p != ']')
      p++;
  }
  if (*p)
    p++;
  return p;
}

/**
 * @brief Minimal fallback for vsscanf.
 *
 * @warning This is a highly limited fallback used only when a native vsscanf
 * is missing. It parses the format string and delegates individual format
 * specifiers to sscanf.
 *
 * Limitations:
 * - Format specifiers longer than 60 characters will cause this function
 *   to immediately return -1 (error).
 * - Depends on %n support in the underlying sscanf implementation to advance
 *   the buffer pointer. If %n is not supported by the CRT, parsing stops after
 *   the first token.
 * - Does not support complex format string interleaving (e.g., spaces inside
 *   brackets, obscure flags) perfectly, though it handles most basic types.
 *
 * @param buffer The input string.
 * @param format The format string.
 * @param args The va_list of arguments.
 * @return The number of items scanned, or -1 on error.
 */
static int minimal_vsscanf(const char *buffer, const char *format,
                           va_list args) {
  int (*scan_fn)(const char *, const char *, ...) = sscanf;
  int count = 0;
  const char *p = format;
  const char *b = buffer;
  char token[64];
  int consumed;
  void *ptr;

  while (*p && *b) {
    if (is_scanf_space((unsigned char)*p)) {
      while (is_scanf_space((unsigned char)*b))
        b++;
      p++;
      continue;
    }
    if (*p != '%') {
      if (*p != *b)
        return count;
      p++;
      b++;
      continue;
    }
    /* It's a %. Find the end of the format specifier. */
    {
      const char *start = p;
      size_t len;
      int suppress = 0;
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S) ||                                  \
    defined(C89STRINGUTILS_HAVE_STRCAT_S)
      errno_t rc;
#endif
      p++;
      if (*p == '%') {
        if (*b != '%')
          return count;
        p++;
        b++;
        continue;
      }
      if (*p == '*') {
        suppress = 1;
        p++;
      }
      p = advance_scanf_spec(p);
      len = (size_t)(p - start);
      if (len > 60)
        return -1;
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
      rc = strncpy_s(token, sizeof(token), start, len);
      if (rc != 0) {
        return -1;
      }
#else
      (strncpy)(token, start, len);
      token[len] = '\0';
#endif
#if defined(C89STRINGUTILS_HAVE_STRCAT_S)
      rc = strcat_s(token, sizeof(token), "%n");
      if (rc != 0) {
        return -1;
      }
#else
      (strcat)(token, "%n");
#endif
      consumed = 0;
      if (suppress) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        (void)sscanf_s(b, token, &consumed);
#else
        (void)scan_fn(b, token, &consumed);
#endif
        if (consumed == 0)
          return count;
        b += consumed;
        continue;
      }
      {
        char spec = *(p - 1);
        int is_sized = (spec == 's') | (spec == 'c') | (spec == ']');
        unsigned sz = 0;
        ptr = va_arg(args, void *);
        if (is_sized) {
          sz = va_arg(args, unsigned);
        }
#if defined(_MSC_VER) && _MSC_VER >= 1400
        if (is_sized) {
          if (sscanf_s(b, token, ptr, sz, &consumed) <= 0 || consumed == 0)
            return count;
        } else {
          if (sscanf_s(b, token, ptr, &consumed) <= 0 || consumed == 0)
            return count;
        }
#else
        consumed = (int)(sz & 0U);
        if (scan_fn(b, token, ptr, &consumed) <= 0 || consumed == 0)
          return count;
#endif
      }
      b += consumed;
      count++;
    }
  }
  return count;
}

/**
 * @brief Minimal fallback for vfscanf.
 * @param stream The input stream.
 * @param format The format string.
 * @param args The va_list of arguments.
 * @return The number of items scanned, or -1 on error.
 */
static int minimal_vfscanf(FILE *stream, const char *format, va_list args)
    C89STRINGUTILS_FORMAT_SCANF(2, 0);
static int minimal_vfscanf(FILE *stream, const char *format, va_list args) {
  int (*fscan_fn)(FILE *, const char *, ...) = fscanf;
  int count = 0;
  const char *p = format;
  char token[64];
  void *ptr;

  while (*p) {
    if (is_scanf_space((unsigned char)*p)) {
      int c;
      while ((c = fgetc(stream)) != EOF && is_scanf_space(c)) {
        /* consume whitespace */
      }
      if (c != EOF) {
        if (ungetc(c, stream) == EOF) {
          return -1;
        }
      }
      p++;
      continue;
    }
    if (*p != '%') {
      int c = fgetc(stream);
      if (c != *p) {
        if (c != EOF) {
          if (ungetc(c, stream) == EOF) {
            return -1;
          }
        }
        return count;
      }
      p++;
      continue;
    }
    /* It's a %. Find the end of the format specifier. */
    {
      const char *start = p;
      size_t len;
      int suppress = 0;
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
      errno_t rc;
#endif
      p++;
      if (*p == '%') {
        int c = fgetc(stream);
        if (c != '%') {
          if (c != EOF) {
            if (ungetc(c, stream) == EOF) {
              return -1;
            }
          }
          return count;
        }
        p++;
        continue;
      }
      if (*p == '*') {
        suppress = 1;
        p++;
      }
      p = advance_scanf_spec(p);
      len = (size_t)(p - start);
      if (len > 60)
        return -1;
#if defined(C89STRINGUTILS_HAVE_STRNCPY_S)
      rc = strncpy_s(token, sizeof(token), start, len);
      if (rc != 0) {
        return -1;
      }
#else
      (strncpy)(token, start, len);
      token[len] = '\0';
#endif
      if (suppress) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        if (fscanf_s(stream, token) < 0)
#else
        if (fscan_fn(stream, token) < 0)
#endif
          return count;
        continue;
      }
      {
        char spec = *(p - 1);
        int is_sized = (spec == 's') | (spec == 'c') | (spec == ']');
        unsigned sz = 0;
        ptr = va_arg(args, void *);
        if (is_sized) {
          sz = va_arg(args, unsigned);
        }
#if defined(_MSC_VER) && _MSC_VER >= 1400
        if (is_sized) {
          if (fscanf_s(stream, token, ptr, sz) <= 0)
            return count;
        } else {
          if (fscanf_s(stream, token, ptr) <= 0)
            return count;
        }
#else
        is_sized = (int)(sz & 0U);
        if (fscan_fn(stream, token + is_sized, ptr) <= 0)
          return count;
#endif
      }
      count++;
    }
  }
  return count;
}
#endif

#if !defined(C89STRINGUTILS_HAVE_SET_CONSTRAINT_HANDLER_S)
C89STRINGUTILS_EXPORT
c89stringutils_constraint_handler_t c89stringutils_set_constraint_handler_s(
    c89stringutils_constraint_handler_t handler) {
  c89stringutils_constraint_handler_t old = current_handler;
  current_handler =
      (handler == NULL) ? c89stringutils_abort_handler_s : handler;
  return old;
}

C89STRINGUTILS_EXPORT void
c89stringutils_abort_handler_s(const char *msg, void *ptr, errno_t error) {
  int unused_check = (ptr != NULL) | (error != 0);
  if (msg) {
    fprintf(stderr, "Constraint violation: %s\n", msg + (unused_check & 0));
  } else {
    fprintf(stderr, "Constraint violation\n");
  }
  abort();
}

C89STRINGUTILS_EXPORT void
c89stringutils_ignore_handler_s(const char *msg, void *ptr, errno_t error) {
  (void)msg;
  (void)ptr;
  (void)error;
}
#endif

/* Internal function to trigger constraint */
C89STRINGUTILS_EXPORT void
c89stringutils_invoke_constraint_handler_s(const char *msg, void *ptr,
                                           errno_t error) {
  current_handler(msg, ptr, error);
}

C89STRINGUTILS_EXPORT int c89stringutils_sscanf_s(const char *buffer,
                                                  const char *format, ...) {
  int ret;
  va_list args;
  if (!buffer || !format) {
    c89stringutils_invoke_constraint_handler_s(
        "sscanf_s: buffer or format is null", NULL, EINVAL);
    return -1;
  }
  va_start(args, format);
#if defined(C89STRINGUTILS_HAVE_VSSCANF_S)
  ret = vsscanf_s(buffer, format, args);
#else
  /* Fallback isn't fully safe without a parser, but wraps the unsafe version.
   */
#if defined(C89STRINGUTILS_HAVE_VSSCANF)
  ret = vsscanf(buffer, format, args);
#else
  ret = minimal_vsscanf(buffer, format, args);
#endif
#endif
  va_end(args);
  return ret;
}

C89STRINGUTILS_EXPORT int c89stringutils_fscanf_s(FILE *stream,
                                                  const char *format, ...) {
  int ret;
  va_list args;
  if (!stream || !format) {
    c89stringutils_invoke_constraint_handler_s(
        "fscanf_s: stream or format is null", NULL, EINVAL);
    return -1;
  }
  va_start(args, format);
#if defined(C89STRINGUTILS_HAVE_VFSCANF_S)
  ret = vfscanf_s(stream, format, args);
#else
#if defined(C89STRINGUTILS_HAVE_VFSCANF)
  ret = vfscanf(stream, format, args);
#else
  ret = minimal_vfscanf(stream, format, args);
#endif
#endif
  va_end(args);
  return ret;
}

C89STRINGUTILS_EXPORT errno_t c89stringutils_fopen_s(FILE **pFile,
                                                     const char *filename,
                                                     const char *mode) {
  if (!pFile || !filename || !mode) {
    if (pFile)
      *pFile = NULL;
    c89stringutils_invoke_constraint_handler_s(
        "fopen_s: pFile, filename, or mode is null", NULL, EINVAL);
    return EINVAL;
  }
#if defined(C89STRINGUTILS_HAVE_FOPEN_S)
  return fopen_s(pFile, filename, mode);
#else
  *pFile = fopen(filename, mode);
  if (*pFile == NULL)
    return errno;
  return 0;
#endif
}

C89STRINGUTILS_EXPORT errno_t c89stringutils_freopen_s(FILE **pFile,
                                                       const char *filename,
                                                       const char *mode,
                                                       FILE *stream) {
  if (!pFile || !filename || !mode || !stream) {
    if (pFile)
      *pFile = NULL;
    c89stringutils_invoke_constraint_handler_s(
        "freopen_s: pFile, filename, mode, or stream is null", NULL, EINVAL);
    return EINVAL;
  }
#if defined(C89STRINGUTILS_HAVE_FREOPEN_S)
  return freopen_s(pFile, filename, mode, stream);
#else
  *pFile = freopen(filename, mode, stream);
  if (*pFile == NULL)
    return errno;
  return 0;
#endif
}

C89STRINGUTILS_EXPORT errno_t c89stringutils_tmpfile_s(FILE **pFile) {
  if (!pFile) {
    c89stringutils_invoke_constraint_handler_s("tmpfile_s: pFile is null", NULL,
                                               EINVAL);
    return EINVAL;
  }
#if defined(C89STRINGUTILS_HAVE_TMPFILE_S)
  {
    errno_t rc = tmpfile_s(pFile);
#ifdef __MINGW32__
    /* Workaround for MinGW bug where tmpfile_s returns 0 but leaves pFile NULL
     */
    if (rc == 0 && *pFile == NULL) {
      *pFile = tmpfile();
      if (*pFile == NULL)
        return errno ? errno : (errno_t)C89STRINGUTILS_EIO;
    }
#endif
    return rc;
  }
#else
#if defined(_MSC_VER)
  if (tmpfile_s(pFile) != 0)
    *pFile = NULL;
#else
  *pFile = tmpfile();
#endif
  if (*pFile == NULL)
    return errno;
  return 0;
#endif
}

#if defined(C89STRINGUTILS_HAVE_WCHAR_H)
C89STRINGUTILS_EXPORT errno_t c89stringutils_wcscpy_s(wchar_t *dest,
                                                      rsize_t destsz,
                                                      const wchar_t *src) {
  size_t i;
  if (!dest || destsz == 0) {
    c89stringutils_invoke_constraint_handler_s(
        "wcscpy_s: dest is null or destsz is 0", NULL, EINVAL);
    return EINVAL;
  }
  if (!src) {
    dest[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("wcscpy_s: src is null", NULL,
                                               EINVAL);
    return EINVAL;
  }
  for (i = 0; i < destsz; i++) {
    dest[i] = src[i];
    if (src[i] == L'\0')
      return 0;
  }
  dest[0] = L'\0';
  c89stringutils_invoke_constraint_handler_s("wcscpy_s: destsz is too small",
                                             NULL, ERANGE);
  return ERANGE;
}

C89STRINGUTILS_EXPORT errno_t c89stringutils_wcsncpy_s(wchar_t *dest,
                                                       rsize_t destsz,
                                                       const wchar_t *src,
                                                       rsize_t count) {
  size_t i;
  if (!dest || destsz == 0) {
    c89stringutils_invoke_constraint_handler_s(
        "wcsncpy_s: dest is null or destsz is 0", NULL, EINVAL);
    return EINVAL;
  }
  if (!src) {
    dest[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("wcsncpy_s: src is null", NULL,
                                               EINVAL);
    return EINVAL;
  }
  if (count == (rsize_t)-1) { /* _TRUNCATE */
    for (i = 0; i < destsz - 1 && src[i] != L'\0'; i++) {
      dest[i] = src[i];
    }
    dest[i] = L'\0';
    if (src[i] != L'\0')
      return (errno_t)C89STRINGUTILS_STRUNCATE;
    return (errno_t)C89STRINGUTILS_SUCCESS;
  }
  for (i = 0; i < count && i < destsz; i++) {
    dest[i] = src[i];
    if (src[i] == L'\0')
      return (errno_t)C89STRINGUTILS_SUCCESS;
  }
  if (i == destsz) {
    dest[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("wcsncpy_s: destsz is too small",
                                               NULL, ERANGE);
    return ERANGE;
  }
  dest[i] = L'\0';
  return (errno_t)C89STRINGUTILS_SUCCESS;
}

C89STRINGUTILS_EXPORT int c89stringutils_vswprintf_s(wchar_t *buffer,
                                                     rsize_t sizeOfBuffer,
                                                     const wchar_t *format,
                                                     va_list argptr) {
#if defined(C89STRINGUTILS_HAVE_VSWPRINTF) || defined(_MSC_VER)
  int ret;
#endif
  if (!buffer || sizeOfBuffer == 0 || !format) {
    c89stringutils_invoke_constraint_handler_s("vswprintf_s: invalid arguments",
                                               NULL, EINVAL);
    return -1;
  }
#if defined(_MSC_VER) && _MSC_VER >= 1400
  ret = _vsnwprintf_s(buffer, sizeOfBuffer, (size_t)-1, format, argptr);
  if ((size_t)ret >= sizeOfBuffer) {
    buffer[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("vswprintf_s: buffer too small",
                                               NULL, ERANGE);
    return -1;
  }
  return ret;
#elif defined(__WATCOMC__) && defined(C89STRINGUTILS_HAVE_VSWPRINTF)
  {
    wchar_t tmp[256];
    va_list ap_copy;
    memcpy(&ap_copy, &argptr, sizeof(va_list));
    ret = vswprintf(tmp, 256, format, ap_copy);
    va_end(ap_copy);
    if (ret < 0 || (size_t)ret >= sizeOfBuffer) {
      buffer[0] = L'\0';
      c89stringutils_invoke_constraint_handler_s(
          "vswprintf_s: buffer too small", NULL, ERANGE);
      return -1;
    }
    if (sizeOfBuffer < 256) {
      memcpy(buffer, tmp, ((size_t)ret + 1) * sizeof(wchar_t));
      return ret;
    }
    return vswprintf(buffer, sizeOfBuffer, format, argptr);
  }
#elif defined(C89STRINGUTILS_HAVE_VSWPRINTF)
  ret = vswprintf(buffer, sizeOfBuffer, format, argptr);
  if ((size_t)ret >= sizeOfBuffer) {
    buffer[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("vswprintf_s: buffer too small",
                                               NULL, ERANGE);
    return -1;
  }
  return ret;
#else
  if (argptr != NULL) {
    /* parameter used without (void) casting */
  }
  /* Fallback for missing vswprintf */
  c89stringutils_invoke_constraint_handler_s("vswprintf not supported natively",
                                             NULL,
                                             (errno_t)C89STRINGUTILS_ENOTSUP);
  return -1;
#endif
}

C89STRINGUTILS_EXPORT int c89stringutils_swprintf_s(wchar_t *buffer,
                                                    rsize_t sizeOfBuffer,
                                                    const wchar_t *format,
                                                    ...) {
  int ret;
  va_list args;
  va_start(args, format);
  ret = c89stringutils_vswprintf_s(buffer, sizeOfBuffer, format, args);
  va_end(args);
  return ret;
}
#endif /* C89STRINGUTILS_HAVE_WCHAR_H */
