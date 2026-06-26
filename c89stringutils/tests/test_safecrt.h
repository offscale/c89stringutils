#ifndef TEST_SAFECRT_H
#define TEST_SAFECRT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* clang-format off */
#include <c89stringutils_safecrt.h>
#include <errno.h>
#include <greatest.h>
#include <wchar.h>
#include <setjmp.h>
/* clang-format on */

static int g_constraint_hit = 0;
jmp_buf g_abort_env;
int g_aborted = 0;

void mock_abort(void) {
  g_aborted = 1;
  longjmp(g_abort_env, 1);
}

static void test_handler(const char *msg, void *ptr, errno_t error) {
  (void)msg;
  (void)ptr;
  (void)error;
  g_constraint_hit++;
}

TEST test_set_constraint_handler(void) {
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(test_handler);
  c89stringutils_invoke_constraint_handler_s("test", NULL, 1);
  ASSERT_EQ(1, g_constraint_hit);
  c89stringutils_set_constraint_handler_s(old);

  old = c89stringutils_set_constraint_handler_s(NULL);
  if (setjmp(g_abort_env) == 0) {
    c89stringutils_invoke_constraint_handler_s("abort handler", NULL, 1);
    FAIL();
  } else {
    ASSERT_EQ(1, g_aborted);
  }

  g_aborted = 0;
  if (setjmp(g_abort_env) == 0) {
    c89stringutils_invoke_constraint_handler_s(NULL, NULL, 1);
    FAIL();
  } else {
    ASSERT_EQ(1, g_aborted);
  }

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_ignore_handler(void) {
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);
  c89stringutils_invoke_constraint_handler_s("test ignore", NULL, 1);
  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_sscanf_s(void) {
  int val = 0;
  int rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  rc = c89stringutils_sscanf_s("123", "%d", &val);
  ASSERT_EQ(1, rc);
  ASSERT_EQ(123, val);

  rc = c89stringutils_sscanf_s(NULL, "%d", &val);
  ASSERT_EQ(-1, rc);

  rc = c89stringutils_sscanf_s("123", NULL);
  ASSERT_EQ(-1, rc);

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_fscanf_s(void) {
  int val = 0;
  int rc;
  FILE *f;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  c89stringutils_fopen_s(&f, "test_fscanf.txt", "w+");
  fprintf(f, "456");
  rewind(f);

  rc = c89stringutils_fscanf_s(f, "%d", &val);
  ASSERT_EQ(1, rc);
  ASSERT_EQ(456, val);

  fclose(f);
  remove("test_fscanf.txt");

  rc = c89stringutils_fscanf_s(NULL, "%d", &val);
  ASSERT_EQ(-1, rc);

  rc = c89stringutils_fscanf_s(f, NULL);
  ASSERT_EQ(-1, rc);

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

extern int g_mock_fopen_fail;

TEST test_fopen_s(void) {
  FILE *f = NULL;
  errno_t rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  rc = c89stringutils_fopen_s(&f, "test_fopen.txt", "w");
  ASSERT_EQ(0, rc);
  ASSERT(f != NULL);
  fclose(f);
  remove("test_fopen.txt");

  rc = c89stringutils_fopen_s(NULL, "test.txt", "w");
  ASSERT(rc != 0);
  rc = c89stringutils_fopen_s(&f, NULL, "w");
  ASSERT(rc != 0);
  ASSERT_EQ(NULL, f);
  rc = c89stringutils_fopen_s(&f, "test.txt", NULL);
  ASSERT(rc != 0);
  ASSERT_EQ(NULL, f);

  /* force fopen failure */
  rc = c89stringutils_fopen_s(&f, "/does_not_exist/test.txt", "r");
  ASSERT(rc != 0);

#if !defined(C89STRINGUTILS_HAVE_FOPEN_S)
  g_mock_fopen_fail = 1;
  rc = c89stringutils_fopen_s(&f, "test.txt", "w");
  ASSERT(rc != 0);
  g_mock_fopen_fail = 0;
#endif

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_freopen_s(void) {
  FILE *f = NULL;
  FILE *f2 = NULL;
  errno_t rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  c89stringutils_fopen_s(&f, "test_freopen.txt", "w");
  rc = c89stringutils_freopen_s(&f2, "test_freopen2.txt", "w", f);
  ASSERT_EQ(0, rc);
  ASSERT(f2 != NULL);
  fclose(f2);
  remove("test_freopen.txt");
  remove("test_freopen2.txt");

  rc = c89stringutils_freopen_s(NULL, "t.txt", "w", NULL);
  ASSERT(rc != 0);
  rc = c89stringutils_freopen_s(&f2, NULL, "w", f);
  ASSERT(rc != 0);
  ASSERT_EQ(NULL, f2);
  rc = c89stringutils_freopen_s(&f2, "t.txt", NULL, f);
  ASSERT(rc != 0);
  ASSERT_EQ(NULL, f2);
  rc = c89stringutils_freopen_s(&f2, "t.txt", "w", NULL);
  ASSERT(rc != 0);
  ASSERT_EQ(NULL, f2);

#if !defined(C89STRINGUTILS_HAVE_FREOPEN_S)
  g_mock_fopen_fail = 1;
  c89stringutils_fopen_s(
      &f, "test_freopen.txt",
      "w"); /* Just need a valid FILE* on some systems, though it might fail
               here. Actually, we just need freopen to fail. */
  if (!f) {
    g_mock_fopen_fail = 0;
    c89stringutils_fopen_s(&f, "test_freopen.txt", "w");
    g_mock_fopen_fail = 1;
  }
  rc = c89stringutils_freopen_s(&f2, "test_freopen2.txt", "w", f);
  ASSERT(rc != 0);
  if (f)
    fclose(f);
  remove("test_freopen.txt");
  g_mock_fopen_fail = 0;
#endif

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_tmpfile_s(void) {
  FILE *f = NULL;
  errno_t rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  rc = c89stringutils_tmpfile_s(&f);
  ASSERT_EQ(0, rc);
  ASSERT(f != NULL);
  fclose(f);

  rc = c89stringutils_tmpfile_s(NULL);
  ASSERT(rc != 0);

#if !defined(C89STRINGUTILS_HAVE_TMPFILE_S)
  g_mock_fopen_fail = 1;
  rc = c89stringutils_tmpfile_s(&f);
  ASSERT(rc != 0);
  g_mock_fopen_fail = 0;
#endif

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_wcscpy_s(void) {
  wchar_t dest[10];
  errno_t rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  rc = c89stringutils_wcscpy_s(dest, 10, L"test");
  ASSERT_EQ(0, rc);
  ASSERT(wcscmp(dest, L"test") == 0);

  rc = c89stringutils_wcscpy_s(NULL, 10, L"test");
  ASSERT(rc != 0);

  rc = c89stringutils_wcscpy_s(dest, 10, NULL);
  ASSERT(rc != 0);

  rc = c89stringutils_wcscpy_s(dest, 2, L"test");
  ASSERT(rc != 0);

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

TEST test_wcsncpy_s(void) {
  wchar_t dest[10];
  errno_t rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  rc = c89stringutils_wcsncpy_s(dest, 10, L"test", 8);
  ASSERT_EQ(0, rc);

  rc = c89stringutils_wcsncpy_s(dest, 10, L"test", 2);
  ASSERT_EQ(0, rc);
  ASSERT(wcscmp(dest, L"te") == 0);

  rc = c89stringutils_wcsncpy_s(dest, 10, L"test", (size_t)-1);
  ASSERT_EQ(0, rc);

  rc = c89stringutils_wcsncpy_s(dest, 3, L"test", (size_t)-1);
  ASSERT_EQ(80, rc);

  rc = c89stringutils_wcsncpy_s(NULL, 10, L"test", 4);
  ASSERT(rc != 0);

  rc = c89stringutils_wcsncpy_s(dest, 10, NULL, 4);
  ASSERT(rc != 0);

  rc = c89stringutils_wcsncpy_s(dest, 2, L"test", 4);
  ASSERT(rc != 0);

  rc = c89stringutils_wcsncpy_s(dest, 2, L"test", (size_t)-1);
  ASSERT(rc != 0);

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

static int test_vswprintf_wrapper(wchar_t *buffer, size_t sizeOfBuffer,
                                  const wchar_t *format, ...) {
  int rc;
  va_list args;
  va_start(args, format);
  rc = c89stringutils_vswprintf_s(buffer, sizeOfBuffer, format, args);
  va_end(args);
  return rc;
}

TEST test_swprintf_s(void) {
  wchar_t dest[20];
  int rc;
  c89stringutils_constraint_handler_t old =
      c89stringutils_set_constraint_handler_s(c89stringutils_ignore_handler_s);

  rc = c89stringutils_swprintf_s(dest, 20, L"test %d", 123);
  if (rc < 0) {
    c89stringutils_set_constraint_handler_s(old);
    SKIP();
  }
  ASSERT(rc > 0);
  ASSERT(wcscmp(dest, L"test 123") == 0);

  rc = test_vswprintf_wrapper(dest, 20, L"test %d", 123);
  ASSERT(rc > 0);

  rc = c89stringutils_swprintf_s(NULL, 20, L"test %d", 123);
  ASSERT(rc < 0);

  rc = c89stringutils_swprintf_s(dest, 2, L"test %d", 123);
  ASSERT(rc < 0);

  c89stringutils_set_constraint_handler_s(old);
  PASS();
}

SUITE(safecrt_suite) {
  RUN_TEST(test_set_constraint_handler);
  RUN_TEST(test_ignore_handler);
  RUN_TEST(test_sscanf_s);
  RUN_TEST(test_fscanf_s);
  RUN_TEST(test_fopen_s);
  RUN_TEST(test_freopen_s);
  RUN_TEST(test_tmpfile_s);
  RUN_TEST(test_wcscpy_s);
  RUN_TEST(test_wcsncpy_s);
  RUN_TEST(test_swprintf_s);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
