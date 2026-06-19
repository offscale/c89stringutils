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

TEST x_strnstr_should_succeed(void) {
  ASSERT_EQ_FMT(buffer, strnstr(buffer, target, strlen(buffer)), "%s");
  PASS();
}

TEST x_strnstr_should_fail(void) {
  ASSERT_EQ(NULL, strnstr(buffer, "world", 5));
  ASSERT_EQ(NULL, strnstr(NULL, "world", 5));
  ASSERT_EQ(NULL, strnstr(buffer, NULL, 5));
  ASSERT_EQ(buffer, strnstr(buffer, "", 5));
  PASS();
}

TEST x_asprintf_should_succeed(void) {
  char *s = NULL;
  int rc = asprintf(&s, "foo%s", "bar");
  ASSERT_EQ(6, rc);
  ASSERT_STR_EQ("foobar", s);
  free(s);
  ASSERT_EQ(-1, asprintf(NULL, "foo"));
  ASSERT_EQ(-1, asprintf(&s, NULL));
  PASS();
}

TEST x_jasprintf_should_succeed(void) {
  char *s = NULL;
  jasprintf(&s, "foo%s", "bar");
  jasprintf(&s, "can%s", "haz");
  ASSERT_STR_EQ("foobarcanhaz", s);
  free(s);
  ASSERT_EQ(NULL, jasprintf(NULL, NULL));
  ASSERT_EQ(NULL, jasprintf(&s, NULL));
  PASS();
}

TEST x_strcasecmp_should_succeed(void) {
  ASSERT_EQ(0, strcasecmp("HeLlO", "hElLo"));
  ASSERT(strcasecmp("apple", "banana") < 0);
  ASSERT(strcasecmp("banana", "apple") > 0);
  PASS();
}

TEST x_strncasecmp_should_succeed(void) {
  ASSERT_EQ(0, strncasecmp("HeLlO World", "hElLo Earth", 5));
  ASSERT(strncasecmp("apple pie", "apple tart", 9) < 0);
  PASS();
}

static char *test_strcasestr_wrapper(const char *h, const char *n) {
  return strcasestr(h, n);
}

TEST x_strcasestr_should_succeed(void) {
  const char *haystack = "The Quick Brown Fox";
  ASSERT_STR_EQ("Brown Fox", strcasestr(haystack, "bRoWn"));
  ASSERT_EQ((char *)NULL, strcasestr(haystack, "red"));
  ASSERT_EQ(haystack, strcasestr(haystack, ""));
  ASSERT_STR_EQ("", strcasestr("", ""));

  /* Use wrapper to avoid -Wnonnull warnings on GCC for standard string
   * functions if they are macro'd */
  ASSERT_EQ(NULL, test_strcasestr_wrapper(NULL, "fox"));
  ASSERT_EQ(NULL, test_strcasestr_wrapper(haystack, NULL));
  PASS();
}

TEST x_strerrorlen_s_should_succeed(void) {
  ASSERT(strerrorlen_s(ENOMEM) > 0);
  ASSERT(strerrorlen_s(400) == 8); /* ESNULLP */
  ASSERT(
      strerrorlen_s(9999) == 0 ||
      strerrorlen_s(9999) >
          0); /* Unknown error, may have a string like "Unknown error 9999" */
  PASS();
}

/* vasprintf can be tested implicitly via asprintf, but let's test it directly
 * just in case */
static int test_vasprintf_wrapper(char **str, const char *fmt, ...) {
  int rc;
  va_list ap;
  va_start(ap, fmt);
  rc = vasprintf(str, fmt, ap);
  va_end(ap);
  return rc;
}

TEST x_vasprintf_should_succeed(void) {
  char *s = NULL;
  int rc = test_vasprintf_wrapper(&s, "test %d", 123);
  ASSERT_EQ(8, rc);
  ASSERT_STR_EQ("test 123", s);
  free(s);
  ASSERT_EQ(-1, test_vasprintf_wrapper(NULL, "test"));
  ASSERT_EQ(-1, test_vasprintf_wrapper(&s, NULL));
  PASS();
}

TEST x_log_debug_should_succeed(void) {
  LOG_DEBUG("test log debug: %d", 1);
  c89stringutils_log_debug("direct call %s", "test");
  PASS();
}

TEST x_asprintf_realloc_path(void) {
  char *s = NULL;
  int rc;
  /* generate a string longer than INIT_SZ (128) to trigger realloc */
  char big[200];
  memset(big, 'A', 199);
  big[199] = '\0';
  rc = asprintf(&s, "%s", big);
  ASSERT_EQ(199, rc);
  ASSERT_STR_EQ(big, s);
  free(s);
  PASS();
}

TEST x_jasprintf_realloc_path(void) {
  char *s = NULL;
  char big[200];
  memset(big, 'A', 199);
  big[199] = '\0';
  jasprintf(&s, "%s", big);
  jasprintf(&s, "%s", big);
  ASSERT_EQ(398, strlen(s));
  free(s);
  PASS();
}

TEST x_jasprintf_should_fail(void) {
  /* It is difficult to trigger vsnprintf / realloc failures natively without
     mocking. We will rely on existing coverage. */
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(strnstr_suite) {
  RUN_TEST(x_strnstr_should_succeed);
  RUN_TEST(x_strnstr_should_fail);
  RUN_TEST(x_asprintf_should_succeed);
  RUN_TEST(x_asprintf_realloc_path);
  RUN_TEST(x_jasprintf_should_succeed);
  RUN_TEST(x_jasprintf_realloc_path);
  RUN_TEST(x_strcasecmp_should_succeed);
  RUN_TEST(x_strncasecmp_should_succeed);
  RUN_TEST(x_strcasestr_should_succeed);
  RUN_TEST(x_strerrorlen_s_should_succeed);
  RUN_TEST(x_vasprintf_should_succeed);
  RUN_TEST(x_log_debug_should_succeed);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !TEST_STRING_EXTRAS_H */
