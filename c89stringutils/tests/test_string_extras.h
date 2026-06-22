/**
 * @file test_string_extras.h
 * @brief Tests for string extra functions.
 */
#ifndef TEST_STRING_EXTRAS_H
#define TEST_STRING_EXTRAS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <greatest.h>

#include <c89stringutils_string_extras.h>
#include <c89stringutils_log.h>
#include <stdlib.h>
#include <errno.h>
/* clang-format on */

static const char *buffer = "hello world";
static const char *target = "hello\0\0\0";

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strnstr_should_succeed(void) {
  char *out;
  out = c89stringutils_strnstr(buffer, target, strlen(buffer));
  ASSERT_EQ_FMT(buffer, out, "%s");
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strnstr_should_fail(void) {
  char *out;
  out = c89stringutils_strnstr(buffer, "world", 5);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr(buffer, "", 5);
  ASSERT_EQ(buffer, out);
  out = c89stringutils_strnstr(NULL, buffer, 5);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr(buffer, NULL, 5);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr("abcde", "abX", 5);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr("abc", "abcd", 4);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr("a", "abc", 10);
  ASSERT_EQ(NULL, out);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_asprintf_should_succeed(void) {
  char *s = NULL;
  int rc;
  rc = c89stringutils_asprintf(&s, "foo%s", "bar");
  ASSERT_EQ(6, rc);
  ASSERT_STR_EQ("foobar", s);
  free(s);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_asprintf_should_fail(void) {
  char *s = NULL;
  int rc;
  rc = c89stringutils_asprintf(NULL, "foo");
  ASSERT_EQ(-1, rc);
  rc = c89stringutils_asprintf(&s, NULL);
  ASSERT_EQ(-1, rc);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_jasprintf_should_succeed(void) {
  char *s = NULL;
  int rc1;
  int rc2;
  rc1 = c89stringutils_jasprintf(&s, "foo%s", "bar");
  rc2 = c89stringutils_jasprintf(&s, "can%s", "haz");
  ASSERT_EQ(0, rc1);
  ASSERT_EQ(0, rc2);
  ASSERT_STR_EQ("foobarcanhaz", s);
  free(s);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strcasecmp_should_succeed(void) {
  int out;
  out = c89stringutils_strcasecmp("HeLlO", "hElLo");
  ASSERT_EQ(0, out);
  out = c89stringutils_strcasecmp("apple", "banana");
  ASSERT(out < 0);
  out = c89stringutils_strcasecmp("banana", "apple");
  ASSERT(out > 0);
  out = c89stringutils_strcasecmp(NULL, "apple");
  ASSERT_EQ(-1, out);
  out = c89stringutils_strcasecmp("apple", NULL);
  ASSERT_EQ(1, out);
  out = c89stringutils_strcasecmp(NULL, NULL);
  ASSERT_EQ(0, out);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strncasecmp_should_succeed(void) {
  int out;
  out = c89stringutils_strncasecmp("HeLlO World", "hElLo Earth", 5);
  ASSERT_EQ(0, out);
  out = c89stringutils_strncasecmp("apple pie", "apple tart", 9);
  ASSERT(out < 0);
  out = c89stringutils_strncasecmp(NULL, "apple", 5);
  ASSERT_EQ(-1, out);
  out = c89stringutils_strncasecmp("apple", NULL, 5);
  ASSERT_EQ(1, out);
  out = c89stringutils_strncasecmp(NULL, NULL, 5);
  ASSERT_EQ(0, out);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strcasestr_should_succeed(void) {
  const char *haystack = "The Quick Brown Fox";
  char *out;
  out = c89stringutils_strcasestr(haystack, "bRoWn");
  ASSERT_STR_EQ("Brown Fox", out);
  out = c89stringutils_strcasestr(haystack, "red");
  ASSERT_EQ((char *)NULL, out);
  out = c89stringutils_strcasestr(haystack, "");
  ASSERT_EQ(haystack, out);
  out = c89stringutils_strcasestr("", "");
  ASSERT_STR_EQ("", out);
  out = c89stringutils_strcasestr(NULL, haystack);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strcasestr(haystack, NULL);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strcasestr("abcde", "abX");
  ASSERT_EQ(NULL, out);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strerrorlen_s_should_succeed(void) {
  size_t out;
  out = c89stringutils_strerrorlen_s(ENOMEM);
  ASSERT(out > 0);
  out = c89stringutils_strerrorlen_s(400);
  ASSERT(out == 8); /* ESNULLP */
  out = c89stringutils_strerrorlen_s(99999);
  ASSERT(out > 0 || out == 0); /* Unknown error string */
  PASS();
}

/* vasprintf can be tested implicitly via asprintf, but let's test it directly
 * just in case */
/**
 * @brief Wrapper for vasprintf for testing.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return exit code
 */
static int test_vasprintf_wrapper(char **str, const char *fmt, ...) {
  int rc;
  va_list ap;
  va_start(ap, fmt);
  rc = c89stringutils_vasprintf(str, fmt, ap);
  va_end(ap);
  return rc;
}

#if !defined(HAVE_ASPRINTF) || defined(C89STRINGUTILS_TEST_MOCKS)
static int test_alias_vasprintf_wrapper(char **str, const char *fmt, ...) {
  int rc;
  va_list ap;
  va_start(ap, fmt);
  rc = vasprintf(str, fmt, ap);
  va_end(ap);
  return rc;
}
#endif

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_vasprintf_should_succeed(void) {
  char *s = NULL;
  int rc;
  rc = test_vasprintf_wrapper(&s, "test %d", 123);
  ASSERT_EQ(8, rc);
  ASSERT_STR_EQ("test 123", s);
  free(s);
#if !defined(HAVE_ASPRINTF) || defined(C89STRINGUTILS_TEST_MOCKS)
  /* Test aliases if they are compiled in this environment */
  rc = asprintf(&s, "alias %d", 456);
  ASSERT_EQ(9, rc);
  ASSERT_STR_EQ("alias 456", s);
  free(s);

  rc = test_alias_vasprintf_wrapper(&s, "valias %d", 789);
  ASSERT_EQ(10, rc);
  ASSERT_STR_EQ("valias 789", s);
  free(s);
#endif
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_vasprintf_should_fail(void) {
  char *s = NULL;
  int rc;
  rc = test_vasprintf_wrapper(NULL, "test %d", 123);
  ASSERT_EQ(-1, rc);
  rc = test_vasprintf_wrapper(&s, NULL, 123);
  ASSERT_EQ(-1, rc);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_log_debug_should_succeed(void) {
  LOG_DEBUG("test log debug: %d", 1);
  c89stringutils_log_debug("direct call %s", "test");
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_asprintf_realloc_path(void) {
  char *s = NULL;
  int rc;
  /* generate a string longer than INIT_SZ (128) to trigger realloc */
  char big[200];
  memset(big, 'A', 199);
  big[199] = '\0';
  rc = c89stringutils_asprintf(&s, "%s", big);
  ASSERT_EQ(199, rc);
  ASSERT_STR_EQ(big, s);
  free(s);
  PASS();
}

extern int g_mock_malloc_fail;
extern int g_mock_realloc_fail;
extern int g_mock_vsnprintf_fail;
extern int g_mock_vsnprintf_ret;
extern int g_mock_strerror_null;

extern int g_mock_vsnprintf_fail_call;
extern int g_mock_vsnprintf_call_count;

TEST x_mock_failures(void) {
  char *s = NULL;
  size_t len;
  int rc;

  g_mock_malloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  g_mock_malloc_fail = 0;

  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = 200; /* force realloc path */
  g_mock_realloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  g_mock_vsnprintf_fail = 0;
  g_mock_realloc_fail = 0;

  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail_call = 0;
  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = INT_MAX;
  rc = c89stringutils_asprintf(&s, "test");
  ASSERT_EQ(-1, rc);
  g_mock_vsnprintf_fail = 0;

  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail_call = 2; /* Fail on the second call (after realloc) */
  g_mock_vsnprintf_fail = 0;
  /* To trigger realloc, first call must return > 128. I need to format a long
   * string */
  {
    char big[200];
    memset(big, 'A', 199);
    big[199] = '\0';
    g_mock_vsnprintf_ret2 = 205; /* return rc >= len */
    ASSERT_EQ(-1, c89stringutils_asprintf(&s, "%s", big));

    g_mock_vsnprintf_call_count = 0;
    g_mock_vsnprintf_fail_call = 2;
    g_mock_vsnprintf_ret2 = -1; /* return rc < 0 */
    ASSERT_EQ(-1, c89stringutils_asprintf(&s, "%s", big));
  }
  g_mock_vsnprintf_fail_call = 0;

  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = -1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  ASSERT_EQ(-1, c89stringutils_jasprintf(&s, "test"));
  ASSERT_EQ(-1, jasprintf(&s, "test"));
  g_mock_vsnprintf_fail = 0;

  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail_call = 2;
  g_mock_vsnprintf_ret2 = -1;
  ASSERT_EQ(-1, c89stringutils_jasprintf(&s, "test"));
  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail_call = 2;
  g_mock_vsnprintf_ret2 = -1;
  ASSERT_EQ(-1, jasprintf(&s, "test"));
  g_mock_vsnprintf_fail_call = 0;

  s = malloc(10);
  if (s)
    strcpy(s, "123");
  g_mock_realloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_jasprintf(&s, "test"));
  ASSERT_EQ(-1, jasprintf(&s, "test"));
  g_mock_realloc_fail = 0;
  if (s) {
    free(s);
    s = NULL;
  }

  g_mock_strerror_null = 1;
  len = c89stringutils_strerrorlen_s(99999);
  ASSERT_EQ(0, len);
  g_mock_strerror_null = 0;

  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_jasprintf_realloc_path(void) {
  char *s = NULL;
  char big[200];
  memset(big, 'A', 199);
  big[199] = '\0';
  c89stringutils_jasprintf(&s, "%s", big);
  c89stringutils_jasprintf(&s, "%s", big);
  ASSERT_EQ(398, strlen(s));
  free(s);
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_jasprintf_alias_should_succeed(void) {
  char *s = NULL;
  int rc1;
  int rc2;
  rc1 = jasprintf(&s, "foo%s", "bar");
  rc2 = jasprintf(&s, "can%s", "haz");
  ASSERT_EQ(0, rc1);
  ASSERT_EQ(0, rc2);
  ASSERT_STR_EQ("foobarcanhaz", s);
  free(s);
  rc1 = jasprintf(NULL, "foo");
  ASSERT_EQ(-1, rc1);
  rc1 = jasprintf(&s, NULL);
  ASSERT_EQ(-1, rc1);

  rc1 = c89stringutils_jasprintf(NULL, "foo");
  ASSERT_EQ(-1, rc1);
  rc1 = c89stringutils_jasprintf(&s, NULL);
  ASSERT_EQ(-1, rc1);
  PASS();
}

/* Suites can group multiple tests with common setup. */
/**
 * @brief Test suite
 */
SUITE(strnstr_suite) {
  RUN_TEST(x_strnstr_should_succeed);
  RUN_TEST(x_strnstr_should_fail);
  RUN_TEST(x_asprintf_should_succeed);
  RUN_TEST(x_asprintf_should_fail);
  RUN_TEST(x_asprintf_realloc_path);
  RUN_TEST(x_jasprintf_should_succeed);
  RUN_TEST(x_jasprintf_realloc_path);
  RUN_TEST(x_mock_failures);
  RUN_TEST(x_jasprintf_alias_should_succeed);
  RUN_TEST(x_strcasecmp_should_succeed);
  RUN_TEST(x_strncasecmp_should_succeed);
  RUN_TEST(x_strcasestr_should_succeed);
  RUN_TEST(x_strerrorlen_s_should_succeed);
  RUN_TEST(x_vasprintf_should_succeed);
  RUN_TEST(x_vasprintf_should_fail);
  RUN_TEST(x_log_debug_should_succeed);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !TEST_STRING_EXTRAS_H */
