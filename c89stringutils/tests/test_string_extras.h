#include <greatest.h>

#include <c89stringutils_string_extras.h>

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
  asprintf(&s, "foo%s", "bar");
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

/* Suites can group multiple tests with common setup. */
SUITE(strnstr_suite) {
  RUN_TEST(x_strnstr_should_succeed);
  RUN_TEST(x_strnstr_should_fail);
  RUN_TEST(x_asprintf_should_succeed);
  RUN_TEST(x_jasprintf_should_succeed);
}
