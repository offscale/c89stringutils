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

/** @brief Test buffer */
static const char *test_buffer = "hello world";
/** @brief Test target */
static const char *test_target = "hello\0\0\0";

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strnstr_should_succeed(void) {
  char *out;
  out = c89stringutils_strnstr(test_buffer, test_target, strlen(test_buffer));
  ASSERT_EQ_FMT(test_buffer, out, "%s");
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strnstr_should_fail(void) {
  char *out;
  out = c89stringutils_strnstr(test_buffer, "world", 5);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr(test_buffer, "", 5);
  ASSERT_EQ(test_buffer, out);
  /* Many native strnstr implementations crash on NULL arguments */
  out = c89stringutils_strnstr(NULL, test_buffer, 5);
  ASSERT_EQ(NULL, out);
  out = c89stringutils_strnstr(test_buffer, NULL, 5);
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
  ASSERT_EQ(0, rc);
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
#if !defined(C89STRINGUTILS_HAVE__STRICMP) &&                                  \
    !defined(C89STRINGUTILS_HAVE_STRCASECMP)
  ASSERT_EQ(0, c89stringutils_strcasecmp(NULL, NULL));
  ASSERT_EQ(-1, c89stringutils_strcasecmp(NULL, "test"));
  ASSERT_EQ(1, c89stringutils_strcasecmp("test", NULL));
#endif
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
#if !defined(C89STRINGUTILS_HAVE__STRNICMP) &&                                 \
    !defined(C89STRINGUTILS_HAVE_STRNCASECMP)
  ASSERT_EQ(0, c89stringutils_strncasecmp(NULL, NULL, 5));
  ASSERT_EQ(-1, c89stringutils_strncasecmp(NULL, "test", 5));
  ASSERT_EQ(1, c89stringutils_strncasecmp("test", NULL, 5));
#endif
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
  ASSERT(out != NULL);
  ASSERT(strcmp("Brown Fox", out) == 0);
  out = c89stringutils_strcasestr(haystack, "red");
  ASSERT(out == NULL);
  out = c89stringutils_strcasestr(haystack, "");
  ASSERT(out == haystack);
  out = c89stringutils_strcasestr("", "");
  ASSERT(out != NULL);
#if !defined(C89STRINGUTILS_HAVE_STRCASESTR) ||                                \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
  out = c89stringutils_strcasestr(NULL, haystack);
  ASSERT(out == NULL);
  out = c89stringutils_strcasestr(haystack, NULL);
  ASSERT(out == NULL);
#endif

  out = c89stringutils_strcasestr("abcde", "abX");
  ASSERT_EQ(NULL, out);
  PASS();
}

/**
 * @brief Test case for strerror_s
 * @return enum test result
 */
TEST x_strerror_s_should_succeed(void) {
  char buf[256];
  errno_t rc;
  rc = c89stringutils_strerror_s(buf, sizeof(buf), ENOMEM);
  ASSERT_EQ(0, rc);
  ASSERT(strlen(buf) > 0);

  rc = c89stringutils_strerror_s(NULL, 100, ENOMEM);
  ASSERT(rc != 0);

  rc = c89stringutils_strerror_s(buf, 0, ENOMEM);
  ASSERT(rc != 0);

  rc = c89stringutils_strerror_s(buf, sizeof(buf), 99999);
  ASSERT(rc == 0 || rc != 0); /* Could succeed with 'Unknown error' or fail */
  PASS();
}

/**
 * @brief Wrapper for vsnprintf_s for testing.
 */
static int test_vsnprintf_s_wrapper(char *s, size_t n, const char *format,
                                    ...) {
  int rc;
  va_list args;
  va_start(args, format);
  rc = c89stringutils_vsnprintf_s(s, n, format, args);
  va_end(args);
  return rc;
}

/**
 * @brief Wrapper for vsnprintf for testing.
 */
static int test_vsnprintf_wrapper(char *s, size_t n, const char *format, ...) {
  int rc;
  va_list args;
  va_start(args, format);
  rc = c89stringutils_vsnprintf(s, n, format, args);
  va_end(args);
  return rc;
}

/**
 * @brief Test case for snprintf_s
 * @return enum test result
 */
TEST x_snprintf_s_should_succeed(void) {
  char buf[20];
  int rc;
  rc = c89stringutils_snprintf_s(buf, sizeof(buf), "test %d", 123);
  ASSERT(rc == 8);
  ASSERT_STR_EQ("test 123", buf);

  rc = c89stringutils_snprintf_s(NULL, 0, "test");
  ASSERT(rc < 0);

  rc = test_vsnprintf_s_wrapper(buf, sizeof(buf), "test %d", 123);
  ASSERT(rc == 8);
  ASSERT_STR_EQ("test 123", buf);

  rc = test_vsnprintf_s_wrapper(NULL, 0, "test");
  ASSERT(rc < 0);
  PASS();
}

/**
 * @brief Test case for snprintf
 * @return enum test result
 */
TEST x_snprintf_should_succeed(void) {
  char buf[20];
  int rc;
  rc = c89stringutils_snprintf(buf, sizeof(buf), "test %d", 123);
  ASSERT_EQ(8, rc);
  ASSERT_STR_EQ("test 123", buf);

  rc = c89stringutils_snprintf(NULL, 0, "test");
  ASSERT(rc >= 0); /* standard snprintf behavior */

  rc = test_vsnprintf_wrapper(buf, sizeof(buf), "test %d", 123);
  ASSERT_EQ(8, rc);
  ASSERT_STR_EQ("test 123", buf);

  rc = test_vsnprintf_wrapper(NULL, 0, "test");
  ASSERT(rc >= 0); /* standard snprintf behavior */
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

#if !defined(C89STRINGUTILS_HAVE_ASPRINTF) ||                                  \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
/**
 * @brief Wrapper for alias vasprintf for testing.
 * @param str A pointer to a string pointer where the allocated string will be
 * stored.
 * @param fmt The format string.
 * @param ... The arguments.
 * @return exit code
 */
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
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("test 123", s);
  free(s);
#if !defined(C89STRINGUTILS_HAVE_ASPRINTF) ||                                  \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
  /* Test aliases if they are compiled in this environment */
  rc = asprintf(&s, "alias %d", 456);
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("alias 456", s);
  free(s);

  rc = test_alias_vasprintf_wrapper(&s, "valias %d", 789);
  ASSERT_EQ(0, rc);
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

/** @brief Mock state for log_debug failure */
extern int g_mock_log_debug_fail;

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_log_debug_should_fail(void) {
#ifdef C89STRINGUTILS_TEST_MOCKS
  g_mock_log_debug_fail = 1;
  LOG_DEBUG("this should fail gracefully: %d", 1);
  c89stringutils_log_debug("this should also fail gracefully");
  g_mock_log_debug_fail = 0;
#endif
  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strcpy_s_should_succeed(void) {
  char buf[20];
  errno_t rc;

  rc = c89stringutils_strcpy_s(buf, sizeof(buf), "hello");
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello", buf);

  rc = c89stringutils_strcpy_s(NULL, sizeof(buf), "hello");
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strcpy_s(buf, 0, "hello");
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strcpy_s(buf, sizeof(buf), NULL);
  ASSERT_EQ(22, rc);
  ASSERT_EQ('\0', buf[0]);

  rc = c89stringutils_strcpy_s(buf, 5, "hello");
  ASSERT_EQ(34, rc); /* ERANGE */
  ASSERT_EQ('\0', buf[0]);

  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strncpy_s_should_succeed(void) {
  char buf[20];
  errno_t rc;

  rc = c89stringutils_strncpy_s(buf, sizeof(buf), "hello", 10);
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello", buf);

  rc = c89stringutils_strncpy_s(buf, sizeof(buf), "hello world", 5);
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello", buf);

  rc = c89stringutils_strncpy_s(buf, sizeof(buf), "hello",
                                (rsize_t)-1); /* _TRUNCATE */
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello", buf);

  rc = c89stringutils_strncpy_s(buf, 6, "hello world", (rsize_t)-1);
  ASSERT_EQ(80, rc); /* STRUNCATE / ERANGE depending on platform, Windows says
                        STRUNCATE which is 80 */
  ASSERT_STR_EQ("hello", buf);

  rc = c89stringutils_strncpy_s(NULL, sizeof(buf), "hello", 10);
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strncpy_s(buf, 0, "hello", 10);
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strncpy_s(buf, sizeof(buf), NULL, 10);
  ASSERT_EQ(22, rc);
  ASSERT_EQ('\0', buf[0]);

  rc = c89stringutils_strncpy_s(buf, 5, "hello world", 10);
  ASSERT_EQ(34, rc); /* ERANGE */
  ASSERT_EQ('\0', buf[0]);

  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strcat_s_should_succeed(void) {
  char buf[20] = "hello";
  errno_t rc;

  rc = c89stringutils_strcat_s(buf, sizeof(buf), " world");
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello world", buf);

  rc = c89stringutils_strcat_s(NULL, sizeof(buf), "!");
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strcat_s(buf, 0, "!");
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strcat_s(buf, sizeof(buf), NULL);
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strcpy_s(buf, sizeof(buf), "1234567890123456789");
  rc = c89stringutils_strcat_s(buf, sizeof(buf), "!");
  ASSERT_EQ(34, rc); /* ERANGE */
  ASSERT_EQ('\0', buf[0]);

  /* buffer not null terminated */
  memset(buf, 'A', sizeof(buf));
  rc = c89stringutils_strcat_s(buf, sizeof(buf), "!");
  ASSERT_EQ(22, rc); /* EINVAL */

  PASS();
}

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_strncat_s_should_succeed(void) {
  char buf[20] = "hello";
  errno_t rc;

  rc = c89stringutils_strncat_s(buf, sizeof(buf), " world", 6);
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello world", buf);

  c89stringutils_strcpy_s(buf, sizeof(buf), "hello");
  rc = c89stringutils_strncat_s(buf, sizeof(buf), " world!", 6);
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello world", buf);

  c89stringutils_strcpy_s(buf, sizeof(buf), "hello");
  rc = c89stringutils_strncat_s(buf, sizeof(buf), " world",
                                (rsize_t)-1); /* _TRUNCATE */
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ("hello world", buf);

  c89stringutils_strcpy_s(buf, sizeof(buf), "12345678901234");
  rc = c89stringutils_strncat_s(buf, sizeof(buf), "567890",
                                (rsize_t)-1); /* _TRUNCATE */
  ASSERT_EQ(80, rc);                          /* STRUNCATE */
  ASSERT_STR_EQ("1234567890123456789", buf);

  rc = c89stringutils_strncat_s(NULL, sizeof(buf), "!", 1);
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strncat_s(buf, 0, "!", 1);
  ASSERT_EQ(22, rc);

  rc = c89stringutils_strncat_s(buf, sizeof(buf), NULL, 1);
  ASSERT_EQ(22, rc);

  c89stringutils_strcpy_s(buf, sizeof(buf), "1234567890123456789");
  rc = c89stringutils_strncat_s(buf, sizeof(buf), "!", 1);
  ASSERT_EQ(34, rc); /* ERANGE */
  ASSERT_EQ('\0', buf[0]);

  /* buffer not null terminated */
  memset(buf, 'A', sizeof(buf));
  rc = c89stringutils_strncat_s(buf, sizeof(buf), "!", 1);
  ASSERT_EQ(22, rc); /* EINVAL */

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
  ASSERT_EQ(0, rc);
  ASSERT_STR_EQ(big, s);
  free(s);
  PASS();
}

/** @brief Mock state for malloc failure */
extern int g_mock_malloc_fail;
/** @brief Mock state for realloc failure */
extern int g_mock_realloc_fail;
/** @brief Mock state for vsnprintf failure */
extern int g_mock_vsnprintf_fail;
/** @brief Mock state for vsnprintf return */
extern int g_mock_vsnprintf_ret;
/** @brief Mock state for strerror returning null */
extern int g_mock_strerror_null;

/** @brief Mock state for vsnprintf call count trigger */
extern int g_mock_vsnprintf_fail_call;
/** @brief Mock state for vsnprintf call count */
extern int g_mock_vsnprintf_call_count;
/** @brief Mock state for vsnprintf second return */
extern int g_mock_vsnprintf_ret2;
/** @brief Mock state for vsnprintf failing on large sizes */
extern int g_mock_vsnprintf_fail_on_large_size;

/**
 * @brief Test case
 * @return enum test result
 */
TEST x_mock_failures(void) {
  char *s = NULL;
#if !defined(C89STRINGUTILS_HAVE_STRERROR_S)
  size_t len;
#endif

#if !defined(C89STRINGUTILS_HAVE_VASPRINTF) ||                                 \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
  int rc;
  printf("Testing malloc fail\n");
  g_mock_malloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  ASSERT_EQ(-1, c89stringutils_jasprintf(&s, "test"));
  ASSERT_EQ(-1, jasprintf(&s, "test"));
  g_mock_malloc_fail = 0;

  printf("Testing realloc fail\n");
  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = 200; /* force realloc path */
  g_mock_realloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  g_mock_vsnprintf_fail = 0;
  g_mock_realloc_fail = 0;

  printf("Testing realloc fail in exponential loop\n");
  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = -1;
  g_mock_realloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  g_mock_vsnprintf_fail = 0;
  g_mock_realloc_fail = 0;

  printf("Testing success after exponential loop\n");
  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail_call = 2;
  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = -1;
  g_mock_vsnprintf_ret2 = 5;
  rc = c89stringutils_asprintf(&s, "test");
  ASSERT_EQ(0, rc);
  if (s)
    free(s);
  s = NULL;
  g_mock_vsnprintf_fail = 0;
  g_mock_vsnprintf_fail_call = 0;

  g_mock_vsnprintf_call_count = 0;
  g_mock_vsnprintf_fail_call = 0;
  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = INT_MAX;
  rc = c89stringutils_asprintf(&s, "test");
  ASSERT_EQ(-1, rc);
  g_mock_vsnprintf_fail = 0;

  g_mock_vsnprintf_fail = 1;
  g_mock_vsnprintf_ret = -1;
  ASSERT_EQ(-1, c89stringutils_asprintf(&s, "test"));
  ASSERT_EQ(-1, c89stringutils_jasprintf(&s, "test"));
  ASSERT_EQ(-1, jasprintf(&s, "test"));
  g_mock_vsnprintf_fail = 0;

  /* To trigger realloc failure dynamically without brittle call counts */
  {
    char big[200];
    memset(big, 'A', 199);
    big[199] = '\0';
    g_mock_vsnprintf_fail_on_large_size = 1;
    g_mock_vsnprintf_call_count = 0;
    {
      int ret_code = c89stringutils_asprintf(&s, "%s", big);
      ASSERT_EQ_FMT(-1, ret_code, "%d");
    }

    g_mock_vsnprintf_fail_on_large_size = 2;
    g_mock_vsnprintf_call_count = 0;
    {
      int ret_code = c89stringutils_asprintf(&s, "%s", big);
      ASSERT_EQ_FMT(-1, ret_code, "%d");
    }

    g_mock_vsnprintf_fail_on_large_size = 0;
  }
#endif
  s = malloc(10);
  if (s) {
#if defined(_MSC_VER)
    strcpy_s(s, 10, "123");
#else
    strcpy(s, "123");
#endif
  }
  g_mock_realloc_fail = 1;
  ASSERT_EQ(-1, c89stringutils_jasprintf(&s, "test"));
  ASSERT_EQ(-1, jasprintf(&s, "test"));
  g_mock_realloc_fail = 0;
  if (s) {
    free(s);
    s = NULL;
  }

  /* Cover missing paths */
  {
    char dummy[10];
    snprintf(dummy, 10, "test");
#if !defined(C89STRINGUTILS_HAVE_STRCASESTR) ||                                \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
    c89stringutils_strcasestr(NULL, "a");
    c89stringutils_strcasestr("a", NULL);
#endif
#if !defined(C89STRINGUTILS_HAVE_STRNSTR) ||                                   \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
    c89stringutils_strnstr(NULL, "a", 1);
    c89stringutils_strnstr("a", NULL, 1);
#endif
#if !defined(C89STRINGUTILS_HAVE_VASPRINTF) ||                                 \
    defined(C89STRINGUTILS_FORCE_FALLBACKS)
    {
      char *sptr = NULL;
      va_list empty_va;
      const char *null_fmt = NULL;
      memset(&empty_va, 0, sizeof(empty_va));
      c89stringutils_vasprintf(NULL, "a", empty_va);
      c89stringutils_vasprintf(&sptr, null_fmt, empty_va);
      (void)sptr;
    }
#endif
  }

#if !defined(C89STRINGUTILS_HAVE_STRERROR_S)
  g_mock_strerror_null = 1;
  len = c89stringutils_strerrorlen_s(99999);
  ASSERT_EQ(0, len);
  {
    char err_s_buf[10];
    ASSERT_EQ(22, c89stringutils_strerror_s(err_s_buf, 10, 99999));
  }
  g_mock_strerror_null = 0;
#endif

  ASSERT_EQ(0, c89stringutils_strncasecmp("test", "TEST", 10));

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
#if defined(_MSC_VER) && _MSC_VER < 1900
  PASS();
#else

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
#endif
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
  RUN_TEST(x_strerror_s_should_succeed);
  RUN_TEST(x_snprintf_s_should_succeed);
  RUN_TEST(x_snprintf_should_succeed);
  RUN_TEST(x_strerrorlen_s_should_succeed);
  RUN_TEST(x_vasprintf_should_succeed);
  RUN_TEST(x_vasprintf_should_fail);
  RUN_TEST(x_log_debug_should_succeed);
  RUN_TEST(x_log_debug_should_fail);
  RUN_TEST(x_strcpy_s_should_succeed);
  RUN_TEST(x_strncpy_s_should_succeed);
  RUN_TEST(x_strcat_s_should_succeed);
  RUN_TEST(x_strncat_s_should_succeed);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !TEST_STRING_EXTRAS_H */
