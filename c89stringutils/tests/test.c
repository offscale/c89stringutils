/**
 * @file test.c
 * @brief Main entry point for greatest tests.
 */
/* clang-format off */
#if defined(_MSC_VER)
#pragma warning(disable: 4127) /* conditional expression is constant */
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <greatest.h>
#include <c89stringutils_log.h>
#include <c89stringutils_safecrt.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "test_string_extras.h"
#include "test_safecrt.h"
/* clang-format on */

/** @brief Mock state for malloc failure */
int g_mock_malloc_fail = 0;
/** @brief Mock state for realloc failure */
int g_mock_realloc_fail = 0;
/** @brief Mock state for vsnprintf failure */
int g_mock_vsnprintf_fail = 0;
/** @brief Mock state for vsnprintf return */
int g_mock_vsnprintf_ret = 0;
/** @brief Mock state for strerror returning null */
int g_mock_strerror_null = 0;
/** @brief Mock state for log_debug failure */
int g_mock_log_debug_fail = 0;

/**
 * @brief Mock for malloc
 * @param size the size
 * @return the allocated memory
 */
void *mock_malloc(size_t size) {
  if (g_mock_malloc_fail)
    return NULL;
  return malloc(size);
}

/**
 * @brief Mock for realloc
 * @param ptr the pointer
 * @param size the size
 * @return the reallocated memory
 */
void *mock_realloc(void *ptr, size_t size) {
  if (g_mock_realloc_fail)
    return NULL;
  return realloc(ptr, size);
}

/** @brief Mock state for vsnprintf call count trigger */
int g_mock_vsnprintf_fail_call = 0;
/** @brief Mock state for vsnprintf call count */
int g_mock_vsnprintf_call_count = 0;
/** @brief Mock state for vsnprintf second return */
int g_mock_vsnprintf_ret2 = 0; /* for second call */
/** @brief Mock state for vsnprintf failing on large sizes */
int g_mock_vsnprintf_fail_on_large_size = 0;

/**
 * @brief Mock vsnprintf core logic
 * @param size the size
 * @param ret the return value
 * @return the processed return value
 */
static int do_mock_vsnprintf(size_t size, int ret) {
  g_mock_vsnprintf_call_count++;
  if (g_mock_vsnprintf_fail_on_large_size == 1 &&
      (size > 150 || (size == 0 && g_mock_vsnprintf_call_count >= 3))) {
    return -1;
  }
  if (g_mock_vsnprintf_fail_on_large_size == 2 &&
      (size > 150 || (size == 0 && g_mock_vsnprintf_call_count >= 3))) {
    return 9999; /* Return something >= size */
  }
  if (g_mock_vsnprintf_fail_call == g_mock_vsnprintf_call_count)
    return g_mock_vsnprintf_ret2;
  if (g_mock_vsnprintf_fail)
    return g_mock_vsnprintf_ret < 0 ? -1 : g_mock_vsnprintf_ret;
  return ret;
}

/**
 * @brief Mock vsnprintf
 * @param str the string
 * @param size the size
 * @param format the format string
 * @param ap the va_list
 * @return the written length
 */
int mock_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
  int ret = -1;
  if (str == NULL) {
    /* Need a valid buffer for vsprintf if vsnprintf is missing, but cygwin has
     * vsnprintf */
#if defined(__CYGWIN__) || defined(__linux__) || defined(__APPLE__) ||         \
    defined(_WIN32)
    ret = vsnprintf(NULL, 0, format, ap);
#else
    char dummy[1024];
    ret = vsprintf(dummy, format, ap);
#endif
  } else {
#if defined(__CYGWIN__) || defined(__linux__) || defined(__APPLE__) ||         \
    defined(_WIN32)
    ret = vsnprintf(str, size, format, ap);
#else
    ret = vsprintf(str, format, ap);
#endif
  }
  return do_mock_vsnprintf(size, ret);
}

/**
 * @brief Mock vscprintf
 * @param format the format string
 * @param ap the va_list
 * @return the written length
 */
int mock_vscprintf(const char *format, va_list ap) {
  int ret = -1;
#ifdef _MSC_VER
  ret = _vscprintf(format, ap);
#else
  ret = vsnprintf(NULL, 0, format, ap);
#endif
  return do_mock_vsnprintf(0, ret);
}

/**
 * @brief Mock vsnprintf_s
 * @param buffer the buffer
 * @param sizeOfBuffer the size of the buffer
 * @param count the count
 * @param format the format string
 * @param ap the va_list
 * @return the written length
 */
