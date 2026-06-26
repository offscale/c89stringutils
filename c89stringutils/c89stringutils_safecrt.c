/**
 * @file c89stringutils_safecrt.c
 * @brief Implementations of safe CRT functions.
 */

/* clang-format off */
#include "c89stringutils_safecrt.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996) /* disable secure warnings for fallbacks */
#endif

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
#define abort mock_abort
#define fopen mock_fopen
#define freopen mock_freopen
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
  int count = 0;
  const char *p = format;
  const char *b = buffer;
  char token[64];
  int consumed;
  void *ptr;

  while (*p && *b) {
    if (*p == ' ' || *p == '\t' || *p == '\n') {
      while (*b == ' ' || *b == '\t' || *b == '\n')
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
      p++;
      if (*p == '%') {
        if (*b != '%')
          return count;
        p++;
        b++;
        continue;
      }
      if (*p == '*') {
        /* Assignment suppressing */
        while (*p && strchr("cdieEfgGosuxXpn[", *p) == NULL)
          p++;
        if (*p)
          p++;
        len = (size_t)(p - start);
        if (len > 60)
          return -1;
        strncpy(token, start, len);
        token[len] = '\0';
        strcat(token, "%n");
        consumed = 0;
        if (sscanf(b, token, &consumed) < 0 || consumed == 0)
          return count;
        b += consumed;
        continue;
      }
      /* regular argument */
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
      len = (size_t)(p - start);
      if (len > 60)
        return -1;
      strncpy(token, start, len);
      token[len] = '\0';
      strcat(token, "%n");

      ptr = va_arg(args, void *);
      consumed = 0;
      if (sscanf(b, token, ptr, &consumed) <= 0 || consumed == 0)
        return count;
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
static int minimal_vfscanf(FILE *stream, const char *format, va_list args) {
  int count = 0;
  const char *p = format;
  char token[64];
  void *ptr;

  while (*p) {
    if (*p == ' ' || *p == '\t' || *p == '\n') {
      int c;
      while ((c = fgetc(stream)) != EOF &&
             (c == ' ' || c == '\t' || c == '\n')) {
        /* consume whitespace */
      }
      if (c != EOF)
        ungetc(c, stream);
      p++;
      continue;
    }
    if (*p != '%') {
      int c = fgetc(stream);
      if (c != *p) {
        if (c != EOF)
          ungetc(c, stream);
        return count;
      }
      p++;
      continue;
    }
    /* It's a %. Find the end of the format specifier. */
    {
      const char *start = p;
      size_t len;
      p++;
      if (*p == '%') {
        int c = fgetc(stream);
        if (c != '%') {
          if (c != EOF)
            ungetc(c, stream);
          return count;
        }
        p++;
        continue;
      }
      if (*p == '*') {
        /* Assignment suppressing */
        while (*p && strchr("cdieEfgGosuxXpn[", *p) == NULL)
          p++;
        if (*p)
          p++;
        len = (size_t)(p - start);
        if (len > 60)
          return -1;
        strncpy(token, start, len);
        token[len] = '\0';
        if (fscanf(stream, token) < 0)
          return count;
        continue;
      }
      /* regular argument */
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
      len = (size_t)(p - start);
      if (len > 60)
        return -1;
      strncpy(token, start, len);
      token[len] = '\0';

      ptr = va_arg(args, void *);
      if (fscanf(stream, token, ptr) <= 0)
        return count;
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
  (void)ptr;
  (void)error;
  if (msg) {
    fprintf(stderr, "Constraint violation: %s\n", msg);
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
  /* ignore */
}
#endif

/* Internal function to trigger constraint */
C89STRINGUTILS_EXPORT void
c89stringutils_invoke_constraint_handler_s(const char *msg, void *ptr,
                                           errno_t error) {
  if (current_handler) {
    current_handler(msg, ptr, error);
  }
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
    /* Workaround for MinGW bug where tmpfile_s returns 0 but leaves pFile NULL
     */
    if (rc == 0 && *pFile == NULL) {
      *pFile = tmpfile();
      if (*pFile == NULL)
        return errno ? errno : 5; /* EIO fallback */
    }
    return rc;
  }
#else
  *pFile = tmpfile();
  if (*pFile == NULL)
    return errno;
  return 0;
#endif
}

#if defined(C89STRINGUTILS_HAVE_WCHAR_H)
#if !defined(C89STRINGUTILS_HAVE_WCSCPY_S)
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
#endif

#if !defined(C89STRINGUTILS_HAVE_WCSNCPY_S)
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
      return 80; /* STRUNCATE / ERANGE */
    return 0;
  }
  for (i = 0; i < count && i < destsz; i++) {
    dest[i] = src[i];
    if (src[i] == L'\0')
      return 0;
  }
  if (i == destsz) {
    dest[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("wcsncpy_s: destsz is too small",
                                               NULL, ERANGE);
    return ERANGE;
  }
  dest[i] = L'\0';
  return 0;
}
#endif

#if !defined(C89STRINGUTILS_HAVE_VSWPRINTF_S)
C89STRINGUTILS_EXPORT int c89stringutils_vswprintf_s(wchar_t *buffer,
                                                     rsize_t sizeOfBuffer,
                                                     const wchar_t *format,
                                                     va_list argptr) {
  int ret;
  if (!buffer || sizeOfBuffer == 0 || !format) {
    c89stringutils_invoke_constraint_handler_s("vswprintf_s: invalid arguments",
                                               NULL, EINVAL);
    return -1;
  }
#if defined(C89STRINGUTILS_HAVE_VSWPRINTF)
  ret = vswprintf(buffer, sizeOfBuffer, format, argptr);
  if (ret < 0 || (size_t)ret >= sizeOfBuffer) {
    buffer[0] = L'\0';
    c89stringutils_invoke_constraint_handler_s("vswprintf_s: buffer too small",
                                               NULL, ERANGE);
    return -1;
  }
  return ret;
#else
  /* Fallback for missing vswprintf */
  c89stringutils_invoke_constraint_handler_s("vswprintf not supported natively",
                                             NULL, ENOTSUP);
  return -1;
#endif
}
#endif

#if !defined(C89STRINGUTILS_HAVE_SWPRINTF_S)
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
#endif
#endif /* C89STRINGUTILS_HAVE_WCHAR_H */
