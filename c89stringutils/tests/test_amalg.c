/**
 * @file test_amalg.c
 * @brief Standalone test for the STB-style single-header amalgamation.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define C89STRINGUTILS_IMPLEMENTATION
/* clang-format off */
#include "c89stringutils_amalgamation.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
/* clang-format on */

static int g_mock_printf_fail = 0;
static int mock_printf(const char *format, ...)
    C89STRINGUTILS_FORMAT_PRINTF(1, 2);
static int mock_printf(const char *format, ...) {
  if (g_mock_printf_fail)
    return -1;
  {
    int rc;
    va_list ap;
    va_start(ap, format);
    rc = vprintf(format, ap);
    va_end(ap);
    return rc;
  }
}
#define printf mock_printf

static int g_mock_fprintf_fail = 0;
static int mock_fprintf(FILE *stream, const char *format, ...)
    C89STRINGUTILS_FORMAT_PRINTF(2, 3);
static int mock_fprintf(FILE *stream, const char *format, ...) {
  if (g_mock_fprintf_fail)
    return -1;
  {
    int rc;
    va_list ap;
    va_start(ap, format);
    rc = vfprintf(stream, format, ap);
    va_end(ap);
    return rc;
  }
}
#define fprintf mock_fprintf

static int test_amalg(int force_asprintf_fail) {
  char *x = NULL;
  int rc;
  if (force_asprintf_fail) {
    rc = -1;
  } else {
    rc = c89stringutils_asprintf(&x, "hello %s", "world");
  }

  if (rc == 0 && x != NULL) {
    {
      int print_rc = printf("%s\n", x);
      if (print_rc < 0) {
        free(x);
        return 1;
      }
    }
    free(x);
    return 0;
  }
  {
    int print_rc = fprintf(stderr, "Failed: rc = %d, x = %p\n", rc, (void *)x);
    if (print_rc < 0) {
      return 1;
    }
  }
  return 1;
}

#undef printf
#undef fprintf
int main(void) {
  int rc;

  /* Success path */
  g_mock_printf_fail = 0;
  g_mock_fprintf_fail = 0;
  rc = test_amalg(0);
  if (rc != 0)
    return 1;

  /* printf failure path */
  g_mock_printf_fail = 1;
  rc = test_amalg(0);
  if (rc != 1)
    return 1;

  /* asprintf failure path */
  g_mock_printf_fail = 0;
  rc = test_amalg(1);
  if (rc != 1)
    return 1;

  /* fprintf failure path */
  g_mock_fprintf_fail = 1;
  rc = test_amalg(1);
  if (rc != 1)
    return 1;

  return 0;
}
