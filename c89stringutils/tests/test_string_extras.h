#ifndef TEST_STRING_EXTRAS_H
#define TEST_STRING_EXTRAS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <greatest.h>

#include <c89stringutils_string_extras.h>
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
  ASSERT_STR_EQ(buffer, strnstr(buffer, target, strlen(buffer)));
  PASS();
}

TEST x_asprintf_should_succeed(void) {
  char *s = NULL;
  int rc = asprintf(&s, "foo%s", "bar");
  ASSERT_EQ(6, rc);
  ASSERT_STR_EQ("foobar", s);
  free(s);
  PASS();
}

TEST x_jasprintf_should_succeed(void) {
  char *s = NULL;
  jasprintf(&s, "foo%s", "bar");
  jasprintf(&s, "can%s", "haz");
  ASSERT_STR_EQ("foobarcanhaz", s);
  free(s);
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

TEST x_strcasestr_should_succeed(void) {
  const char *haystack = "The Quick Brown Fox";
  ASSERT_STR_EQ("Brown Fox", strcasestr(haystack, "bRoWn"));
  ASSERT_EQ((char *)NULL, strcasestr(haystack, "red"));
  PASS();
}

TEST x_strerrorlen_s_should_succeed(void) {
  ASSERT(strerrorlen_s(ENOMEM) > 0);
  ASSERT(strerrorlen_s(400) == 8); /* ESNULLP */
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
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(strnstr_suite) {
  RUN_TEST(x_strnstr_should_succeed);
  RUN_TEST(x_strnstr_should_fail);
  RUN_TEST(x_asprintf_should_succeed);
  RUN_TEST(x_jasprintf_should_succeed);
  RUN_TEST(x_strcasecmp_should_succeed);
  RUN_TEST(x_strncasecmp_should_succeed);
  RUN_TEST(x_strcasestr_should_succeed);
  RUN_TEST(x_strerrorlen_s_should_succeed);
  RUN_TEST(x_vasprintf_should_succeed);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !TEST_STRING_EXTRAS_H */