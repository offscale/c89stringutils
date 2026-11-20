/**
 * @file test_lib.c
 * @brief Integration test verifying linkage against the built c89stringutils
 * static or shared library target.
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
/* clang-format off */
#include <c89stringutils_string_extras.h>
#include <c89stringutils_safecrt.h>
#include <c89stringutils_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* clang-format on */

int main(void) {
  char *dyn = NULL;
  char buf[32];
  errno_t err_rc;
  int rc;
  int scanned = 0;

  rc = c89stringutils_asprintf(&dyn, "val=%d", 42);
  if (rc != 0 || dyn == NULL || strcmp(dyn, "val=42") != 0) {
    free(dyn);
    return 1;
  }

  rc = c89stringutils_jasprintf(&dyn, ",%s", "ok");
  if (rc != 0 || strcmp(dyn, "val=42,ok") != 0) {
    free(dyn);
    return 2;
  }
  free(dyn);

  rc = c89stringutils_snprintf(buf, sizeof(buf), "sn=%d", 7);
  if (rc != 4 || strcmp(buf, "sn=7") != 0) {
    return 3;
  }

  err_rc = c89stringutils_strcpy_s(buf, sizeof(buf), "copied");
  if (err_rc != (errno_t)C89STRINGUTILS_SUCCESS || strcmp(buf, "copied") != 0) {
    return 4;
  }

  rc = c89stringutils_sscanf_s("99", "%d", &scanned);
  if (rc != 1 || scanned != 99) {
    return 5;
  }

  if (c89stringutils_strerrorlen_s(400) == 0) {
    return 6;
  }

  return 0;
}