int mock_vsnprintf_s(char *buffer, size_t sizeOfBuffer, size_t count,
                     const char *format, va_list ap) {
  int ret = -1;
#ifdef _MSC_VER
  ret = _vsnprintf_s(buffer, sizeOfBuffer, count, format, ap);
#else
  ret = vsnprintf(buffer, count, format, ap);
#endif
  return do_mock_vsnprintf(sizeOfBuffer, ret);
}

/**
 * @brief Mock vsprintf
 * @param buffer the buffer
 * @param format the format string
 * @param ap the va_list
 * @return the written length
 */
int mock_vsprintf(char *buffer, const char *format, va_list ap) {
  int ret = vsprintf(buffer, format, ap);
  return do_mock_vsnprintf(9999, ret); /* size unknown, pass large */
}

/**
 * @brief Mock vfprintf
 * @param stream the stream
 * @param format the format string
 * @param ap the va_list
 * @return the written length
 */
int mock_vfprintf(FILE *stream, const char *format, va_list ap) {
  if (g_mock_log_debug_fail)
    return -1;
  return vfprintf(stream, format, ap);
}

/**
 * @brief Mock fprintf
 * @param stream the stream
 * @param format the format string
 * @param ... the va_list
 * @return the written length
 */
int mock_fprintf(FILE *stream, const char *format, ...) {
  int ret;
  va_list ap;
  if (g_mock_log_debug_fail)
    return -1;
  va_start(ap, format);
  ret = vfprintf(stream, format, ap);
  va_end(ap);
  return ret;
}

#ifdef _WIN32
/**
 * @brief Mock vfprintf_s
 * @param stream the stream
 * @param format the format string
 * @param ap the va_list
 * @return the written length
 */
int mock_vfprintf_s(FILE *stream, const char *format, va_list ap) {
  if (g_mock_log_debug_fail)
    return -1;
  return vfprintf_s(stream, format, ap);
}

/**
 * @brief Mock fprintf_s
 * @param stream the stream
 * @param format the format string
 * @param ... the va_list
 * @return the written length
 */
int mock_fprintf_s(FILE *stream, const char *format, ...) {
  int ret;
  va_list ap;
  if (g_mock_log_debug_fail)
    return -1;
  va_start(ap, format);
  ret = vfprintf_s(stream, format, ap);
  va_end(ap);
  return ret;
}

/**
 * @brief Mock strerror_s
 * @param buffer the buffer
 * @param sizeInBytes the size
 * @param errnum the error number
 * @return the error code
 */
int mock_strerror_s(char *buffer, size_t sizeInBytes, int errnum) {
  if (g_mock_strerror_null)
    return 22; /* EINVAL */
  return strerror_s(buffer, sizeInBytes, errnum);
}
#endif

/**
 * @brief Mock strerror
 * @param errnum the error number
 * @return the error string
 */
char *mock_strerror(int errnum) {
  if (g_mock_strerror_null)
    return NULL;
  return strerror(errnum);
}

int g_mock_fopen_fail = 0;
FILE *mock_fopen(const char *filename, const char *mode) {
  if (g_mock_fopen_fail) {
    errno = 12;
    return NULL;
  }
  return fopen(filename, mode);
}

FILE *mock_freopen(const char *filename, const char *mode, FILE *stream) {
  if (g_mock_fopen_fail) {
    errno = 12;
    return NULL;
  }
  return freopen(filename, mode, stream);
}

FILE *mock_tmpfile(void) {
  if (g_mock_fopen_fail) {
    errno = 12;
    return NULL;
  }
  return tmpfile();
}

#if defined(_MSC_VER)
#include <crtdbg.h>
#if defined(C89STRINGUTILS_HAVE_WCHAR_H) || defined(_MSC_VER)
/**
 * @brief Invalid parameter handler that does nothing.
 * @param expression the expression
 * @param function the function
 * @param file the file
 * @param line the line
 * @param pReserved the reserved pointer
 */
void myInvalidParameterHandler(const wchar_t *expression,
                               const wchar_t *function, const wchar_t *file,
                               unsigned int line, uintptr_t pReserved) {
  /* Do nothing, let the safe CRT function return the error code */
  (void)expression;
  (void)function;
  (void)file;
  (void)line;
  (void)pReserved;
}
#endif
#endif

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

/**
 * @brief Main function for tests.
 * @param argc Number of arguments.
 * @param argv Argument values.
 * @return Exit status.
 */
int main(int argc, char **argv) {
#if defined(_MSC_VER)
  _set_invalid_parameter_handler(myInvalidParameterHandler);
  _CrtSetReportMode(_CRT_ASSERT, 0);
#endif
  c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(strnstr_suite);
  RUN_SUITE(safecrt_suite);
  GREATEST_MAIN_END();
}
