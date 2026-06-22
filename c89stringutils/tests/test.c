/**
 * @file test.c
 * @brief Main entry point for greatest tests.
 */
/* clang-format off */
#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
#include <greatest.h>
#include <c89stringutils_log.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "test_string_extras.h"
/* clang-format on */

int g_mock_malloc_fail = 0;
int g_mock_realloc_fail = 0;
int g_mock_vsnprintf_fail = 0;
int g_mock_vsnprintf_ret = 0;
int g_mock_strerror_null = 0;

void *mock_malloc(size_t size) {
  if (g_mock_malloc_fail)
    return NULL;
  return malloc(size);
}

void *mock_realloc(void *ptr, size_t size) {
  if (g_mock_realloc_fail)
    return NULL;
  return realloc(ptr, size);
}

int g_mock_vsnprintf_fail_call = 0;
int g_mock_vsnprintf_call_count = 0;
int g_mock_vsnprintf_ret2 = 0; /* for second call */

int mock_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
  g_mock_vsnprintf_call_count++;
  if (g_mock_vsnprintf_fail_call == g_mock_vsnprintf_call_count)
    return g_mock_vsnprintf_ret2;
  if (g_mock_vsnprintf_fail)
    return g_mock_vsnprintf_ret < 0 ? -1 : g_mock_vsnprintf_ret;
  return vsnprintf(str, size, format, ap);
}

char *mock_strerror(int errnum) {
  if (g_mock_strerror_null)
    return NULL;
  return strerror(errnum);
}

#ifdef _MSC_VER
int mock_strerror_s(char *buffer, size_t sizeInBytes, int errnum) {
  if (g_mock_strerror_null)
    return 22; /* EINVAL */
  return strerror_s(buffer, sizeInBytes, errnum);
}
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
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(strnstr_suite);
  GREATEST_MAIN_END();
}
