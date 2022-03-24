#include <greatest.h>

#define C89STRINGUTILS_IMPLEMENTATION
#include <c89stringutils_string_extras.h>

static const char *buffer = "hello world";
static const char *target = "hello\0\0\0";

TEST x_strnstr_should_succeed(void) {
  ASSERT_EQ_FMT(buffer, strnstr(buffer, target, strlen(buffer)), "%s");
  PASS();
}

TEST x_strnstr_should_fail(void) {
  ASSERT_EQ(strcmp(buffer, strnstr(buffer, target, strlen(buffer))), 0);
  PASS();
}

TEST x_jasprintf_should_succeed(void) {
  char *s = NULL;
  jasprintf(&s, "foo%s", "bar");
  jasprintf(&s, "can%s", "haz");
  ASSERT_EQ(strcmp(s, "foobarcanhaz"), 0);
  free(s);
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(strnstr_suite) {
  RUN_TEST(x_strnstr_should_succeed);
  RUN_TEST(x_strnstr_should_fail);
  RUN_TEST(x_jasprintf_should_succeed);
}
